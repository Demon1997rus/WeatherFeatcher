#!/usr/bin/env python3
"""Converts the GeoNames cities15000 dump into data/cities.json.

Usage:
    python tools/build_cities_json.py cities15000.txt data/cities.json [alternateNamesV2.txt]

Input:  GeoNames dumps (tab-separated, see download.geonames.org/export/dump/).
        alternateNamesV2.txt is optional but recommended: it provides proper
        Russian display names (isolanguage == 'ru', isPreferred wins).
Output: compact JSON array sorted by population (desc), entries:
        {"n": display_name, "a": ascii_name, "c": country,
         "la": lat, "lo": lon, "p": population}
"""

import json
import sys


def load_russian_names(path: str) -> dict:
    """geonameid -> Russian name; preferred names overwrite earlier ones."""
    names = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 5 or parts[2] != "ru":
                continue
            geoname_id = int(parts[1])
            is_preferred = parts[4] == "1"
            if is_preferred or geoname_id not in names:
                names[geoname_id] = parts[3]
    return names


def main(src: str, dst: str, alternates_path: str = None) -> None:
    russian_names = load_russian_names(alternates_path) if alternates_path else {}

    cities = []
    with open(src, encoding="utf-8") as f:
        for line in f:
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 15:
                continue
            population = int(parts[14] or 0)
            if population <= 0:
                continue
            display = russian_names.get(int(parts[0]), parts[1])
            cities.append({
                "n": display,
                "a": parts[2],
                "c": parts[8],
                "la": float(parts[4]),
                "lo": float(parts[5]),
                "p": population,
            })

    cities.sort(key=lambda c: c["p"], reverse=True)

    with open(dst, "w", encoding="utf-8") as f:
        json.dump(cities, f, ensure_ascii=False, separators=(",", ":"))

    print(f"{len(cities)} cities written to {dst}")


if __name__ == "__main__":
    if len(sys.argv) not in (3, 4):
        sys.exit(
            f"Usage: {sys.argv[0]} cities15000.txt data/cities.json [alternateNamesV2.txt]"
        )
    main(sys.argv[1], sys.argv[2], sys.argv[3] if len(sys.argv) == 4 else None)
