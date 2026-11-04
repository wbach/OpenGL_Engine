#!/usr/bin/env python3
"""Serve a WebGPU/Emscripten build with the headers required by pthreads.

Usage:
    python3 serve_webgpu.py /path/to/WebGpuGame
    python3 serve_webgpu.py /path/to/WebGpuGame --port 8000
    python3 serve_webgpu.py /path/to/WebGpuGame --host 0.0.0.0
"""

from __future__ import annotations

import argparse
import mimetypes
import sys
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path


class WebGPURequestHandler(SimpleHTTPRequestHandler):
    """Static file handler with cross-origin isolation headers."""

    def end_headers(self) -> None:
        # Required by Emscripten pthreads / SharedArrayBuffer.
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")

        # Avoid stale Game.js/Game.wasm while developing.
        self.send_header("Cache-Control", "no-store")
        super().end_headers()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Serve a WebGPU build with COOP/COEP headers enabled "
            "for Emscripten pthreads."
        )
    )
    parser.add_argument(
        "root_dir",
        type=Path,
        help="Directory to serve (the directory containing Game.html).",
    )
    parser.add_argument(
        "--host",
        default="127.0.0.1",
        help="Bind address. Use 0.0.0.0 to allow LAN access (default: 127.0.0.1).",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=8000,
        help="TCP port (default: 8000).",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    try:
        root_dir = args.root_dir.expanduser().resolve(strict=True)
    except FileNotFoundError:
        print(f"Error: directory does not exist: {args.root_dir}", file=sys.stderr)
        return 2

    if not root_dir.is_dir():
        print(f"Error: not a directory: {root_dir}", file=sys.stderr)
        return 2

    if not (root_dir / "Game.html").is_file():
        print(
            f"Warning: {root_dir / 'Game.html'} not found. "
            "The directory will still be served.",
            file=sys.stderr,
        )

    # Ensure common WebAssembly content types are served correctly.
    mimetypes.add_type("application/wasm", ".wasm")
    mimetypes.add_type("application/zip", ".zip")

    handler = partial(WebGPURequestHandler, directory=str(root_dir))

    try:
        server = ThreadingHTTPServer((args.host, args.port), handler)
    except OSError as exc:
        print(f"Error: cannot start server on {args.host}:{args.port}: {exc}", file=sys.stderr)
        return 1

    display_host = "localhost" if args.host in ("0.0.0.0", "::") else args.host
    print(f"Serving: {root_dir}")
    print(f"URL:     http://{display_host}:{args.port}/Game.html")
    print("Headers: COOP=same-origin, COEP=require-corp")
    print("Press Ctrl+C to stop.")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping WebGPU development server...")
    finally:
        server.server_close()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
