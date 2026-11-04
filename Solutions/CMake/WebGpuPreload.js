
(function () {
    Module['preRun'] = Module['preRun'] || [];

    Module['preRun'].push(function () {

        const dependency = "bengine-game-assets";

        const PACKAGE_URL = "http://192.168.0.56:800/game.zip";
        const CHUNK_SIZE = 64 * 1024;

        addRunDependency(dependency);

        function loadFflate() {
            if (globalThis.fflate) {
                return Promise.resolve();
            }

            return new Promise((resolve, reject) => {
                const script = document.createElement("script");

                script.src = new URL(
                    "fflate.js",
                    document.baseURI
                ).href;

                script.onload = resolve;
                script.onerror = () => reject(
                    new Error("Nie można załadować fflate.js")
                );

                document.head.appendChild(script);
            });
        }

        function safePath(name) {
            const normalized = name.replace(/\\/g, "/");

            if (
                !normalized ||
                normalized.startsWith("/") ||
                /^[a-zA-Z]:/.test(normalized) ||
                normalized.includes("\0")
            ) {
                throw new Error("Nieprawidłowa ścieżka ZIP: " + name);
            }

            const parts = normalized.split("/");

            if (parts.some(p => p === "." || p === "..")) {
                throw new Error("Niedozwolona ścieżka ZIP: " + name);
            }

            return "/" + parts.filter(Boolean).join("/");
        }

        async function preloadGameAssets() {

            await loadFflate();

            console.log("BEngine: downloading game assets...");

            const response = await fetch(PACKAGE_URL, {
                cache: "no-cache"
            });

            if (!response.ok) {
                throw new Error(
                    "HTTP " + response.status +
                    " podczas pobierania game.zip"
                );
            }

            if (!response.body) {
                throw new Error(
                    "Przeglądarka nie obsługuje strumieniowego pobierania"
                );
            }

            const totalBytes =
                Number(response.headers.get("Content-Length")) || 0;

            const reader = response.body.getReader();

            const zip = globalThis.fflate;

            if (!zip.Unzip || !zip.UnzipInflate) {
                throw new Error("Brak obsługi streaming ZIP w fflate");
            }

            let downloadedBytes = 0;
            let extractedBytes = 0;
            let extractedFiles = 0;
            let lastProgressTime = 0;
            let extractionError = null;

            // Streaming ZIP.
            // UnzipInflate działa synchronicznie dla kolejnych fragmentów,
            // dzięki czemu nie tworzymy kolejki nieskończonej liczby
            // zadań w dodatkowych Workerach.

            const unzipper = new zip.Unzip(file => {

                if (extractionError) {
                    return;
                }

                let stream = null;
                let streamOpen = false;

                try {

                    const path = safePath(file.name);

                    const isDirectory = file.name.endsWith("/");

                    if (isDirectory) {

                        FS.mkdirTree(path);

                        file.ondata = (err) => {
                            if (err) {
                                extractionError = err;
                            }
                        };

                        file.start();
                        return;
                    }

                    const lastSlash = path.lastIndexOf("/");

                    const parent =
                        lastSlash <= 0
                            ? "/"
                            : path.substring(0, lastSlash);

                    FS.mkdirTree(parent);

                    stream = FS.open(path, "w");
                    streamOpen = true;

                    file.ondata = (err, data, final) => {

                        if (err) {
                            extractionError = err;

                            if (streamOpen) {
                                FS.close(stream);
                                streamOpen = false;
                            }

                            return;
                        }

                        try {

                            if (data && data.length) {

                                const written = FS.write(
                                    stream,
                                    data,
                                    0,
                                    data.length
                                );

                                if (written !== data.length) {
                                    throw new Error(
                                        "Niepełny zapis pliku: " + path
                                    );
                                }

                                extractedBytes += written;
                            }

                            if (final) {

                                FS.close(stream);
                                streamOpen = false;

                                extractedFiles++;
                            }

                        } catch (error) {

                            extractionError = error;

                            if (streamOpen) {
                                FS.close(stream);
                                streamOpen = false;
                            }
                        }
                    };

                    file.start();

                } catch (error) {

                    extractionError = error;

                    if (streamOpen) {
                        FS.close(stream);
                    }
                }
            });

            // Standardowy ZIP: DEFLATE (8) oraz STORE (0).
            unzipper.register(zip.UnzipInflate);

            try {

                while (true) {

                    const result = await reader.read();

                    if (result.done) {
                        break;
                    }

                    const chunk = result.value;

                    downloadedBytes += chunk.length;

                    // Dzielimy także większe fragmenty sieciowe
                    // na mniejsze kawałki.

                    for (
                        let offset = 0;
                        offset < chunk.length;
                        offset += CHUNK_SIZE
                    ) {

                        if (extractionError) {
                            throw extractionError;
                        }

                        const part = chunk.subarray(
                            offset,
                            Math.min(
                                offset + CHUNK_SIZE,
                                chunk.length
                            )
                        );

                        unzipper.push(part, false);
                    }

                    if (extractionError) {
                        throw extractionError;
                    }

                    // Aktualizacja postępu co około sekundę.
                    const now = Date.now();

                    if (now - lastProgressTime >= 1000) {

                        const percent = totalBytes
                            ? (
                                downloadedBytes / totalBytes * 100
                            ).toFixed(1)
                            : "?";

                        console.log(
                            "BEngine: ZIP " + percent + "% | " +
                            (downloadedBytes / 1024 / 1024).toFixed(1) +
                            " MB pobrane | " +
                            extractedFiles + " plików rozpakowanych"
                        );

                        lastProgressTime = now;
                    }
                }

                // Koniec archiwum.
                unzipper.push(new Uint8Array(0), true);

                if (extractionError) {
                    throw extractionError;
                }

            } finally {
                reader.releaseLock();
            }

            // Kontrola kompletności.
            FS.stat("/config.xml");
            FS.stat("/scenes.json");
            FS.stat("/Data");
            FS.stat("/Scenes");

            console.log("==================================");
            console.log("BEngine: ASSETS READY");
            console.log("Files:", extractedFiles);
            console.log(
                "Unpacked:",
                (extractedBytes / 1024 / 1024 / 1024).toFixed(2),
                "GiB"
            );
            console.log("==================================");
        }

        preloadGameAssets()
            .then(() => {

                console.log("BEngine: starting application");

                removeRunDependency(dependency);

            })
            .catch(error => {

                console.error(
                    "BEngine: asset initialization failed:",
                    error
                );

                abort(
                    "BEngine: nie udało się pobrać lub rozpakować zasobów"
                );
            });
    });
})();
