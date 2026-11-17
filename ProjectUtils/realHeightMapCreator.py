#!/usr/bin/env python3
"""
Generator heightmapy z NMT GUGiK WCS (GRID1 / GeoTIFF).

Pobiera teren kafelkami w lokalnym układzie AEQD, reprojektuje każdy kafelek
bezpośrednio do docelowej siatki i zapisuje:
  - OUTPUT.raw  : surowe uint16 little-endian, bez nagłówka
  - OUTPUT.png  : 16-bit grayscale
  - OUTPUT.json : metadane

Wymagania:
  pip install numpy rasterio pyproj owslib pillow

Uwaga o zerach:
Usługa WCS może zwracać 0 w miejscach bez danych, mimo że GeoTIFF nie ma
ustawionego NoData. Domyślnie zera źródłowe są zatem traktowane jako NoData.
Jeśli pobierasz teren, na którym wysokość 0 m jest rzeczywistą wartością,
uruchom skrypt z --keep-zero-elevation.
"""

import argparse
import json
import math
import sys
import time
from io import BytesIO

import numpy as np
import rasterio
from PIL import Image
from owslib.wcs import WebCoverageService
from pyproj import CRS
from rasterio.io import MemoryFile
from rasterio.transform import from_origin
from rasterio.warp import reproject, Resampling, transform_bounds


WCS_URL = (
    "https://mapy.geoportal.gov.pl/wss/service/"
    "PZGIK/NMT/GRID1/WCS/DigitalTerrainModelFormatTIFF"
)
SOURCE_CRS = "EPSG:2180"


def log(message):
    print(message, flush=True)


def create_local_crs(lat, lon):
    """Lokalny układ azymutalny AEQD; jednostką jest metr."""
    return CRS.from_proj4(
        f"+proj=aeqd +lat_0={lat} +lon_0={lon} "
        "+datum=WGS84 +units=m +no_defs"
    )


def connect_wcs(timeout):
    log("Łączenie z GUGiK WCS...")
    wcs = WebCoverageService(WCS_URL, version="1.0.0", timeout=timeout)
    coverages = list(wcs.contents.keys())

    if not coverages:
        raise RuntimeError("GUGiK WCS nie udostępnia żadnego pokrycia.")

    log("Dostępne pokrycia:")
    for coverage in coverages:
        log(f"  - {coverage}")

    return wcs, coverages


def get_coverage_format(wcs, coverage_id):
    coverage = wcs.contents[coverage_id]
    formats = getattr(coverage, "supportedFormats", []) or []
    log(f"Dostępne formaty: {formats}")

    for fmt in formats:
        if "tiff" in fmt.lower() or "tif" in fmt.lower():
            return fmt

    raise RuntimeError(
        "Usługa nie zgłosiła formatu GeoTIFF. "
        f"Zgłoszone formaty: {formats}"
    )


def make_tile_windows(width_px, height_px, tile_px):
    """Zwraca prostokątne okna (row0, row1, col0, col1) bez nakładania."""
    for row0 in range(0, height_px, tile_px):
        row1 = min(row0 + tile_px, height_px)
        for col0 in range(0, width_px, tile_px):
            col1 = min(col0 + tile_px, width_px)
            yield row0, row1, col0, col1


def fetch_tile(wcs, coverage_id, bbox, resolution_m, output_format, timeout_retries):
    min_x, min_y, max_x, max_y = bbox

    req_width = max(1, math.ceil((max_x - min_x) / resolution_m))
    req_height = max(1, math.ceil((max_y - min_y) / resolution_m))

    last_error = None
    for attempt in range(1, timeout_retries + 1):
        try:
            response = wcs.getCoverage(
                identifier=coverage_id,
                bbox=(min_x, min_y, max_x, max_y),
                crs=SOURCE_CRS,
                width=req_width,
                height=req_height,
                format=output_format,
            )
            data = response.read()
            if not data:
                raise RuntimeError("WCS zwrócił pustą odpowiedź.")
            if not data.startswith((b"II", b"MM")):
                preview = data[:500].decode("utf-8", errors="replace")
                raise RuntimeError(
                    "Odpowiedź WCS nie jest GeoTIFF-em. "
                    f"Początek odpowiedzi: {preview}"
                )
            return data
        except Exception as exc:
            last_error = exc
            if attempt < timeout_retries:
                wait_s = min(2 ** (attempt - 1), 8)
                log(
                    f"  Błąd pobierania (próba {attempt}/"
                    f"{timeout_retries}): {exc}. Ponawiam za {wait_s}s..."
                )
                time.sleep(wait_s)

    raise RuntimeError(f"Nie udało się pobrać kafelka: {last_error}")


