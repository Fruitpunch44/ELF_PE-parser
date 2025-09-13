# ELF/PE Parser

A lightweight C utility for analyzing ELF (Executable and Linkable Format) and PE(Portable Executable) files.
currently only the ELF section is some what complete

## Features

- **ELF Header Analysis** - Display file type, architecture, entry point, and more
- **Section Table Parsing** - List all sections with names and indices  
- **Text Section Extraction** - View raw machine code in hexadecimal format
- **Cross-platform** - Supports both 32-bit and 64-bit ELF files

## Usage

```bash
./elf_parser [-PTSA] [FILE]

Options:
  -P    Print ELF headers
  -T    Parse and display .text section
  -S    Parse section table
  -A    All of the above
```

## Examples

```bash
# Analyze ELF headers
./elf_parser -P /bin/ls

# View machine code from .text section
./elf_parser -T ./my_program

# Complete analysis
./elf_parser -A /usr/bin/gcc
```

## Building

### Prerequisites
- GCC compiler
- Standard C library with ELF support

### Compilation
```bash
# Using Make
make

# Manual compilation
gcc main.c elf.c -o elf_parser
```

## Sample Output

```
/bin/ls is a valid elf file
ELF Magic: 7f 45 4c 46
Class: ELF64
Data: Two's complement, little-endian
OS/ABI: UNIX - System V
Machine: AMD x86-64
Type: Executable file
Entry point: 0x4060
```

## Technical Details

- Supports both ELF32 and ELF64 formats automatically
- Uses system-appropriate data structures via `ElfW()` macro
- Handles different endianness and architectures
- Memory-safe with proper cleanup and error handling


## TO DO:
- Additional ELF section parsers
- Better error messages
- Windows PE format support
