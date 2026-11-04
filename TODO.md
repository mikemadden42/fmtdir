# TODO

## Bugs

- [x] **Return a non-zero exit code on filesystem errors** (`fmtdir.cc`, `catch` block).
  `fmtdir /nonexistent` prints the error but exits 0. Add `return 1;` in the `catch`.
- [x] **Windows: a failed attribute lookup counts as "hidden"** (`isHidden`).
  `GetFileAttributesW` returns `INVALID_FILE_ATTRIBUTES` (`0xFFFFFFFF`) on failure, which
  has the hidden bit set, so unreadable files are silently skipped. Fix:
  ```cpp
  DWORD attrs = GetFileAttributesW(p.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_HIDDEN);
  ```
- [x] **Windows: some filenames can crash the program.**
  On MSVC, `path::string()` throws `std::system_error` for names the current code page
  can't represent. Only `fs::filesystem_error` is caught, so the program terminates.
  Catch `std::exception`, or keep `fs::path` values and print with `.u8string()`.
  (Not yet verified on Windows.)
- [x] **CI Test step probably fails.**
  The workflow runs `ctest`, but `CMakeLists.txt` has no `enable_testing()` or tests.
  Locally `ctest` exits 1 with "No test configuration file found!". Add
  `enable_testing()` and at least one test, or remove the step.

## Behavior and usability

- [x] Show files with no extension under a clear label such as `(no extension):`
  instead of a bare `:` header. With `--hidden`, dotfiles like `.bashrc` end up there too.
- [ ] Rethink `-h`, which usually means `--help`. Consider `-a`/`--all` (the `ls`
  convention), and accept options in any position (`fmtdir -h .` currently treats `-h`
  as the directory).
- [ ] Decide whether extensions should be grouped case-insensitively (`.TXT` vs `.txt`)
  and whether sorting should ignore case.
- [ ] Update the README: the tool lists files rather than moving them, and the README
  should include build and usage instructions.

## Build and CI

- [ ] Stop overwriting `CMAKE_CXX_FLAGS`; use
  `target_compile_options(fmtdir PRIVATE -Wall -Wextra -pedantic)`.
- [ ] Add `set(CMAKE_CXX_STANDARD_REQUIRED ON)`.
- [x] Add a CI matrix for `ubuntu`, `macos` and `windows` so the platform-specific
  code paths are built and tested.
- [x] Bump `actions/checkout@v3` to v4 or later (v3 runs on a deprecated Node version).
- [ ] Define `WIN32_LEAN_AND_MEAN` and `NOMINMAX` before including `<Windows.h>`.

## Small cleanups

- [ ] Use `entry.is_regular_file()` instead of `fs::is_regular_file(entry.path())`
  to reuse the cached status and avoid an extra `stat` per file.
- [ ] Merge the sort and print loops, and drop the trailing blank line after the last group.