def download_and_reproject(
    wcs,
    coverage_id,
    center_lat,
    center_lon,
    radius_km,
    resolution_m,
    tile_size_m,
    output_format,
    retries,
    keep_zero_elevation,
):
    """
    Pobiera kwadrat w małych kafelkach. Każdy kafelek jest przeliczany
    bezpośrednio do swojego okna w docelowej siatce AEQD.
    """
    radius_m = radius_km * 1000.0
    world_size_m = radius_m * 2.0

    width_px = max(1, round(world_size_m / resolution_m))
    height_px = width_px
    pixel_x = world_size_m / width_px
    pixel_z = world_size_m / height_px

    local_crs = create_local_crs(center_lat, center_lon)
    source_crs = CRS.from_epsg(2180)

    # Rozmiar kafelka wyrażony w pikselach docelowej mapy.
    tile_px = max(1, round(tile_size_m / ((pixel_x + pixel_z) / 2.0)))

    destination = np.full(
        (height_px, width_px), np.nan, dtype=np.float32
    )

    windows = list(make_tile_windows(width_px, height_px, tile_px))
    total_tiles = len(windows)

    log("\nParametry pobierania kafelkowego:")
    log(f"  Obszar: {world_size_m:.1f} x {world_size_m:.1f} m")
    log(f"  Docelowa mapa: {width_px} x {height_px}")
    log(f"  Rozmiar piksela: {pixel_x:.4f} x {pixel_z:.4f} m")
    log(f"  Kafelek: około {tile_size_m:.0f} x {tile_size_m:.0f} m")
    log(f"  Liczba zapytań WCS: {total_tiles}")
    log(f"  CRS źródłowy: {SOURCE_CRS}")
    log(f"  CRS docelowy: lokalny AEQD")

    for index, (row0, row1, col0, col1) in enumerate(windows, start=1):
        # Dokładny zasięg okna w lokalnym układzie AEQD.
        left = -radius_m + col0 * pixel_x
        right = -radius_m + col1 * pixel_x
        top = radius_m - row0 * pixel_z
        bottom = radius_m - row1 * pixel_z

        # Transformujemy granice kafelka do układu WCS.
        bbox = transform_bounds(
            local_crs,
            source_crs,
            left,
            bottom,
            right,
            top,
            densify_pts=21,
        )

        log(
            f"\n[{index}/{total_tiles}] "
            f"Pobieranie kafelka: kolumny {col0}:{col1}, "
            f"wiersze {row0}:{row1}"
        )

        tile_bytes = fetch_tile(
            wcs,
            coverage_id,
            bbox,
            resolution_m,
            output_format,
            retries,
        )

        with MemoryFile(tile_bytes) as memfile:
            with memfile.open() as src:
                if src.crs is None:
                    raise RuntimeError(
                        f"Kafelek {index}: GeoTIFF nie zawiera CRS."
                    )

                source = src.read(1).astype(np.float32)

                # Jeżeli TIFF definiuje NoData, respektujemy tę wartość.
                # W tym serwisie NoData bywa jednak niezdefiniowane, a obszary
                # bez danych są wypełniane zerami. Domyślnie maskujemy zera.
                if src.nodata is not None:
                    source[source == src.nodata] = np.nan

                if not keep_zero_elevation:
                    source[source == 0] = np.nan

                tile_destination = np.full(
                    (row1 - row0, col1 - col0),
                    np.nan,
                    dtype=np.float32,
                )

                tile_transform = from_origin(
                    left, top, pixel_x, pixel_z
                )

                reproject(
                    source=source,
                    destination=tile_destination,
                    src_transform=src.transform,
                    src_crs=src.crs,
                    src_nodata=np.nan,
                    dst_transform=tile_transform,
                    dst_crs=local_crs,
                    dst_nodata=np.nan,
                    resampling=Resampling.bilinear,
                    num_threads=2,
                    init_dest_nodata=True,
                )

        destination[row0:row1, col0:col1] = tile_destination

        tile_valid = np.isfinite(tile_destination)
        valid_count = int(np.count_nonzero(tile_valid))
        tile_count = tile_destination.size
        log(
            f"  Odebrano {len(tile_bytes) / 1024 / 1024:.2f} MB; "
            f"poprawne piksele kafelka: {valid_count:,}/{tile_count:,}"
        )

    valid = np.isfinite(destination)
    valid_count = int(np.count_nonzero(valid))
    total_count = destination.size
    missing_count = total_count - valid_count

    log("\nPodsumowanie pokrycia:")
    log(f"  Poprawne piksele: {valid_count:,}/{total_count:,}")
    log(f"  Brakujące piksele: {missing_count:,}")

    if missing_count:
        missing_pct = 100.0 * missing_count / total_count
        # raise RuntimeError(
        #     f"Po pobraniu kafelków nadal brakuje {missing_count:,} "
        #     f"pikseli ({missing_pct:.2f}% obszaru). "
        #     "Nie zapisuję niekompletnej heightmapy. "
        #     "Może to oznaczać brak danych NMT w części obszaru "
        #     "albo obszary NoData zwracane przez usługę."
        # )

    return destination, local_crs, width_px, height_px, pixel_x, pixel_z


