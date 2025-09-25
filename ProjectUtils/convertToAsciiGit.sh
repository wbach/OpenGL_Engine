#!/bin/bash

# Pobieramy zmienione pliki (staged i unstaged)
git diff HEAD --name-only --diff-filter=ACMRTUXB | while read -r file; do
    # Tylko pliki zwykle
    [ -f "$file" ] || continue

    # Sprawdzamy encoding
    enc=$(file -bi "$file" | sed -n 's/.*charset=\(.*\)$/\1/p')

    # Konwertujemy jesli nie jest ASCII
    if [ "$enc" != "us-ascii" ]; then
        iconv -f utf-8 -t ascii//TRANSLIT "$file" -o tmp && mv tmp "$file"
        echo "Converted $file from $enc to ASCII"
    fi
done