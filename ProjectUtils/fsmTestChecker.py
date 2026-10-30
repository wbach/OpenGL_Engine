import os
import re
import argparse
from pathlib import Path

def extract_events_from_header(header_path):
    """Wczytuje plik nagłówkowy i wyciąga nazwy wszystkich structów (eventów)."""
    if not os.path.exists(header_path):
        raise FileNotFoundError(f"Nie znaleziono pliku nagłówkowego: {header_path}")

    with open(header_path, 'r', encoding='utf-8') as f:
        content = f.read()

    structs = re.findall(r'struct\s+([A-Za-z0-9_]+)\s*\{', content)
    return set(structs)

def extract_state_coverage(test_dir, events):
    """
    Skanuje katalog z testami i przypisuje przetestowane eventy do odpowiednich stanów.
    """
    test_pattern = re.compile(r'TEST_?F?\s*\(\s*[^,]+,\s*([A-Za-z0-9_]+)\s*\)')
    state_coverage = {}

    if not os.path.exists(test_dir):
        raise FileNotFoundError(f"Nie znaleziono katalogu z testami: {test_dir}")

    for root, _, files in os.walk(test_dir):
        for file in files:
            if file.endswith(('.cpp', '.cc', '.h', '.hpp')):
                file_path = Path(root) / file
                state_name = file_path.stem  # Nazwa pliku jako stan

                if state_name not in state_coverage:
                    state_coverage[state_name] = set()

                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        matches = test_pattern.findall(content)
                        for test_name in matches:
                            for event in events:
                                if f"_{event}" in test_name:
                                    state_coverage[state_name].add(event)
                except Exception as e:
                    print(f"Błąd podczas czytania pliku {file_path}: {e}")

    return state_coverage

def main():
    parser = argparse.ArgumentParser(description="Weryfikuje pełne pokrycie każdego eventu w każdym stanie.")
    parser.add_argument("header", type=str, nargs="?", default="CharacterControllerEvents.h", help="Ścieżka do pliku z eventami")
    parser.add_argument("tests_dir", type=str, nargs="?", default="./tests", help="Katalog z plikami testowymi")

    args = parser.parse_args()

    try:
        events = extract_events_from_header(args.header)
        state_coverage = extract_state_coverage(args.tests_dir, events)
    except Exception as e:
        print(f"Błąd: {e}")
        return

    print("\n" + "="*80)
    print(" RAPORT ŚCISŁEGO POKRYCIA: KAŻDY EVENT W KAŻDYM STANIE ")
    print("="*80)
    print(f"Wszystkich eventów w bazie: {len(events)}")
    print(f"Wszystkich stanów (pliki): {len(state_coverage)}")
    print("="*80)

    fully_compliant = True

    for state in sorted(state_coverage.keys()):
        tested_events = state_coverage[state]
        missing_in_state = events - tested_events

        if not missing_in_state:
            print(f"\n🟢 [OK] {state} (100% pokrycia - wszystkie {len(events)} eventów)")
        else:
            fully_compliant = False
            print(f"\n🔴 [BRAKI] {state} ({len(tested_events)}/{len(events)} eventów)")
            print(f"   Brakuje następujących eventów w tym stanie:")
            for ev in sorted(missing_in_state):
                print(f"     ❌ {ev}")

    print("\n" + "="*80)
    if fully_compliant:
        print("🎉 SUKCES! Każdy stan testuje absolutnie każdy event z listy.")
    else:
        print("⚠️  ZNALEZIONO BRAKI. Niektóre stany nie testują wszystkich eventów.")
    print("="*80)

if __name__ == "__main__":
    main()