def save_heightmap(
    destination,
    center_lat,
    center_lon,
    radius_km,
    resolution_m,
    output_prefix,
    coverage_id,
    pixel_x,
    pixel_z,
    keep_zero_elevation,
    tile_size_m,
):
    valid = np.isfinite(destination)
    if not np.any(valid):
        raise RuntimeError("Brak poprawnych danych wysokościowych.")

    min_elevation = float(np.min(destination[valid]))
    max_elevation = float(np.max(destination[valid]))
    elevation_range = max_elevation - min_elevation

    log(f"\nMin elevation: {min_elevation:.3f} m")
    log(f"Max elevation: {max_elevation:.3f} m")
    log("Kodowanie wysokości uint16...")

    if elevation_range > 0:
        normalized = (destination - min_elevation) / elevation_range
        heightmap = np.rint(normalized * 65535.0).astype("<u2")
    else:
        heightmap = np.zeros(destination.shape, dtype="<u2")

    raw_path = output_prefix + ".raw"
    heightmap.tofile(raw_path)
    log(f"Zapisano RAW: {raw_path}")

    png_path = output_prefix + ".png"
    Image.fromarray(heightmap.astype(np.uint16)).save(png_path, format="PNG")
    log(f"Zapisano PNG: {png_path}")

    height_px, width_px = destination.shape
    world_size_m = radius_km * 2000.0

    metadata = {
        "center_lat": center_lat,
        "center_lon": center_lon,
        "radius_km": radius_km,
        "width_m": world_size_m,
        "depth_m": world_size_m,
        "min_elevation_m": min_elevation,
        "max_elevation_m": max_elevation,
        "heightmap_width": width_px,
        "heightmap_height": height_px,
        "pixel_size_x_m": pixel_x,
        "pixel_size_z_m": pixel_z,
        "format": "uint16",
        "endianness": "little",
        "encoding": "normalized_uint16",
        "row_order": "north_to_south",
        "source": "GUGiK",
        "source_dataset": "NMT GRID1",
        "source_coverage": coverage_id,
        "source_crs": SOURCE_CRS,
        "coordinate_system": "local_AEQD",
        "height_unit": "meter",
        "world_unit": "meter",
        "resampling": "bilinear",
        "download_method": "WCS tiled requests",
        "tile_size_m": tile_size_m,
        "zero_treated_as_nodata": not keep_zero_elevation,
    }

    json_path = output_prefix + ".json"
    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(metadata, f, indent=4, ensure_ascii=False)
    log(f"Zapisano JSON: {json_path}")

    log("\n========== GOTOWE ==========")
    log(f"Heightmap: {width_px} x {height_px}")
    log(f"World: {world_size_m:.1f} x {world_size_m:.1f} m")
    log(f"Elevation: {min_elevation:.2f} - {max_elevation:.2f} m")


