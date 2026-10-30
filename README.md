# Image Steganography in C

A command-line tool that hides a secret text file inside a BMP image using
**LSB (Least Significant Bit) encoding**, and extracts it again later. The
stego image looks the same as the original because only the lowest bit of
each pixel byte is changed.

## How it works

Each byte of secret data is spread across the least significant bits of 8
consecutive image bytes. The BMP header (first 54 bytes) is copied unchanged,
then the following is encoded in order:

1. Magic string (`#*`), used to check that an image really contains hidden data
2. Size of the secret file's extension
3. Secret file extension (e.g. `.txt`)
4. Size of the secret file
5. Secret file data

The rest of the image is copied as-is. While decoding, the magic string is
checked first, so a normal image is rejected with a clear error.

## Build

Requires `gcc` and `make`.

```bash
make          # builds the ./stego executable
make clean    # removes object files and the executable
make help     # prints usage
```

## Usage

### Encode

```bash
./stego -e <source.bmp> <secret.txt> [output.bmp]
```

```bash
./stego -e beautiful.bmp secret.txt stego.bmp
```

If `output.bmp` is not given, the result is saved as `default.bmp`.

### Decode

```bash
./stego -d <stego.bmp> [output_file]
```

```bash
./stego -d stego.bmp output.txt
```

If the output file name is not given, it is saved as `output` plus the
original extension (e.g. `output.txt`).

## Project structure

| File | Purpose |
|------|---------|
| `test_encode.c` | `main()`: parses the operation (`-e` / `-d`) and drives encoding or decoding |
| `encode.c` / `encode.h` | Argument validation, capacity check, and all encoding functions |
| `decode.c` / `decode.h` | Argument validation and all decoding functions |
| `common.h` | Magic string definition |
| `types.h` | Shared types (`Status`, `OperationType`, `uint`) |
| `Makefile` | Build script |
| `beautiful.bmp` | Sample source image |
| `secret.txt` | Sample secret file |
| `stego.bmp` | Sample encoded image |
| `output.txt` | Sample decoded output |

## Limitations

- Only 24-bit **BMP** images are supported.
- The secret file must have a `.txt` name (the extension is checked) and the
  extension can be at most 4 characters.
- The image must be large enough to hold the secret data; otherwise encoding
  stops with an error.
- No password or encryption, so anyone who knows the tool can extract the data.

## Possible improvements

- Support any file type as the secret
- Encrypt the data before hiding it
- Support more image formats
