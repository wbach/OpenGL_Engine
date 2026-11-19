#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Generator blend mapy RGBA16 (uint16 little-endian, 0..65535) zgodny z obecnym
terrain shaderem (bez zmian w shaderze), realHeightMapCreator.py oraz road_generator.py.

Warstwy:
  - background (backTextureAmount = 1 - (R+G+B+A)): trawa + skała w shaderze
  - R: lasy i zadrzewienia
  - G: pola uprawne
  - B: woda (zbiorniki, rzeki, strumienie, mokradła)
  - A: tereny zurbanizowane / utwardzone / place / piasek
"""

import argparse
import json
import math
import sys
from pathlib import Path

import numpy as np
import requests
from PIL import Image, ImageDraw, ImageFilter
from pyproj import CRS, Transformer


OVERPASS_URL = "https://overpass-api.de/api/interpreter"


def log(msg: str) -> None:
    print(msg, flush=True)


def load_heightmap_meta(metadata_path: str):
    with open(metadata_path, "r", encoding="utf-8") as f:
        meta = json.load(f)

    required = [
        "center_lat", "center_lon", "radius_km",
        "heightmap_width", "heightmap_height",
        "pixel_size_x_m", "pixel_size_z_m",
        "row_order",
    ]
    missing = [k for k in required if k not in meta]
    if missing:
        raise ValueError("Brak wymaganych pól w JSON heightmapy: " + ", ".join(missing))

    if meta["row_order"] != "north_to_south":
        raise ValueError("Obsługiwany jest wyłącznie row_order=north_to_south.")

    return meta


def fetch_osm_landcover(meta: dict, timeout: int):
    lat = float(meta["center_lat"])
    lon = float(meta["center_lon"])
    radius = float(meta["radius_km"]) * 1000.0

    dlat = radius / 110540.0
    dlon = radius / max(1.0, 111320.0 * math.cos(math.radians(lat)))
    south, west = lat - dlat, lon - dlon
    north, east = lat + dlat, lon + dlon

    query = f"""
    [out:json][timeout:{timeout}];
    (
      // B: woda i mokradła
      way["natural"="water"]({south},{west},{north},{east});
      relation["natural"="water"]({south},{west},{north},{east});
      way["waterway"~"^(river|stream|canal|drain|ditch)$"]({south},{west},{north},{east});
      way["natural"="wetland"]({south},{west},{north},{east});
      relation["natural"="wetland"]({south},{west},{north},{east});

      // R: lasy i zadrzewienia
      way["landuse"~"^(forest|orchard)$"]({south},{west},{north},{east});
      relation["landuse"~"^(forest|orchard)$"]({south},{west},{north},{east});
      way["natural"~"^(wood|scrub|heath)$"]({south},{west},{north},{east});
      relation["natural"~"^(wood|scrub|heath)$"]({south},{west},{north},{east});

      // G: pola uprawne
      way["landuse"="farmland"]({south},{west},{north},{east});
      relation["landuse"="farmland"]({south},{west},{north},{east});

      // A: tereny zurbanizowane / utwardzone / place / piasek
      way["landuse"~"^(residential|commercial|retail|industrial|construction|railway)$"]({south},{west},{north},{east});
      relation["landuse"~"^(residential|commercial|retail|industrial|construction|railway)$"]({south},{west},{north},{east});
      way["amenity"="parking"]({south},{west},{north},{east});
      way["aeroway"~"^(runway|apron|taxiway)$"]({south},{west},{north},{east});
      way["natural"~"^(beach|sand)$"]({south},{west},{north},{east});
    );
    out body geom;
    """

    log("Pobieranie warstw pokrycia terenu z Overpass API...")
    resp = requests.post(
        OVERPASS_URL,
        data={"data": query},
        timeout=timeout,
        headers={"User-Agent": "TerrainBlendMapGenerator/3.0-uint16"},
    )
    resp.raise_for_status()
    elements = resp.json().get("elements", [])
    log(f"Odebrano {len(elements)} obiektów OSM.")
    return elements


def world_to_pixel(x_flipped: float, z_north: float, radius_m: float, px: float, pz: float):
    """Konwersja współrzędnych (x z odbitą osią X, z rosnące na północ) -> (col, row)."""
    col = (x_flipped + radius_m) / px - 0.5
    row = (radius_m - z_north) / pz - 0.5
    return col, row


def coords_to_aeqd_flipped(geom: list, transformer: Transformer):
    pts = []
    for p in geom:
        x, z = transformer.transform(p["lon"], p["lat"])
        pts.append((-x, z))  # odbicie osi X jak w heightmapie (np.fliplr) i drogach (-x)
    return pts


def stitch_rings(segments: list):
    """Łączy fragmenty obrysów (np. w relacjach multipolygon) w zamknięte pierścienie."""
    remaining = [list(seg) for seg in segments if len(seg) >= 2]
    rings = []

    while remaining:
        current = remaining.pop(0)
        closed = math.hypot(current[0][0] - current[-1][0], current[0][1] - current[-1][1]) < 1e-3

        while not closed:
            matched_idx = None
            reverse_needed = False
            prepend_needed = False
            tail = current[-1]
            head = current[0]

            for idx, seg in enumerate(remaining):
                if math.hypot(tail[0] - seg[0][0], tail[1] - seg[0][1]) < 1e-3:
                    matched_idx = idx
                    break
                if math.hypot(tail[0] - seg[-1][0], tail[1] - seg[-1][1]) < 1e-3:
                    matched_idx = idx
                    reverse_needed = True
                    break
                if math.hypot(head[0] - seg[-1][0], head[1] - seg[-1][1]) < 1e-3:
                    matched_idx = idx
                    prepend_needed = True
                    break
                if math.hypot(head[0] - seg[0][0], head[1] - seg[0][1]) < 1e-3:
                    matched_idx = idx
                    reverse_needed = True
                    prepend_needed = True
                    break

            if matched_idx is None:
                break

            seg = remaining.pop(matched_idx)
            if reverse_needed:
                seg = list(reversed(seg))
            if prepend_needed:
                current = seg[:-1] + current
            else:
                current = current[:-1] + seg

            closed = math.hypot(current[0][0] - current[-1][0], current[0][1] - current[-1][1]) < 1e-3

        if len(current) >= 3:
            rings.append(current)

    return rings


def extract_geometries(el: dict, transformer: Transformer):
    """Zwraca (outer_rings, inner_rings, lines)."""
    outer_rings = []
    inner_rings = []
    lines = []
    etype = el.get("type")

    if etype == "way":
        geom = el.get("geometry", [])
        if len(geom) < 2:
            return outer_rings, inner_rings, lines
        pts = coords_to_aeqd_flipped(geom, transformer)
        if len(pts) >= 3 and math.hypot(pts[0][0] - pts[-1][0], pts[0][1] - pts[-1][1]) < 1e-3:
            outer_rings.append(pts)
        else:
            lines.append(pts)

    elif etype == "relation":
        outer_segs = []
        inner_segs = []
        for member in el.get("members", []):
            if member.get("type") != "way":
                continue
            geom = member.get("geometry", [])
            if len(geom) < 2:
                continue
            pts = coords_to_aeqd_flipped(geom, transformer)
            role = member.get("role", "")
            if role == "inner":
                inner_segs.append(pts)
            else:
                outer_segs.append(pts)

        outer_rings.extend(stitch_rings(outer_segs))
        inner_rings.extend(stitch_rings(inner_segs))

    return outer_rings, inner_rings, lines


def gaussian_blur_float32(arr: np.ndarray, sigma: float) -> np.ndarray:
    """Rozmywa tablicę float32 [0..1] bez użycia przestarzałego trybu 'F' w Pillow."""
    if sigma <= 0.0:
        return np.clip(arr, 0.0, 1.0).astype(np.float32)

    try:
        from scipy.ndimage import gaussian_filter
        return np.clip(gaussian_filter(arr, sigma=sigma, mode="nearest"), 0.0, 1.0).astype(np.float32)
    except ImportError:
        radius = max(1, int(math.ceil(sigma * 3.0)))
        x = np.arange(-radius, radius + 1, dtype=np.float32)
        kernel = np.exp(-0.5 * (x / sigma) ** 2)
        kernel /= np.sum(kernel)

        padded_x = np.pad(arr, ((0, 0), (radius, radius)), mode="edge")
        blurred_x = np.apply_along_axis(lambda m: np.convolve(m, kernel, mode="valid"), 1, padded_x)

        padded_xy = np.pad(blurred_x, ((radius, radius), (0, 0)), mode="edge")
        blurred_xy = np.apply_along_axis(lambda m: np.convolve(m, kernel, mode="valid"), 0, padded_xy)
        return np.clip(blurred_xy, 0.0, 1.0).astype(np.float32)


def rasterize_channel(
    elements: list,
    matcher_fn,
    transformer: Transformer,
    radius_m: float,
    px: float,
    pz: float,
    width: int,
    height: int,
    line_width_m: float = 6.0,
    blur_radius_px: float = 2.0,
) -> np.ndarray:
    mask_img = Image.new("L", (width, height), 0)
    mask_draw = ImageDraw.Draw(mask_img)

    holes_img = Image.new("L", (width, height), 0)
    holes_draw = ImageDraw.Draw(holes_img)

    line_px_width = max(1, int(round(line_width_m / max(0.1, (px + pz) * 0.5))))

    for el in elements:
        tags = el.get("tags", {})
        if not matcher_fn(tags):
            continue

        outer_rings, inner_rings, lines = extract_geometries(el, transformer)

        for ring in outer_rings:
            poly_px = [world_to_pixel(x, z, radius_m, px, pz) for x, z in ring]
            if len(poly_px) >= 3:
                mask_draw.polygon(poly_px, fill=255)

        for hole in inner_rings:
            hole_px = [world_to_pixel(x, z, radius_m, px, pz) for x, z in hole]
            if len(hole_px) >= 3:
                holes_draw.polygon(hole_px, fill=255)

        for line in lines:
            line_px = [world_to_pixel(x, z, radius_m, px, pz) for x, z in line]
            if len(line_px) >= 2:
                mask_draw.line(line_px, fill=255, width=line_px_width)

    mask_arr = np.asarray(mask_img, dtype=np.float32)
    holes_arr = np.asarray(holes_img, dtype=np.float32)
    combined_norm = np.clip((mask_arr - holes_arr) / 255.0, 0.0, 1.0)

    return gaussian_blur_float32(combined_norm, sigma=blur_radius_px)

def save_rgba16_png(png_path: Path, rgba_u16: np.ndarray) -> str:
    """Zapisuje 16-bitowe RGBA PNG (jeśli dostępny cv2 lub png), w przeciwnym razie 8-bit podgląd."""
    try:
        import cv2
        # OpenCV używa kolejności BGRA
        bgra_u16 = rgba_u16[:, :, [2, 1, 0, 3]]
        if cv2.imwrite(str(png_path), bgra_u16):
            return "uint16_rgba_png"
    except ImportError:
        pass

    try:
        import png  # pypng
        h, w, _ = rgba_u16.shape
        rows = rgba_u16.reshape(h, w * 4)
        with open(png_path, "wb") as f:
            writer = png.Writer(width=w, height=h, alpha=True, greyscale=False, bitdepth=16)
            writer.write(f, rows)
        return "uint16_rgba_png"
    except ImportError:
        pass

    preview_u8 = np.rint(rgba_u16.astype(np.float32) / 65535.0 * 255.0).astype(np.uint8)
    Image.fromarray(preview_u8, mode="RGBA").save(png_path)
    return "uint8_rgba_png_preview"


def main():
    parser = argparse.ArgumentParser(
        description="Generator blend mapy RGBA16 (uint16) kompatybilnej z obecnym shaderem i heightmapą AEQD."
    )
    parser.add_argument("--metadata", required=True, help="Ścieżka do pliku .json z metadanymi heightmapy.")
    parser.add_argument("--heightmap", required=False, help="Opcjonalna ścieżka do pliku .raw heightmapy (do walidacji wymiarów).")
    parser.add_argument("--output", default="blendmap", help="Prefiks plików wyjściowych.")
    parser.add_argument("--overpass-timeout", type=int, default=180)
    args = parser.parse_args()

    meta = load_heightmap_meta(args.metadata)
    width = int(meta["heightmap_width"])
    height = int(meta["heightmap_height"])
    radius_m = float(meta["radius_km"]) * 1000.0
    px = float(meta["pixel_size_x_m"])
    pz = float(meta["pixel_size_z_m"])

    if args.heightmap:
        expected_bytes = width * height * 2
        actual_bytes = Path(args.heightmap).stat().st_size
        if actual_bytes != expected_bytes:
            raise ValueError(
                f"Nieprawidłowy rozmiar heightmapy {args.heightmap}: {actual_bytes} B, oczekiwano {expected_bytes} B."
            )

    local_crs = CRS.from_proj4(
        f"+proj=aeqd +lat_0={meta['center_lat']} +lon_0={meta['center_lon']} "
        "+datum=WGS84 +units=m +no_defs"
    )
    transformer = Transformer.from_crs(CRS.from_epsg(4326), local_crs, always_xy=True)

    elements = fetch_osm_landcover(meta, args.overpass_timeout)

    # R = lasy i zadrzewienia
    forest_mask = rasterize_channel(
        elements,
        lambda t: t.get("landuse") in ("forest", "orchard")
        or t.get("natural") in ("wood", "scrub", "heath"),
        transformer, radius_m, px, pz, width, height, blur_radius_px=3.0,
    )

    # G = pola uprawne
    farmland_mask = rasterize_channel(
        elements,
        lambda t: t.get("landuse") == "farmland",
        transformer, radius_m, px, pz, width, height, blur_radius_px=2.5,
    )

    # B = woda i tereny podmokłe
    water_mask = rasterize_channel(
        elements,
        lambda t: t.get("natural") in ("water", "wetland")
        or t.get("waterway") in ("river", "stream", "canal", "drain", "ditch"),
        transformer, radius_m, px, pz, width, height, line_width_m=7.0, blur_radius_px=1.5,
    )

    # A = tereny zurbanizowane / utwardzone / place / piasek
    urban_mask = rasterize_channel(
        elements,
        lambda t: t.get("landuse") in ("residential", "commercial", "retail", "industrial", "construction", "railway")
        or t.get("amenity") == "parking"
        or t.get("aeroway") in ("runway", "apron", "taxiway")
        or t.get("natural") in ("beach", "sand"),
        transformer, radius_m, px, pz, width, height, blur_radius_px=2.5,
    )

    # Priorytety: woda (B) > las (R) > tereny zurbanizowane/utwardzone (A) > pola uprawne (G)
    b = np.clip(water_mask, 0.0, 1.0)
    r = np.clip(forest_mask * (1.0 - b), 0.0, 1.0)
    a = np.clip(urban_mask * (1.0 - b) * (1.0 - r * 0.7), 0.0, 1.0)
    g = np.clip(farmland_mask * (1.0 - b) * (1.0 - r) * (1.0 - a * 0.8), 0.0, 1.0)

    # Normalizacja w float32, aby suma R+G+B+A <= 1.0 (backTextureAmount >= 0.0)
    total = r + g + b + a
    scale = np.where(total > 1.0, 1.0 / np.maximum(total, 1e-6), 1.0)
    r *= scale
    g *= scale
    b *= scale
    a *= scale

    # rgba_u16 = np.stack([
    #     np.rint(np.clip(r, 0.0, 1.0) * 65535.0).astype("<u2"),
    #     np.rint(np.clip(g, 0.0, 1.0) * 65535.0).astype("<u2"),
    #     np.rint(np.clip(b, 0.0, 1.0) * 65535.0).astype("<u2"),
    #     np.rint(np.clip(a, 0.0, 1.0) * 65535.0).astype("<u2"),
    # ], axis=-1)

    zeros_u16 = np.zeros_like(b, dtype="<u2")
    b_u16 = np.rint(np.clip(b, 0.0, 1.0) * 65535.0).astype("<u2")

    rgba_u16 = np.stack([
        zeros_u16,  # R
        zeros_u16,  # G
        zeros_u16,      # B (tylko woda)
        b_u16,  # A
    ], axis=-1)

    raw_path = Path(args.output + ".raw")
    png_path = Path(args.output + ".png")
    json_path = Path(args.output + ".json")

    rgba_u16.tofile(raw_path)
    png_encoding = save_rgba16_png(png_path, rgba_u16)

    meta_out = {
        "width": width,
        "height": height,
        "channels_count": 4,
        "channel_order": "RGBA",
        "format": "uint16",
        "endianness": "little",
        "encoding": "normalized_uint16",
        "png_encoding": png_encoding,
        "channels": {
            "R": "forest_and_woodland",
            "G": "farmland",
            "B": "water_and_wetland",
            "A": "urban_paved_and_sand",
            "remainder_1_minus_rgba_sum": "background_via_calculateBackgroundColor",
        },
        "row_order": "north_to_south",
        "coordinate_system": "local_AEQD_flipped_x",
        "source_heightmap_metadata": str(Path(args.metadata).name),
    }
    json_path.write_text(json.dumps(meta_out, indent=2, ensure_ascii=False), encoding="utf-8")

    log(f"Zapisano RAW (uint16 RGBA): {raw_path} ({raw_path.stat().st_size:,} B)")
    log(f"Zapisano PNG ({png_encoding}): {png_path}")
    log(f"Zapisano JSON: {json_path}")


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        log(f"BŁĄD: {exc}")
        sys.exit(1)