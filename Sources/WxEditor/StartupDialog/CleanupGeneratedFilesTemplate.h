#pragma once

inline const char* CLEANUP_FILES_SH = R"bash(
#!/bin/bash

TARGET_DIR="Data/generated"
SEARCH_IN="."

# Weryfikacja katalogu
if [ ! -d "$TARGET_DIR" ]; then
    echo "Błąd: Katalog $TARGET_DIR nie istnieje."
    exit 1
fi

total_files=$(find "$TARGET_DIR" -maxdepth 1 -type f | wc -l)
if [ "$total_files" -eq 0 ]; then echo "Katalog $TARGET_DIR jest pusty."; exit 0; fi

unused_count=0
unused_total_size=0
current_idx=0
unused_list=()
unused_paths=() # Dodatkowa tablica na pełne ścieżki do usuwania

echo "Analiza plików w: $TARGET_DIR..."
echo "--------------------------------------------------------------------------"

for file_path in "$TARGET_DIR"/*; do
    if [ -f "$file_path" ]; then
        current_idx=$((current_idx + 1))
        filename=$(basename "$file_path")

        # --- LOGIKA OBSŁUGI ZALEŻNOŚCI ---
        # Domyślnie szukamy pełnej nazwy pliku
        search_pattern="$filename"

        # Jeśli plik zawiera w nazwie "_config", szukamy nazwy bez tego członu
        suffixes="_config|_meta|.meta|_props|Config"
        if [[ "$filename" =~ ($suffixes) ]]; then
            # Usuwamy suffix, aby szukać bazy (np. .terrain)
            search_pattern=$(echo "$filename" | sed -E "s/($suffixes)//")
        fi
        # ----------------------------------

        # Statystyki do paska postępu
        percent=$((current_idx * 100 / total_files))
        current_readable_gain=$(numfmt --to=iec --suffix=B $unused_total_size 2>/dev/null || echo "${unused_total_size}B")

        display_name="${filename:0:20}"
        [ ${#filename} -gt 20 ] && display_name="${display_name}..."

        printf "\r\e[K[%-15s] %d%% | Nieużywane: %d | Odzyskasz: %s | Sprawdzam: %-23s" \
            $(printf "#%.0s" $(seq 1 $((percent / 7)))) \
            "$percent" "$unused_count" "$current_readable_gain" "$display_name"

        # Szukamy wzorca (search_pattern) w projekcie
        if ! grep -rlm 1 "$search_pattern" "$SEARCH_IN" --exclude-dir="$TARGET_DIR" --exclude-dir=".git" > /dev/null 2>&1; then
            # Jeśli wzorzec nie został znaleziony, plik uznajemy za nieużywany
            f_size=$(stat -c%s "$file_path" 2>/dev/null || stat -f%z "$file_path" 2>/dev/null || echo 0)
            unused_total_size=$((unused_total_size + f_size))
            unused_count=$((unused_count + 1))
            unused_list+=("$filename ($(numfmt --to=iec $f_size 2>/dev/null || echo $f_size))")
            unused_paths+=("$file_path")
        fi
    fi
done

echo -e "\n--------------------------------------------------------------------------"
echo "ZAKOŃCZONO ANALIZĘ"

if [ $unused_count -gt 0 ]; then
    echo -e "\nLISTA NIEUŻYWANYCH PLIKÓW:"
    printf '%s\n' "${unused_list[@]}" | column -t

    final_size=$(numfmt --to=iec --suffix=B $unused_total_size 2>/dev/null || echo $unused_total_size)
    echo -e "\nPodsumowanie: Znaleziono $unused_count plików o łącznej wadze $final_size."
    echo "--------------------------------------------------------------------------"

    # Pytanie o usunięcie
    read -p "Czy chcesz usunąć wszystkie nieużywane pliki? (t/n): " choice

    case "$choice" in
        t|T|y|Y )
            echo "Usuwanie plików..."
            for path in "${unused_paths[@]}"; do
                rm "$path"
                echo "Usunięto: $path"
            done
            echo "Gotowe. Odzyskano $final_size miejsca."
            ;;
        * )
            echo "Anulowano. Żadne pliki nie zostały usunięte."
            ;;
    esac
else
    echo "Wszystkie pliki są w użyciu. Nie ma nic do usuwania."
fi
)bash";
