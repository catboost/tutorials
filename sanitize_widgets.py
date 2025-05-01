#!/usr/bin/env python3
"""
sanitize_widgets.py
-------------------
Make ipywidget outputs invisible to old nbformat validators **without**
clearing any other cell outputs.  Works in-place, no backups created.

Usage
-----
    python sanitize_widgets.py  <folder>
"""

from pathlib import Path
import sys, nbformat

WIDGET_MIME = "application/vnd.jupyter.widget-view+json"

def fix_one(nb_path: Path) -> bool:
    """Return True if the notebook was modified."""
    nb = nbformat.read(nb_path, as_version=4)
    changed = False

    # Drop global widgets metadata
    if nb.metadata.pop("widgets", None) is not None:
        changed = True

    # Strip only the offending MIME bundle from code-cell outputs
    for cell in nb.cells:
        if cell.cell_type != "code" or "outputs" not in cell:      # skip non-code / empty
            continue
        for out in list(cell.outputs):                             # iterate over *copy*
            if getattr(out, "data", None) and WIDGET_MIME in out.data:
                cell.outputs.remove(out)
                changed = True

    if changed:
        nbformat.write(nb, nb_path)
    return changed

def main(root: Path):
    patched, untouched = 0, 0
    for nb_path in root.rglob("*.ipynb"):
        if fix_one(nb_path):
            patched += 1
            print(f"✔ fixed {nb_path.relative_to(root)}")
        else:
            untouched += 1
    print(f"Done: {patched} patched, {untouched} already clean.")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("Usage: python sanitize_widgets.py <folder>")
    main(Path(sys.argv[1]).expanduser().resolve())
