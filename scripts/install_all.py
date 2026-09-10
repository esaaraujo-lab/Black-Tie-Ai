#!/usr/bin/env python3
"""Compatibility wrapper. The supported installer is ../install.py."""
from pathlib import Path
import runpy
runpy.run_path(str(Path(__file__).resolve().parents[1] / "install.py"), run_name="__main__")
