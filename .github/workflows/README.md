# GitHub Actions Workflows Documentation

This directory contains GitHub Actions workflows for CI/CD operations on the HHC C++ project.

## Workflows Overview

### 1. CI Workflow (`ci.yml`)

**Purpose**: Main continuous integration pipeline for linting, building, testing, and fuzzing.

**Triggers**:
- Push to `master` or `develop` branches
- Pull requests to `master` or `develop` branches
- Manual workflow dispatch

**Jobs**:

1. **Lint** - Code quality checks with clang-tidy
   - Runs on: `ubuntu-latest`
   - Must pass for other jobs to run

2. **Build & Test C++** - Multi-platform C++ compilation and testing
   - Platforms: Linux (AMD64/ARM64), macOS (AMD64/ARM64), Windows (AMD64)
   - Uses native runners where available
   - Experimental ARM64 Linux support with `continue-on-error`
   
3. **Build & Test Python** - Python wheel building and testing
   - Python versions: 3.8, 3.9, 3.10, 3.11, 3.12
   - Platforms: Same as C++
   - Uses cibuildwheel for cross-platform wheels
   
4. **Fuzzing** - Security testing with LibFuzzer
   - Runs only on push or manual trigger
   - 5-minute fuzzing sessions for decode32/decode64
   - Uploads corpus for future runs

**Key Features**:
- Concurrent job cancellation for same branch
- Job dependencies ensure logical flow
- Experimental jobs don't fail the pipeline
- Comprehensive artifact upload

### 2. Release Workflow (`release.yml`)

**Purpose**: Build release artifacts, create GitHub releases, and publish to PyPI.

**Triggers**:
- Push of tags matching `v*` pattern
- Manual workflow dispatch with tag input

**Jobs**:

1. **Build Release Artifacts** - Create distribution packages
   - Builds Python wheels for all platforms
   - Creates C++ library archives (tar.gz/zip)
   - Generates source distribution

2. **Create GitHub Release** - Automated release creation
   - Generates changelog from git history
   - Creates release with all artifacts
   - Generates SHA256 checksums
   - Marks pre-releases based on tag pattern

3. **Publish to PyPI** - Python package distribution
   - Uses PyPI trusted publishing (no tokens needed)
   - Requires `pypi` environment approval
   - Skips existing versions

4. **Publish to Test PyPI** (optional) - For testing
   - Only runs on manual workflow dispatch
   - Useful for release testing

**Key Features**:
- Full platform matrix support
- Automated changelog generation
- Checksum generation for all artifacts
- Pre-release detection (alpha, beta, rc)

### 3. Coverage Workflow (`coverage.yml`)

**Purpose**: Generate code coverage reports for C++ and Python code.

**Triggers**:
- Push to `master` or `develop` branches
- Pull requests
- Weekly schedule (Monday 3 AM UTC)
- Manual workflow dispatch

**Jobs**:

1. **C++ Coverage** - LLVM-based coverage analysis
   - Uses clang's built-in coverage tools
   - Generates HTML, LCOV, and text reports
   - Excludes test/benchmark/example code
   - Comments on PRs with coverage summary

2. **Python Coverage** - pytest-cov based analysis
   - Tests on Python 3.8 and 3.11
   - Multiple report formats
   - Per-version coverage tracking

3. **Combined Report** - Aggregates all coverage data
   - Merges C++ and Python reports
   - Creates unified artifact
   - 30-day retention

**Key Features**:
- Codecov integration
- PR commenting with coverage stats
- Build caching for faster runs
- Multiple output formats

## Platform Support Matrix

| Platform | Architecture | Runner | Status |
|----------|-------------|--------|--------|
| Linux | AMD64 | `ubuntu-latest` | ✅ Stable |
| Linux | ARM64 | `ubuntu-latest-arm64` | 🧪 Experimental |
| macOS | AMD64 | `macos-13` | ✅ Stable |
| macOS | ARM64 | `macos-14` | ✅ Stable |
| Windows | AMD64 | `windows-latest` | ✅ Stable |

