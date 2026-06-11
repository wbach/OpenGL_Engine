import os
import subprocess
import sys
from pathlib import Path

# Konfiguracja rozszerzeń plików
VERTEX_EXTENSIONS = {'.vert', '.vs'}
FRAGMENT_EXTENSIONS = {'.frag', '.fs'}

def convert_shader(file_path, input_dir, output_dir):
    file_path = Path(file_path)
    ext = file_path.suffix

    if ext not in VERTEX_EXTENSIONS and ext not in FRAGMENT_EXTENSIONS:
        return

    # Zachowanie struktury podkatalogów w folderze docelowym
    relative_path = file_path.relative_to(input_dir)
    vulkan_glsl_output = Path(output_dir) / relative_path

    # Tworzenie podkatalogów w folderze docelowym, jeśli są potrzebne
    vulkan_glsl_output.parent.mkdir(parents=True, exist_ok=True)

    print(f"Przetwarzanie: {relative_path}")

    # Określenie typu shadera dla glslang
    stage = "vert" if ext in VERTEX_EXTENSIONS else "frag"

    # Ścieżka tymczasowa dla SPIR-V (tworzona obok pliku wynikowego)
    spirv_temp = vulkan_glsl_output.with_suffix(".tmp.spv")

    try:
        # KROK 1: Kompilacja OpenGL GLSL do SPIR-V
        subprocess.run([
            "glslangValidator",
            "-V",  # Tryb OpenGL do SPIR-V
            "--auto-map-locations",
            "--auto-map-bindings",
            "-S", stage,
            "-o", str(spirv_temp),
            str(file_path)
        ], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # KROK 2: Dekompilacja SPIR-V do Vulkan GLSL za pomocą SPIRV-Cross
        subprocess.run([
            "spirv-cross",
            "--vulkan-semantics",
            "--output", str(vulkan_glsl_output),
            str(spirv_temp)
        ], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        print(f"  [SUKCES] Zapisano w: {vulkan_glsl_output}")

    except subprocess.CalledProcessError as e:
        print(f"  [BŁĄD] Nie udało się przekonwertować {relative_path}")
        if e.stderr:
            print(e.stderr.decode('utf-8'))
    finally:
        # Czyszczenie pliku tymczasowego SPIR-V
        if spirv_temp.exists():
            spirv_temp.unlink()

def main():
    # Sprawdzenie poprawności argumentów
    if len(sys.argv) < 3:
        print("Błąd: Niepoprawna liczba argumentów.")
        print("Użycie: python convert.py <katalog_zrodlowy> <katalog_docelowy>")
        return

    input_dir = Path(sys.argv[1])
    output_dir = Path(sys.argv[2])

    if not input_dir.is_dir():
        print(f"Błąd: Katalog źródłowy '{input_dir}' nie istnieje lub nie jest katalogiem.")
        return

    print(f"Katalog źródłowy: {input_dir.resolve()}")
    print(f"Katalog docelowy: {output_dir.resolve()}")
    print("-" * 50)

    # Przeszukiwanie katalogu źródłowego
    for root, _, files in os.walk(input_dir):
        for file in files:
            full_path = Path(root) / file
            convert_shader(full_path, input_dir, output_dir)

if __name__ == "__main__":
    main()