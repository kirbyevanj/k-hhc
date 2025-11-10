#!/usr/bin/env python3
"""
Setup script for HHC Python bindings.
"""

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup, find_packages
import os
import sys
import subprocess

# Get the directory containing this setup.py
this_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.dirname(this_dir)

# Check if k-hhc directory exists in current dir (sdist) or parent dir (dev)
if os.path.exists(os.path.join(this_dir, "k-hhc")):
    include_dir = os.path.join(this_dir, "k-hhc")
else:
    include_dir = os.path.join(root_dir, "k-hhc")

# Detect compiler type and configure appropriate flags
def detect_compiler_and_configure():
    """Detect the compiler and return appropriate configuration."""
    import platform
    
    # Determine if we're using MSVC/clang-cl or GCC/Clang
    is_msvc_style = platform.system() == 'Windows' and 'MSC' in sys.version
    
    # Check environment
    cxx = os.environ.get('CXX', '')
    is_clang_cl = 'clang-cl' in cxx
    if 'cl.exe' in cxx or 'cl' == cxx or is_clang_cl:
        is_msvc_style = True
    elif 'clang' in cxx or 'g++' in cxx or 'gcc' in cxx:
        is_msvc_style = False
    
    if is_msvc_style:
        # MSVC-style flags (used by both MSVC and clang-cl)
        # /std:c++latest for latest C++ features
        # /Zc:__cplusplus to correctly define __cplusplus macro
        return {
            'cxx_std': None,  # Don't use cxx_std with MSVC-style, handle it manually
            'extra_compile_args': ['/EHsc', '/bigobj', '/std:c++latest', '/Zc:__cplusplus'],
            'extra_link_args': []
        }
    
    # GCC/Clang flags - detect available C++ standard support
    compiler = os.environ.get('CXX', 'c++')

    # Try standards in order of preference: c++23, c++2b, c++20
    standards_to_try = [
        ('c++23', 23),
        ('c++2b', None),  # None means use extra_compile_args
        ('c++20', 20),
    ]

    for std_flag, cxx_std_value in standards_to_try:
        try:
            result = subprocess.run(
                [compiler, f'-std={std_flag}', '-x', 'c++', '-E', '-'],
                input=b'int main() { return 0; }',
                capture_output=True,
                timeout=5
            )
            if result.returncode == 0:
                if std_flag != 'c++23':
                    print(f"Note: Using -std={std_flag} as compiler doesn't support -std=c++23", file=sys.stderr)

                if cxx_std_value is not None:
                    # Use setuptools cxx_std parameter
                    return {
                        'cxx_std': cxx_std_value,
                        'extra_compile_args': ['-fvisibility=hidden', '-g0'],
                        'extra_link_args': []
                    }
                else:
                    # Use manual flag in extra_compile_args
                    return {
                        'cxx_std': None,
                        'extra_compile_args': [f'-std={std_flag}', '-fvisibility=hidden', '-g0'],
                        'extra_link_args': []
                    }
        except (subprocess.TimeoutExpired, FileNotFoundError, OSError):
            pass

    # Ultimate fallback
    print("Warning: No supported C++ standard found, using default", file=sys.stderr)
    return {
        'cxx_std': None,
        'extra_compile_args': ['-fvisibility=hidden', '-g0'],
        'extra_link_args': []
    }

# Get compiler configuration
config = detect_compiler_and_configure()

# Build extension kwargs
ext_kwargs = {
    "name": "k_hhc",
    "sources": ["hhc_python.cpp"],
    "include_dirs": [include_dir],
    "language": "c++",
    "extra_compile_args": config['extra_compile_args'],
    "extra_link_args": config['extra_link_args'],
}

if config['cxx_std'] is not None:
    ext_kwargs["cxx_std"] = config['cxx_std']

# Define the extension module
ext_modules = [
    Pybind11Extension(**ext_kwargs),
]

# Read README for long description
readme_path = os.path.join(root_dir, "README.md")
if os.path.exists(readme_path):
    with open(readme_path, "r", encoding="utf-8") as f:
        long_description = f.read()
else:
    long_description = "Python bindings for k-hhc (Hexahexacontadecimal) encoding/decoding"

setup(
    name="k-hhc",
    version="1.0.2",
    author="Evan Kirby",
    author_email="kirbyevanj@gmail.com",
    url="https://github.com/kirbyevanj/k-hhc",
    description="Python bindings for k-hhc (Hexahexacontadecimal) encoding/decoding",
    long_description=long_description,
    long_description_content_type="text/markdown",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    python_requires=">=3.6",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: Python :: 3.13",
        "Programming Language :: C++",
        "Operating System :: OS Independent",
        "Topic :: Software Development :: Libraries",
    ],
    zip_safe=False,
    license="Apache-2.0",
)
