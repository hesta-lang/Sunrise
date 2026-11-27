#!/usr/bin/env python3
"""Convert the original Sunrise cover to Geode's logo.png without resizing or cropping."""
from pathlib import Path

from PIL import Image, ImageOps

root = Path(__file__).resolve().parents[1]
source = root / "logo.jpg"
destination = root / "logo.png"

with Image.open(source) as original:
    image = ImageOps.exif_transpose(original)
    if image.mode not in ("RGB", "RGBA"):
        image = image.convert("RGB")
    image.save(destination, format="PNG", optimize=True)
    print(f"Generated logo.png ({image.width}x{image.height}) from the original cover.")