**Notes**:
- ARM64 Linux uses GitHub-hosted ARM runners (requires billing plan)
- Experimental jobs use `continue-on-error: true`
- All runners are native (no QEMU emulation)

## Best Practices Implemented

### 1. Security
- Minimal permissions (`contents: read` by default)
- Scoped permissions per job when needed
- PyPI trusted publishing (no token storage)

### 2. Performance
- Concurrency groups to cancel duplicate runs
- Build caching where appropriate
- Parallel test execution
- Native runners for all platforms

### 3. Reliability
- Timeout limits on all jobs
- Continue-on-error for experimental platforms
- Artifact upload even on failure
- Status check jobs for clear pass/fail

### 4. Developer Experience
- Clear job naming
- Comprehensive status reporting
- PR comments for coverage
- Manual workflow dispatch options

## Required Secrets

| Secret | Required | Used In | Purpose |
|--------|----------|---------|---------|
| `CODECOV_TOKEN` | Optional | coverage.yml | Upload coverage to Codecov |

## Required Environments

| Environment | Used In | Purpose |
|-------------|---------|---------|
| `pypi` | release.yml | PyPI publishing approval |
| `testpypi` | release.yml | Test PyPI publishing approval |

## Workflow Dependencies

```
CI Workflow:
  Lint → Build & Test (C++/Python) → Fuzz → Status Check
         ↓
         Upload Artifacts

Release Workflow:
  Build Artifacts → Create GitHub Release → Publish PyPI
                  ↘                       ↗
                    Test PyPI (optional)

Coverage Workflow:
  C++ Coverage ↘
                → Combined Report → Status Check
  Python Coverage ↗
```

## Artifact Retention

- Test results: 7 days
- Coverage reports: 30 days
- Fuzzing corpus: 7 days
- Release artifacts: 90 days (GitHub default)

## Manual Triggering

All workflows support `workflow_dispatch` for manual runs:

```bash
# Trigger CI manually
gh workflow run ci.yml

# Trigger release with specific tag
gh workflow run release.yml -f tag=v1.2.3

# Trigger coverage analysis
gh workflow run coverage.yml
```

## Monitoring and Debugging

### View Workflow Status
```bash
# List recent workflow runs
gh run list

# View specific run details
gh run view <run-id>

# Watch a run in progress
gh run watch
```

### Download Artifacts
```bash
# List artifacts for a run
gh run view <run-id> --json artifacts

# Download specific artifact
gh run download <run-id> -n <artifact-name>
```

### Common Issues

1. **ARM64 Runner Not Available**
   - Error: "No runner matching the specified labels"
   - Solution: ARM64 runners require GitHub Team/Enterprise plan
   - Workaround: Jobs marked experimental will be skipped

2. **Coverage Upload Fails**
   - Check if `CODECOV_TOKEN` is set (optional but recommended)
   - Verify Codecov GitHub App is installed

3. **PyPI Publishing Fails**
   - Ensure PyPI trusted publishing is configured
   - Check environment protection rules
   - Verify version doesn't already exist

## Customization

### Adding New Platforms

To add a new platform, update the matrix in the relevant workflow:

```yaml
matrix:
  include:
    - os: newos
      arch: newarch
      runs-on: runner-name
      experimental: true  # Set for new platforms
```

### Adjusting Python Versions

Edit the `CIBW_BUILD` environment variable:
```yaml
CIBW_BUILD: cp38-* cp39-* cp310-* cp311-* cp312-* cp313-*
```

### Changing Fuzzing Duration

Modify the timeout in the fuzzing job:
```yaml
timeout 600s ./build/fuzz/hhc_fuzz_decode32  # 10 minutes
```

## Contributing

When modifying workflows:
1. Test with `workflow_dispatch` first
2. Validate YAML syntax locally
3. Check for security best practices
4. Update this documentation

## Resources

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [PyPI Trusted Publishing](https://docs.pypi.org/trusted-publishers/)
- [Codecov Documentation](https://docs.codecov.com/)
- [cibuildwheel Documentation](https://cibuildwheel.readthedocs.io/)