# GitHub Actions Workflows Quick Reference

## Workflow Triggers Summary

| Workflow | Push (master/develop) | Pull Request | Tags | Schedule | Manual |
|----------|---------------------|--------------|------|----------|--------|
| CI | ✅ | ✅ | ❌ | ❌ | ✅ |
| Python Package | ✅ | ✅ | ✅ | ❌ | ✅ |
| Fuzzing | ✅ | ✅ | ❌ | ✅ Daily | ✅ |
| Release | ❌ | ❌ | ✅ v* | ❌ | ✅ |
| Documentation | ✅ master only | ❌ | ❌ | ❌ | ✅ |

## Architecture Support

| Workflow | AMD64 | ARM64 |
|----------|-------|-------|
| CI | ✅ | ✅ |
| Python Package | ✅ | ✅ |
| Fuzzing | ✅ | ✅ |
| Release | ✅ | ✅ |
| Documentation | ✅ | ❌ |

## Platform Support

### C++ Library (CI Workflow)
- **Linux AMD64**: `ubuntu-latest` (native)
- **Linux ARM64**: `ubuntu-24.04-arm` (native, GitHub-hosted)
- **macOS AMD64**: `macos-13` (Intel, native)
- **macOS ARM64**: `macos-14` (Apple Silicon, native)
- **Windows**: Not currently tested (header-only should work)

**Note:** All builds use native runners. No QEMU emulation.

### Python Package
- **Linux**: Ubuntu (manylinux) - AMD64, ARM64
- **macOS**: Latest - AMD64, ARM64
- **Windows**: Latest - AMD64
- **Python Versions**: 3.8, 3.9, 3.10, 3.11, 3.12

## Manual Workflow Dispatch

All workflows support manual triggering via GitHub UI:
1. Go to Actions tab
2. Select workflow
3. Click "Run workflow"
4. Select branch
5. Click "Run workflow" button

## Publishing Checklist

### To Test PyPI (develop branch)
```bash
git checkout develop
git push origin develop
# Automatically publishes to test.pypi.org
```

### To PyPI (version tag)
```bash
# Update version in:
# - CMakeLists.txt
# - python/setup.py
# - python/pyproject.toml

git add .
git commit -m "chore: bump version to 1.0.0"
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin master
git push origin v1.0.0
# Automatically publishes to pypi.org
```

## Secrets Required

### Optional Secrets
- `CODECOV_TOKEN`: For code coverage uploads
- `TEST_PYPI_API_TOKEN`: For Test PyPI publishing (if not using trusted publishing)

### PyPI Trusted Publishing (Recommended)
Configure in PyPI project settings instead of using API tokens:
- Workflow: `python.yml`
- Environment: `pypi`
- Repository: `yourusername/hhc-cpp`

## Common Commands

### Trigger CI locally
```bash
# Build and test
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure

# With coverage
cmake -B build -DHHC_ENABLE_COVERAGE=ON
cmake --build build --target coverage
```

### Build Python wheels locally
```bash
pip install cibuildwheel
cd python
cibuildwheel --platform linux
```

### Run fuzzing locally
```bash
cmake -B build -DHHC_ENABLE_FUZZING=ON
cmake --build build --target hhc_fuzz_decode32
./build/fuzz/hhc_fuzz_decode32 -max_total_time=60
```

## Monitoring

- **CI Status**: Check Actions tab for build status
- **Coverage**: View at codecov.io/gh/yourusername/hhc-cpp
- **Documentation**: Deployed to yourusername.github.io/hhc-cpp
- **PyPI**: Check at pypi.org/project/hhc

## Troubleshooting

### ARM64 builds
- All ARM64 builds use native runners (no QEMU)
- Linux ARM64 requires `ubuntu-24.04-arm` runner availability
- For private repos, may require GitHub Team or Enterprise plan
- Check Actions billing for ARM64 minute usage

### Python wheel build fails
- Check C++23 compiler support
- Verify pybind11 version
- Review cibuildwheel logs

### Coverage upload fails
- Verify CODECOV_TOKEN is set
- Check Codecov integration is enabled
- Review coverage file generation

### PyPI publish fails
- Verify trusted publishing is configured
- Check version number is incremented
- Ensure tag matches version

## Performance Tips

- **Caching**: Dependencies are cached automatically
- **Parallel builds**: All builds use maximum parallelism
- **Matrix strategy**: Jobs run in parallel
- **Artifacts**: Uploaded for debugging, expire after 90 days

