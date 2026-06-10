#!/usr/bin/env python3
"""
Setup script for HHC Python bindings (abi3, stable CPython Limited API).

The extension is implemented with raw CPython APIs (PyMethodDef, Python.h),
not pybind11. Pybind11Extension/build_ext are used only as setuptools build
helpers (compiler flags, include paths). All metadata is in pyproject.toml.
"""

from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext  # build helper only
import os

# Resolve include directory for the bundled headers (sdist vs. repo layout)
this_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(this_dir)

if os.path.exists(os.path.join(this_dir, "k-hhc")):
    include_dir = os.path.join(this_dir, "k-hhc")
else:
    include_dir = os.path.join(root_dir, "k-hhc")

# Use C++17 standard
cxx_std_flag = 17

# Pybind11Extension wraps the raw CPython extension for setuptools; bindings are not pybind11.
ext_modules = [
    Pybind11Extension(
        "k_hhc",
        sources=["hhc_python.cpp"],
        include_dirs=[include_dir],
        language="c++",
        py_limited_api=True,
        define_macros=[("Py_LIMITED_API", "0x03070000")],  # CPython 3.7 stable ABI (PEP 384)
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
