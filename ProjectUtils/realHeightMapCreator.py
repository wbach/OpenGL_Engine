import os
import numpy as np
from PIL import Image
import rasterio
import elevation
import json
import math

def generate_city_heightmap(bbox, output_filename="heightmap.png", img_size=(1024, 1024)):
    """
    Pobiera dane DEM dla zadanego bounding boxa i tworzy 16-bitową heightmapę PNG.
    bbox: (min_lon, min_lat, max_lon, max_lat) - współrzędne geograficzne
    """
    min_lon, min_lat, max_lon, max_lat = bbox
    dem_tif = "temp_dem.tif"

    print("1. Pobieranie danych wysokościowych (DEM) dla zadanego obszaru...")
    # Pobieramy dane SRTM za pomocą biblioteki elevation
    try:
        elevation.clip(bounds=(min_lon, min_lat, max_lon, max_lat), output=dem_tif)
    except Exception as e:
        print(f"Błąd podczas pobierania danych: {e}")
        return

    print("2. Przetwarzanie rastra i generowanie heightmapy...")
    with rasterio.open(dem_tif) as src:
        # Odczytujemy dane wysokościowe do macierzy numpy
        elevation_data = src.read(1)

        # Obsługa wartości 'nodata' (np. morze lub brak danych) - zamieniamy na 0
        nodata = src.nodatavals[0]
        if nodata is not None:
            elevation_data = np.where(elevation_data == nodata, 0, elevation_data)

        # Zastąpienie ewentualnych ujemnych wysokości lub NaN zerami
        elevation_data = np.nan_to_num(elevation_data, nan=0.0)

    # Normalizacja wysokości do zakresu 16-bitowego (0 - 65535)
    min_val = np.min(elevation_data)
    max_val = np.max(elevation_data)

    print(f"Wysokości w terenie: od {min_val:.1f}m do {max_val:.1f}m n.p.m.")

    if max_val > min_val:
        # Skalowanie liniowe do pełnego zakresu 16-bit (dla zachowania precyzji w grze)
        normalized = (elevation_data - min_val) / (max_val - min_val)
        img_16bit = (normalized * 65535).astype(np.uint16)
    else:
        img_16bit = np.zeros_like(elevation_data, dtype=np.uint16)

    # Zmiana rozmiaru obrazu do docelowej rozdzielczości heightmapy (np. 1024x1024)
    img = Image.fromarray(img_16bit)
    img_resized = img.resize(img_size, Image.Resampling.BICUBIC)

    # Zapis do pliku PNG (16-bit skala szarości)
    img_resized.save(output_filename)
    print(f"Sukces! Heightmapa zapisana jako: {output_filename}")

    # Sprzątanie pliku tymczasowego
    if os.path.exists(dem_tif):
        os.remove(dem_tif)

    lat_center = (min_lat + max_lat) / 2.0
    width_meters = (max_lon - min_lon) * (111320.0 * math.cos(math.radians(lat_center)))
    depth_meters = (max_lat - min_lat) * 111280.0

    metadata = {
        "width_m": round(width_meters, 2),
        "depth_m": round(depth_meters, 2),
        "min_elevation_m": float(round(min_val, 2)),
        "max_elevation_m": float(round(max_val, 2)),
        "elevation_range_m": float(round(max_val - min_val, 2))
    }

    with open("gorzow_meta.json", "w") as f:
        json.dump(metadata, f, indent=4)

# --- PRZYKŁAD UŻYCIA ---
# Przykładowy obszar (np. fragment Kotliny Jeleniogórskiej / Karkonoszy lub dowolne miasto)
# Format: (min_longitude, min_latitude, max_longitude, max_latitude)
# Wrocław okolice: (16.9, 51.0, 17.1, 51.2)
#wroclaw_bbox = (16.9, 51.0, 17.1, 51.2)
#gorzow_bbox = (15.13, 52.68, 15.32, 52.78)
gorzow_bbox = (15.0949, 52.7024, 15.3067, 52.8146)
#52.8146, 15.0949 52.7024, 15.3067
# https://www.openstreetmap.org/search?lat=52.7024&lon=15.3067&zoom=12#map=13/52.72491/15.27872
# if __name__ == "__main__":
#     generate_city_heightmap(wroclaw_bbox, output_filename="wroclaw_heightmap.png", img_size=(2048, 2048))
if __name__ == "__main__":
      generate_city_heightmap(gorzow_bbox, output_filename="gorzow_heightmap.png", img_size=(2048, 2048))