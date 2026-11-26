#!/usr/bin/env python3
"""Render the Sunrise vector mark as Geode's required logo.png asset."""
from pathlib import Path
import cairosvg

root = Path(__file__).resolve().parents[1]
cairosvg.svg2png(
    url=str(root / "logo.svg"),
    write_to=str(root / "logo.png"),
    output_width=1024,
    output_height=1024,
)
print("Generated logo.png (1024x1024)")
