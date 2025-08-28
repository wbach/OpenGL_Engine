#!/bin/bash
set -e

#usage example  from ProjectUtils dir run : ./createApp.sh  ../build ../App

INPUT_DIR="$1"       # katalog buildu z binarkami i lib
OUTPUT_DIR="$2"      # gdzie zapisać AppImage
APPNAME="MyGame"
APPDIR="$OUTPUT_DIR/$APPNAME.AppDir"

mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/lib"

echo "👉 Tworzę AppDir w $APPDIR ..."

# Kopiowanie binarek ELF do usr/bin
# ------------------------------
find "$INPUT_DIR" -type f -exec file {} \; | grep 'ELF.*executable' | cut -d: -f1 | while read -r bin; do
    cp "$bin" "$APPDIR/usr/bin/"
    chmod +x "$APPDIR/usr/bin/$(basename "$bin")"
done

# ------------------------------
# Kopiowanie bibliotek (.so*) do usr/lib
# ------------------------------
find "$INPUT_DIR" -type f -name "*.so*" -exec cp {} "$APPDIR/usr/lib/" \;
# Ustawienie praw wykonywania dla binarek
chmod +x "$APPDIR/usr/bin/"*


# Tworzymy AppRun z poprawnym LD_LIBRARY_PATH
cat > "$APPDIR/AppRun" <<'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="$HERE/usr/lib:$LD_LIBRARY_PATH"
exec "$HERE/usr/bin/WxEditorExe" "$@"
EOF
chmod +x "$APPDIR/AppRun"

# Plik .desktop
cat > "$APPDIR/$APPNAME.desktop" <<EOF
[Desktop Entry]
Name=$APPNAME
Exec=WxEditorExe
Icon=$APPNAME
Type=Application
Categories=Game;
EOF

# Dummy ikona
convert -size 256x256 xc:white -gravity center -pointsize 24 -fill black -annotate +0+0 "GameEditor" "$APPDIR/$APPNAME.png"

# Generowanie AppImage
echo "🚀 Generuję AppImage..."
ARCH=x86_64 ./appimagetool-x86_64.AppImage "$APPDIR" "$OUTPUT_DIR/$APPNAME-x86_64.AppImage"

