#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
OSM Road Mesh Generator
----------------------
Tworzy kafelkowe modele OBJ z geometrii dróg OpenStreetMap, dopasowanej
do heightmapy wygenerowanej przez skrypt GUGiK NMT.

Wymagania:
    pip install numpy pyproj shapely requests

Wejście:
    terrain.raw
    terrain.json

Wyjście:
    OUTPUT/
      roads.mtl
      tile_x000_z000.obj
      tile_x001_z000.obj
      ...
      manifest.json

Każdy OBJ jest jednym modelem/kafelkiem. W jego obrębie obiekty OBJ (o)
są grupowane według klasy materiału. Silnik może zaimportować je jako
oddzielne meshe. Zachowanie podziału obiektów zależy od ustawień Assimp.

Układ współrzędnych:
    X = wschód
    Y = wysokość
    Z = północ
    środek mapy = (0, 0)
    zakres XY mapy = [-radius, +radius]
    heightmap row_order = north_to_south
"""

import argparse
import json
import math
import sys
import time
from pathlib import Path

import numpy as np
import requests
from pyproj import CRS, Transformer
from shapely.geometry import LineString, box
from shapely.ops import triangulate


OVERPASS_URL = "https://overpass-api.de/api/interpreter"

# Szerokości są wartościami domyślnymi w metrach. OSM "width" nadpisuje
# je, jeśli tag zawiera wartość liczbową, np. "5", "5 m".
ROAD_WIDTHS = {
    "motorway": 12.0, "motorway_link": 7.0,
    "trunk": 10.0, "trunk_link": 7.0,
    "primary": 9.0, "primary_link": 6.5,
    "secondary": 8.0, "secondary_link": 6.0,
    "tertiary": 7.0, "tertiary_link": 5.5,
    "unclassified": 6.0,
    "residential": 5.5,
    "living_street": 5.0,
    "service": 4.0,
    "road": 5.0,
    "track": 3.2,
    "path": 1.8,
    "footway": 1.8,
    "pedestrian": 2.5,
    "cycleway": 2.0,
    "bridleway": 2.0,
    "steps": 1.5,
}

MATERIAL_FOR_HIGHWAY = {
    "motorway": "asphalt", "motorway_link": "asphalt",
    "trunk": "asphalt", "trunk_link": "asphalt",
    "primary": "asphalt", "primary_link": "asphalt",
    "secondary": "asphalt", "secondary_link": "asphalt",
    "tertiary": "asphalt", "tertiary_link": "asphalt",
    "unclassified": "asphalt", "residential": "asphalt",
    "living_street": "asphalt", "service": "asphalt", "road": "asphalt",
    "track": "dirt",
    "path": "path", "footway": "path", "pedestrian": "path",
    "cycleway": "path", "bridleway": "path", "steps": "path",
}


def log(msg):
    print(msg, flush=True)


def parse_width(tags, highway):
    value = tags.get("width")
    if value:
        # OSM zwykle zapisuje szerokość jako "4.5" lub "4.5 m".
        cleaned = value.lower().replace(",", ".").replace("meters", "").replace("meter", "").replace("m", "").strip()
        try:
            width = float(cleaned)
            if 0.5 <= width <= 40:
                return width
        except ValueError:
            pass
    return ROAD_WIDTHS.get(highway, 5.0)


def load_heightmap(raw_path, metadata_path):
    with open(metadata_path, "r", encoding="utf-8") as f:
        meta = json.load(f)

    required = [
        "center_lat", "center_lon", "radius_km",
        "heightmap_width", "heightmap_height",
        "pixel_size_x_m", "pixel_size_z_m",
        "min_elevation_m", "max_elevation_m",
        "row_order", "encoding", "endianness",
    ]
    missing = [k for k in required if k not in meta]
    if missing:
        raise ValueError("Brak pól w JSON heightmapy: " + ", ".join(missing))

    if meta["row_order"] != "north_to_south":
        raise ValueError("Obsługiwany jest wyłącznie row_order=north_to_south.")
    if meta["encoding"] != "normalized_uint16":
        raise ValueError("Obsługiwane jest wyłącznie encoding=normalized_uint16.")
    if meta["endianness"] != "little":
        raise ValueError("Obsługiwany jest wyłącznie little-endian.")

    width = int(meta["heightmap_width"])
    height = int(meta["heightmap_height"])
    expected = width * height * 2
    actual = Path(raw_path).stat().st_size
    if actual != expected:
        raise ValueError(
            f"Nieprawidłowy rozmiar RAW: {actual} B, oczekiwano {expected} B "
            f"({width}x{height} uint16)."
        )

    data = np.fromfile(raw_path, dtype="<u2").reshape((height, width))
    return data, meta


def sample_elevation(data, meta, x, z):
    """Bilinear sampling; x rośnie na wschód, z na północ."""
    w = data.shape[1]
    h = data.shape[0]
    radius = float(meta["radius_km"]) * 1000.0
    px = float(meta["pixel_size_x_m"])
    pz = float(meta["pixel_size_z_m"])

    # Współrzędne indeksowe względem środków pikseli.
    col = (x + radius) / px - 0.5
    row = (radius - z) / pz - 0.5
    col = min(max(col, 0.0), w - 1.0)
    row = min(max(row, 0.0), h - 1.0)

    c0 = int(math.floor(col))
    r0 = int(math.floor(row))
    c1 = min(c0 + 1, w - 1)
    r1 = min(r0 + 1, h - 1)
    tx = col - c0
    ty = row - r0

    a = float(data[r0, c0]) * (1.0 - tx) + float(data[r0, c1]) * tx
    b = float(data[r1, c0]) * (1.0 - tx) + float(data[r1, c1]) * tx
    encoded = a * (1.0 - ty) + b * ty

    low = float(meta["min_elevation_m"])
    high = float(meta["max_elevation_m"])
    return low + (encoded / 65535.0) * (high - low)


def fetch_osm_roads(meta, timeout, include_paths):
    """Pobiera drogi z Overpass dla geograficznego bbox otaczającego mapę."""
    lat = float(meta["center_lat"])
    lon = float(meta["center_lon"])
    radius = float(meta["radius_km"]) * 1000.0

    # Bbox z zapasem. Dla niewielkich polskich obszarów przybliżenie
    # stopniowe jest wystarczające do pobrania, geometria jest potem
    # dokładnie transformowana i przycinana w AEQD.
    dlat = radius / 110540.0
    dlon = radius / max(1.0, 111320.0 * math.cos(math.radians(lat)))
    south, west = lat - dlat, lon - dlon
    north, east = lat + dlat, lon + dlon

    highway_values = list(ROAD_WIDTHS.keys())
    if not include_paths:
        highway_values = [k for k in highway_values if MATERIAL_FOR_HIGHWAY[k] != "path"]
    regex = "|".join(highway_values)
    query = f"""
    [out:json][timeout:180];
    way["highway"~"^({regex})$"]({south},{west},{north},{east});
    out tags geom;
    """

    log(f"Pobieranie OSM z Overpass: S={south:.6f}, W={west:.6f}, N={north:.6f}, E={east:.6f}")
    response = requests.post(
        OVERPASS_URL,
        data={"data": query},
        timeout=timeout,
        headers={"User-Agent": "OSMRoadMeshGenerator/1.0"},
    )
    response.raise_for_status()
    payload = response.json()
    elements = payload.get("elements", [])
    log(f"Overpass zwrócił {len(elements)} dróg.")
    return elements


def densify_coords(coords, spacing):
    """Wstawia punkty pośrednie, aby ograniczyć długość segmentu."""
    if len(coords) < 2:
        return coords
    result = [coords[0]]
    for a, b in zip(coords[:-1], coords[1:]):
        dx, dz = b[0] - a[0], b[1] - a[1]
        length = math.hypot(dx, dz)
        count = max(1, int(math.ceil(length / spacing)))
        for i in range(1, count + 1):
            t = i / count
            result.append((a[0] + dx * t, a[1] + dz * t))
    return result


def polygon_parts(geom):
    if geom.is_empty:
        return []
    if geom.geom_type == "Polygon":
        return [geom]
    if geom.geom_type == "MultiPolygon":
        return list(geom.geoms)
    if geom.geom_type == "GeometryCollection":
        out = []
        for g in geom.geoms:
            out.extend(polygon_parts(g))
        return out
    return []


def triangulate_polygon(poly):
    """Triangulacja z odrzuceniem trójkątów leżących poza polygonem."""
    result = []
    for tri in triangulate(poly):
        # Reprezentatywny punkt zabezpiecza przed trójkątami poza obrysem.
        if poly.covers(tri):
            coords = list(tri.exterior.coords)[:3]
            if len(coords) == 3:
                result.append(coords)
    return result


def write_mtl(path):
    materials = {
        "asphalt": (0.16, 0.16, 0.16),
        "dirt": (0.30, 0.22, 0.13),
        "path": (0.42, 0.39, 0.31),
    }
    with open(path, "w", encoding="utf-8") as f:
        for name, (r, g, b) in materials.items():
            f.write(f"newmtl {name}\n")
            f.write(f"Kd {r:.4f} {g:.4f} {b:.4f}\n")
            f.write("Ka 0.0000 0.0000 0.0000\n")
            f.write("Ks 0.0500 0.0500 0.0500\n")
            f.write("Ns 8.0\n")
            f.write("illum 2\n\n")


def write_obj(path, mtl_name, groups, uv_scale, elevation_offset):
    """
    groups: material -> list of triangles, każdy trójkąt to 3 punkty (x,z).
    Wierzchołki są celowo współdzielone tylko w obrębie obiektu/materialu
    przez prosty cache współrzędnych.
    """
    with open(path, "w", encoding="utf-8") as f:
        f.write("# Generated by OSM Road Mesh Generator\n")
        f.write(f"mtllib {mtl_name}\n")
        global_v = 1
        global_vt = 1

        for material, triangles in groups.items():
            if not triangles:
                continue
            f.write(f"\no {material}\n")
            f.write(f"usemtl {material}\n")
            vertex_map = {}
            vertices = []
            texcoords = []
            faces = []

            for tri in triangles:
                # Odwrócenie windingu: w układzie X/Y/Z (Y-up) daje normalną +Y.
                tri = [tri[0], tri[2], tri[1]]
                face_indices = []
                for x, z in tri:
                    key = (round(x, 4), round(z, 4))
                    idx = vertex_map.get(key)
                    if idx is None:
                        y = sample_elevation(HEIGHT_DATA, HEIGHT_META, x, z) + elevation_offset
                        idx = len(vertices) + 1
                        vertex_map[key] = idx
                        vertices.append((x, y, z))
                        texcoords.append((x / uv_scale, z / uv_scale))
                    face_indices.append(idx)
                faces.append(face_indices)

            for x, y, z in vertices:
                f.write(f"v {x:.4f} {y:.4f} {z:.4f}\n")
            for u, v in texcoords:
                f.write(f"vt {u:.6f} {v:.6f}\n")
            for face in faces:
                a, b, c = [global_v + i - 1 for i in face]
                f.write(f"f {a}/{global_vt + face[0] - 1} "
                        f"{b}/{global_vt + face[1] - 1} "
                        f"{c}/{global_vt + face[2] - 1}\n")
            global_v += len(vertices)
            global_vt += len(texcoords)


def main():
    parser = argparse.ArgumentParser(description="Generator meshów dróg OSM dopasowanych do heightmapy.")
    parser.add_argument("--heightmap", required=True, help="Plik RAW z generatora heightmapy.")
    parser.add_argument("--metadata", required=True, help="Plik JSON z metadanymi heightmapy.")
    parser.add_argument("--output-dir", default="roads_out", help="Katalog wyjściowy.")
    parser.add_argument("--tile-size", type=float, default=512.0, help="Bok kafelka modelu w metrach.")
    parser.add_argument("--spacing", type=float, default=2.5, help="Maksymalna odległość punktów centerline w metrach.")
    parser.add_argument("--uv-scale", type=float, default=8.0, help="Powtarzanie UV co tyle metrów.")
    parser.add_argument("--elevation-offset", type=float, default=0.08, help="Uniesienie nad terenem w metrach.")
    parser.add_argument("--overpass-timeout", type=int, default=240)
    parser.add_argument("--include-paths", action="store_true", help="Uwzględnij footway/path/cycleway itp.")
    args = parser.parse_args()

    if args.tile_size <= 0 or args.spacing <= 0 or args.uv_scale <= 0:
        parser.error("tile-size, spacing i uv-scale muszą być dodatnie.")

    global HEIGHT_DATA, HEIGHT_META
    HEIGHT_DATA, HEIGHT_META = load_heightmap(args.heightmap, args.metadata)

    meta = HEIGHT_META
    radius = float(meta["radius_km"]) * 1000.0
    world = box(-radius, -radius, radius, radius)

    local_crs = CRS.from_proj4(
        f"+proj=aeqd +lat_0={meta['center_lat']} +lon_0={meta['center_lon']} "
        "+datum=WGS84 +units=m +no_defs"
    )
    transformer = Transformer.from_crs(CRS.from_epsg(4326), local_crs, always_xy=True)

    elements = fetch_osm_roads(meta, args.overpass_timeout, args.include_paths)

    # Bufory geometrii dróg są przycinane do zasięgu mapy, a potem do kafelków.
    roads = []
    skipped = 0
    for element in elements:
        if element.get("type") != "way":
            continue
        tags = element.get("tags", {})
        highway = tags.get("highway")
        if highway not in ROAD_WIDTHS:
            continue
        geom = element.get("geometry", [])
        if len(geom) < 2:
            skipped += 1
            continue

        geographic = [(p["lon"], p["lat"]) for p in geom]
        projected = [transformer.transform(lon, lat) for lon, lat in geographic]
        projected = densify_coords(projected, args.spacing)
        line = LineString(projected)
        if not line.is_valid or line.length < 0.1:
            skipped += 1
            continue

        width = parse_width(tags, highway)
        # cap_style=2 (flat), join_style=2 (mitre); ograniczenie mitre
        # zapobiega bardzo długim ostrym narożnikom.
        surface = line.buffer(width / 2.0, cap_style=2, join_style=2, mitre_limit=2.0)
        surface = surface.intersection(world)
        if surface.is_empty:
            continue
        material = MATERIAL_FOR_HIGHWAY[highway]
        roads.append({
            "id": str(element.get("id", "unknown")),
            "highway": highway,
            "material": material,
            "surface": surface,
        })

    log(f"Przygotowano {len(roads)} powierzchni dróg; pominięto {skipped} niepoprawnych geometrii.")

    tile_count = int(math.ceil((2.0 * radius) / args.tile_size))
    out_dir = Path(args.output_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    mtl_path = out_dir / "roads.mtl"
    write_mtl(mtl_path)

    manifest = {
        "format": "OBJ",
        "coordinate_system": "local_AEQD",
        "axis": {"x": "east", "y": "up", "z": "north"},
        "center_lat": meta["center_lat"],
        "center_lon": meta["center_lon"],
        "tile_size_m": args.tile_size,
        "heightmap_raw": str(Path(args.heightmap).name),
        "heightmap_metadata": str(Path(args.metadata).name),
        "materials": ["asphalt", "dirt", "path"],
        "tiles": [],
    }

    total_triangles = 0
    for tz in range(tile_count):
        for tx in range(tile_count):
            # Kafelki indeksowane od południowego-zachodu; współrzędne
            # modelu nadal są względem środka mapy.
            min_x = -radius + tx * args.tile_size
            min_z = -radius + tz * args.tile_size
            max_x = min(min_x + args.tile_size, radius)
            max_z = min(min_z + args.tile_size, radius)
            tile_bounds = box(min_x, min_z, max_x, max_z)
            groups = {"asphalt": [], "dirt": [], "path": []}

            for road in roads:
                if not road["surface"].intersects(tile_bounds):
                    continue
                clipped = road["surface"].intersection(tile_bounds)
                for poly in polygon_parts(clipped):
                    if poly.area < 0.05:
                        continue
                    for tri in triangulate_polygon(poly):
                        groups[road["material"]].append(tri)

            tri_count = sum(len(v) for v in groups.values())
            if tri_count == 0:
                continue

            filename = f"tile_x{tx:03d}_z{tz:03d}.obj"
            obj_path = out_dir / filename
            write_obj(
                obj_path, "roads.mtl", groups,
                args.uv_scale, args.elevation_offset
            )
            total_triangles += tri_count
            manifest["tiles"].append({
                "x": tx, "z": tz,
                "file": filename,
                "bounds_xz_m": [min_x, min_z, max_x, max_z],
                "triangle_count": tri_count,
                "mesh_objects": [k for k, v in groups.items() if v],
            })
            log(f"Zapisano {filename}: {tri_count:,} trójkątów")

    manifest["total_tiles"] = len(manifest["tiles"])
    manifest["total_triangles"] = total_triangles
    with open(out_dir / "manifest.json", "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2, ensure_ascii=False)

    log("\n========== GOTOWE ==========")
    log(f"Kafelki/model: {len(manifest['tiles'])}")
    log(f"Łącznie trójkątów: {total_triangles:,}")
    log(f"Katalog: {out_dir.resolve()}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        log("Przerwano przez użytkownika.")
        sys.exit(130)
    except Exception as exc:
        log(f"BŁĄD: {exc}")
        raise
