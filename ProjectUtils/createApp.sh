#!/bin/bash
set -e

#usage example:
# ./createApp.sh ../build ../App WxEditorExe
# ./createApp.sh ../build ../App GameExe

INPUT_DIR="$1"       # katalog buildu z binarkami i lib
OUTPUT_DIR="$2"      # gdzie zapisać AppImage

create_appimage() {
    local input_dir="$1"
    local output_dir="$2"
    local bin_name="$3"

    # unikalne nazwy dla AppDir i AppImage wg binarki
    local appname="${bin_name}App"
    local appdir="$output_dir/$appname.AppDir"

    mkdir -p "$appdir/usr/bin" "$appdir/usr/lib"
    echo "👉 Tworzę AppDir w $appdir ..."

    # Kopiowanie tylko wybranej binarki
    local bin_path
    bin_path=$(find "$input_dir" -type f -name "$bin_name" -executable | head -n 1)
    if [[ -z "$bin_path" ]]; then
        echo "❌ Nie znaleziono binarki $bin_name w $input_dir"
        return 1
    fi
    cp "$bin_path" "$appdir/usr/bin/"
    chmod +x "$appdir/usr/bin/$bin_name"

    # Kopiowanie bibliotek (.so*) do usr/lib
    find "$input_dir" -type f -name "*.so*" -exec cp {} "$appdir/usr/lib/" \;

    # Tworzymy AppRun z poprawnym LD_LIBRARY_PATH
    cat > "$appdir/AppRun" <<EOF
#!/bin/bash
HERE="\$(dirname "\$(readlink -f "\${0}")")"
export LD_LIBRARY_PATH="\$HERE/usr/lib:\$LD_LIBRARY_PATH"
exec "\$HERE/usr/bin/$bin_name" "\$@"
EOF
    chmod +x "$appdir/AppRun"

    # Plik .desktop
    cat > "$appdir/$appname.desktop" <<EOF
[Desktop Entry]
Name=$appname
Exec=$bin_name
Icon=$appname
Type=Application
Categories=Game;
EOF

    # Dummy ikona
    convert -size 256x256 xc:white -gravity center -pointsize 24 -fill black -annotate +0+0 "$bin_name" "$appdir/$appname.png"

    # Generowanie AppImage
    echo "🚀 Generuję AppImage dla $bin_name..."
    ARCH=x86_64 ./appimagetool-x86_64.AppImage "$appdir" "$output_dir/$appname-x86_64.AppImage"
}

# Wywołania przykładowe
create_appimage "$INPUT_DIR" "$OUTPUT_DIR" "WxEditorExe"
create_appimage "$INPUT_DIR" "$OUTPUT_DIR" "GameExe"

