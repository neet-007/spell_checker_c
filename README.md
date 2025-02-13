
# Spell Checker

## Overview

A C program that takes an input dictionary file and spell-checks input either from **stdin** or from a file if provided.

## Usage

### Build the program
Run `make` to compile the program:
```bash
make
```

### Run the spell checker
```bash
spell_checker <dictionary_file> [input_file]
```
- `<dictionary_file>`: The file containing valid words.
- `[input_file]` *(optional)*: A file to spell-check. If not provided, input is read from **stdin**.

## Notes
- The program uses a **trie** and searches for patterns in it.
- If the pattern is found and has a word termination, it is considered a match.

## Coming Soon
- **Spelling suggestions**
- **Auto-complete**