def main():
    parser = argparse.ArgumentParser(
        description="GUGiK NMT Heightmap Generator - pobieranie kafelkowe"
    )
    parser.add_argument("--lat", type=float, required=True)
    parser.add_argument("--lon", type=float, required=True)
    parser.add_argument("--radius", type=float, default=7.0)
    parser.add_argument("--resolution", type=float, default=5.0)
    parser.add_argument("--output", type=str, default="terrain")
    parser.add_argument(
        "--coverage",
        type=str,
        default=None,
        help="Opcjonalny identyfikator pokrycia WCS",
    )
    parser.add_argument(
        "--tile-size",
        type=float,
        default=2000.0,
        help="Bok kafelka w metrach; domyślnie 2000 m",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=180,
        help="Timeout pojedynczego połączenia WCS w sekundach",
    )
    parser.add_argument(
        "--retries",
        type=int,
        default=3,
        help="Liczba prób pobrania każdego kafelka",
    )
    parser.add_argument(
        "--keep-zero-elevation",
        action="store_true",
        help="Nie traktuj zer źródłowych jako NoData",
    )

    args = parser.parse_args()

    if not -90 <= args.lat <= 90:
        parser.error("Nieprawidłowa latitude.")
    if not -180 <= args.lon <= 180:
        parser.error("Nieprawidłowa longitude.")
    if args.radius <= 0 or args.resolution <= 0:
        parser.error("Radius i resolution muszą być dodatnie.")
    if args.tile_size <= 0 or args.tile_size > 2400:
        parser.error("--tile-size musi być większy od 0 i nie większy niż 2400 m.")
    if args.timeout <= 0 or args.retries <= 0:
        parser.error("--timeout i --retries muszą być dodatnie.")
    if args.resolution < 1:
        parser.error("Resolution nie może być mniejsze niż 1 m.")

    try:
        wcs, coverages = connect_wcs(args.timeout)

        coverage_id = args.coverage or coverages[0]
        if coverage_id not in coverages:
            raise RuntimeError(f"Nieznane coverage: {coverage_id}")

        log(f"\nWybrane pokrycie: {coverage_id}")
        output_format = get_coverage_format(wcs, coverage_id)

        (
            destination,
            _local_crs,
            _width_px,
            _height_px,
            pixel_x,
            pixel_z,
        ) = download_and_reproject(
            wcs=wcs,
            coverage_id=coverage_id,
            center_lat=args.lat,
            center_lon=args.lon,
            radius_km=args.radius,
            resolution_m=args.resolution,
            tile_size_m=args.tile_size,
            output_format=output_format,
            retries=args.retries,
            keep_zero_elevation=args.keep_zero_elevation,
        )

        save_heightmap(
            destination=destination,
            center_lat=args.lat,
            center_lon=args.lon,
            radius_km=args.radius,
            resolution_m=args.resolution,
            output_prefix=args.output,
            coverage_id=coverage_id,
            pixel_x=pixel_x,
            pixel_z=pixel_z,
            keep_zero_elevation=args.keep_zero_elevation,
            tile_size_m=args.tile_size,
        )

    except Exception as error:
        log(f"\nERROR: {error}")
        sys.exit(1)


if __name__ == "__main__":
    main()
