#!/usr/bin/env python3
HEADER =\
''' // Código autogenerado, no modificar
// Para regenerar, corrrer el archivo generate_unicode.py
#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#ifndef _SIMBOLOS_H
#define _SIMBOLOS_H'''

FOOTER=\
'''
#endif
'''

if __name__ == '__main__':
    header_symbols = open ('simbolos.h', 'w')
    header_symbols.write(HEADER) 
    for codepoint in range(0x2500, 0x257F):
        header_symbols.write("const cchar_t U{0:X} = {{0, L'{1:s}'}};\n".format(codepoint, chr(codepoint)))
        print("const cchar_t U{0:x} = {{0, L'{1:s}'}};".format(codepoint, chr(codepoint)))
    header_symbols.write(FOOTER) 
    header_symbols.close()
