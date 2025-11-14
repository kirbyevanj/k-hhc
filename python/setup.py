#!/usr/bin/env python3
"""
Setup script for HHC Python bindings (abi3, pybind11).

Focuses only on extension build; all metadata is in pyproject.toml.
"""

from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext
import os

# Resolve include directory for the bundled headers (sdist vs. repo layout)
this_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(this_dir)

if os.path.exists(os.path.join(this_dir, "k-hhc")):
    include_dir = os.path.join(this_dir, "k-hhc")
else:
    include_dir = os.path.join(root_dir, "k-hhc")

ext_modules = [
    Pybind11Extension(
        "k_hhc",
        sources=["hhc_python.cpp"],
        include_dirs=[include_dir],
        language="c++",
        # Build against the stable ABI (PEP 384)
        py_limited_api=True,
        define_macros=[("Py_LIMITED_API", "0x03070000")],  # Python 3.7+ stable ABI
        cxx_std=20,
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
