import sys
import os
import numpy as np
from PIL import Image

def load_and_fix_layer(path, target_size=None):
    img = Image.open(path).convert('RGB')
    if target_size and img.size != target_size:
        img = img.resize(target_size, Image.Resampling.LANCZOS)
    
    data = np.array(img).astype(np.float32) / 255.0
    
    # 1. Agresywne wykrywanie tła (czysta zieleń tła)
    # Wszystko co jest blisko (0, 1, 0) traktujemy jako tło
    mask = (data[:, :, 1] > 0.8) & (data[:, :, 0] < 0.2) & (data[:, :, 2] < 0.2)

    # 2. Skalowanie wektorów (Twoje wejście jest zbyt silne)
    # Używamy mniejszego mnożnika, żeby x^2 + y^2 nie wybuchło
    x = (data[:, :, 0] * 2.0 - 1.0) * 0.6
    y = (data[:, :, 1] * 2.0 - 1.0) * 0.6
    
    # 3. Czyszczenie tła ZANIM policzymy Z
    x[mask] = 0.0
    y[mask] = 0.0

    # 4. Bezpieczne obliczanie Z
    # Jeśli x^2 + y^2 > 1, clipujemy do 1, żeby sqrt nie dał błędu
    mag_sq = np.clip(x**2 + y**2, 0.0, 1.0)
    z = np.sqrt(1.0 - mag_sq)
    
    # W miejscach tła wymuszamy idealny błękit [0, 0, 1]
    z[mask] = 1.0
    
    return np.stack([x, y, z], axis=2), img.size, mask

def main():
    if len(sys.argv) < 2:
        print("Użycie: python script.py <base_name>")
        return

    base = sys.argv[1]
    layers = []
    first_size = None
    final_mask = None

    # Szukamy _S2 (baza) i _S1 (detal)
    for s in ["_S2", "_S3", "_S1"]:
        for ext in [".tga", ".png", ".jpg"]:
            path = f"{base}{s}{ext}"
            if os.path.exists(path):
                print(f"Wczytuję {path}...")
                layer_n, size, mask = load_and_fix_layer(path, first_size)
                if first_size is None:
                    first_size = size
                    final_mask = mask
                layers.append(layer_n)
                break

    if not layers: return

    # Po prostu uśredniamy warstwy (bezpieczniejsze przy tak silnych danych niż RNM)
    combined = np.mean(layers, axis=0)
    
    # Normalizacja końcowa wektorów
    norm = np.maximum(np.linalg.norm(combined, axis=2, keepdims=True), 1e-6)
    final_n = combined / norm

    # Konwersja do błękitnej mapy Tangent Space
    out_rgb = (final_n + 1.0) * 0.5
    out_rgb = (out_rgb * 255).astype(np.uint8)

    # Dodajemy alfę dla przezroczystości tła
    alpha = (~final_mask * 255).astype(np.uint8)
    final_rgba = np.dstack((out_rgb, alpha))

    output_path = f"{base}_FINAL_OK.png"
    Image.fromarray(final_rgba).save(output_path)
    print(f"Zapisano poprawną mapę: {output_path}")

if __name__ == "__main__":
    main()