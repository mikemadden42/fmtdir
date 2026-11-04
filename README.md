# fmtdir

List the files in a directory, grouped by extension. fmtdir only reads the
directory; it doesn't move, rename or change any files.

## Building

fmtdir needs CMake 3.10 or later and a C++17 compiler.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Run the tests with:

```sh
cd build
ctest -C Release
```

## Usage

```
fmtdir [options] <directory>

Options:
  -a, --all, --hidden   Include hidden files
  -h, --help            Show this help
```

Options may come before or after the directory. Use `--` to end the options,
for example to list a directory whose name starts with `-`:

```sh
fmtdir -- -my-dir
```

## Example

```
$ fmtdir .
(no extension):
Makefile

.jpg:
beach.jpg
IMG_0001.JPG

.txt:
notes.txt
README.TXT
```

## Details

- Extensions are grouped without regard to case, under a lowercase heading:
  `.JPG` and `.jpg` files are both listed under `.jpg:`. Names within a group
  are also sorted without regard to case.
- Files without an extension are listed under `(no extension)`. Dotfiles such
  as `.env` have no extension, so with `-a` they appear there too.
- Hidden files are skipped unless `-a` is given. On macOS and Linux, a file is
  hidden if its name starts with a dot; on Windows, if it has the hidden
  attribute.
- Only files directly in the directory are listed. Subdirectories are skipped,
  not searched.
- fmtdir exits with 0 on success and 1 on an error, such as a missing
  directory or an unknown option.

## Upgrading

Earlier versions used `-h` to include hidden files. `-h` now shows help; use
`-a`, `--all` or `--hidden` instead.

## License

MIT. See [LICENSE](LICENSE).
