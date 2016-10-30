#!/usr/bin/env python3
HEADER =\
''' // Código autogenerado, no modificar
// Para regenerar, corrrer el archivo generate_unicode.py
#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#ifndef _SIMBOLOS_H
#define _SIMBOLOS_H
'''

FOOTER=\
'''
#endif
'''

if __name__ == '__main__':
    header_symbols = open ('simbolos.h', 'w')
    header_symbols.write(HEADER) 
    for codepoint in range(0x2500, 0x259F):
        header_symbols.write("static const cchar_t U{0:X} = {{0, {{L'{1:s}', L'\\0'}}, 0}};\n".format(codepoint, chr(codepoint)))

    header_symbols.write("static const cchar_t U{0:X} = {{0, {{L'{1:s}', L'\\0'}}, 0}};\n".format(0x1F422, chr(0x1F422)))
    header_symbols.write(FOOTER) 
    header_symbols.close()
