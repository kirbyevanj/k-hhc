#!/usr/bin/env python3
"""
Setup script for HHC Python bindings (abi3, pybind11).

Focuses only on extension build; all metadata is in pyproject.toml.
"""

from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext
import os
import subprocess
import re

# Resolve include directory for the bundled headers (sdist vs. repo layout)
this_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(this_dir)

if os.path.exists(os.path.join(this_dir, "k-hhc")):
    include_dir = os.path.join(this_dir, "k-hhc")
else:
    include_dir = os.path.join(root_dir, "k-hhc")

# Detect GCC version and determine appropriate C++20 standard flag
# GCC 9/10 support C++20 via -std=c++2a, GCC 11+ supports -std=c++20
def get_cxx_std_flag():
    """Detect GCC version and return appropriate C++ standard flag."""
    # Check for CXXFLAGS override (set by CI for musllinux)
    cxxflags = os.environ.get("CXXFLAGS", "")
    if "-std=c++2a" in cxxflags:
        # CI has set -std=c++2a, use cxx_std='2a' to match
        return "2a"
    
    # Try to detect GCC version
    cxx = os.environ.get("CXX", "g++")
    try:
        result = subprocess.run([cxx, "--version"], capture_output=True, text=True, timeout=5)
        version_output = result.stdout
        # Extract GCC version number (e.g., "g++ (GCC) 9.3.0" -> 9)
        match = re.search(r"g\+\+\s+.*?(\d+)\.\d+", version_output)
        if match:
            gcc_major = int(match.group(1))
            if gcc_major < 11:
                # GCC 9/10: use c++2a instead of c++20
                return "2a"
    except (subprocess.TimeoutExpired, FileNotFoundError, ValueError, IndexError):
        # If detection fails, default to 20 (will work with GCC 11+)
        pass
    
    # Default to c++20 for GCC 11+ or unknown compiler
    return 20

cxx_std_flag = get_cxx_std_flag()

ext_modules = [
    Pybind11Extension(
        "k_hhc",
        sources=["hhc_python.cpp"],
        include_dirs=[include_dir],
        language="c++",
        # Build against the stable ABI (PEP 384)
        py_limited_api=True,
        define_macros=[("Py_LIMITED_API", "0x03070000")],  # Python 3.7+ stable ABI
        cxx_std=cxx_std_flag,
    )
]

setup(
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    # Disable package discovery - this is a C extension only
    packages=[],
    py_modules=[],
    # Ensure the wheel is tagged as abi3 even when using `python -m build`
    options={"bdist_wheel": {"py_limited_api": "cp37"}},
)
