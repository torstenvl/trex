# `trex`

## Description and usage
`trex` is a tiny regex library with UTF-8 support.

Usage is simple: 

````C
if (regexmatch("^\\\\fcharset\\d+ ?$", rtfcommand)) { 
    /* set the character set for the current font definition */ 
}
````

## Features
- Public domain
- UTF-8 aware and locale aware
- Small source code size (< 100 SLOC)
- Small binary size (~3.5kb on macOS, ~4kb on Linux)
- No dynamic memory allocation
- Supports standard metacharacter classes `\d`, `\s`, and `\w`
- Easy to add additional metacharacter classes (e.g., supports `\x` for hexadecimal digits out of the box)

## Limitations
- No character sets, e.g., `[0-9]`
- No character groups or alternates, e.g., `(abc|def)`
- No quantifiers, e.g., `\d{1,5}`
- No support for index or match length (return value only indicates whether there's a match)
- Locale support relies on system library iswxxxxx functions, which are broken on macOS for East Asian languages and Arabic
