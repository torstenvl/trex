//                                    trex
//                      tiny regular expression library
//
// Written in 2022 by Joshua Lee Ockert <https://github.com/torstenvl/>
// The author disclaims all copyright in this work. Permission to use, copy, 
// modify, and/or distribute this work for any purpose is hereby granted.
// THIS WORK IS PROVIDED "AS IS" WITH NO WARRANTY OF ANY KIND. THE IMPLIED
// WARRANTIES OF MERCHANTABILITY, FITNESS, NON-INFRINGEMENT, AND TITLE ARE
// EXPRESSLY DISCLAIMED. NO AUTHOR SHALL BE LIABLE UNDER ANY THEORY OF LAW
// FOR ANY DAMAGES OF ANY KIND RESULTING FROM THE USE OF THIS WORK.
#include <locale.h>
#include <wctype.h>
#include <ctype.h>
#include "trex.h" 

static int match_xplus(const unsigned char *rex, const unsigned char *txt, const unsigned char *rexafter, int min);
static int match_maybe(const unsigned char *rex, const unsigned char *txt, const unsigned char *rexafter);

static int cdpt(const unsigned char *s) {
    if (s[0]<128) return s[0];
    // For byte patterns matching this...                                     ... get the correct bits from them and put them in the right places
    if (s[0]>>5== 6 && s[1]>>6==2)                             return                                         ((s[0] & 0x1F)<<6 | (s[1] & 0x3F));
    if (s[0]>>4==14 && s[1]>>6==2 && s[2]>>6==2)               return                     ((s[0] & 0x0F)<<12 | (s[1] & 0x3F)<<6 | (s[2] & 0x3F));
    if (s[0]>>3==30 && s[1]>>6==2 && s[2]>>6==2 && s[3]>>6==2) return ((s[0] & 0x07)<<18 | (s[1] & 0x3F)<<12 | (s[2] & 0x3F)<<6 | (s[3] & 0x3F));
    return s[0];
}

// Workaround for poor iswalpha() support in some operating systems
#define isualnum(x) ((*setlocale(LC_ALL, NULL) == 'C') ? (isalnum(x)) : (!iswcntrl(x) && !iswpunct(x) && !iswspace(x)))

// Extremely hot codepath. Handling well-formed UTF-8 by macro --> ~4% speedup
#define next(R) ((R[1]>>6!=2) ? (R+1) : (R[2]>>6!=2) ? (R+2) : \
                 (R[3]>>6!=2) ? (R+3) : (R[4]>>6!=2) ? (R+4) : next_func(R+4))

static const unsigned char *next_func(const unsigned char *rex) {
    do { rex++; } while (*rex && rex[0]>>6 == 2); return rex;
}

static const unsigned char *prev(const unsigned char *rex) {
    do { rex--; } while (*rex && rex[0]>>6 == 2); return rex;
}

static int matchclass(const unsigned char *rex, const unsigned char *txt) {
    switch (rex[0]) {  // Add custom classes here in lieu of ranges
        case 'd':                return  iswdigit((wint_t)cdpt(txt));
        case 'D':                return !iswdigit((wint_t)cdpt(txt));
        case 'x':                return  iswxdigit((wint_t)cdpt(txt));
        case 'X':                return !iswxdigit((wint_t)cdpt(txt));
        case 'w':                return  (txt[0]=='_'||isualnum((wint_t)cdpt(txt)));
        case 'W':                return !(txt[0]=='_'||isualnum((wint_t)cdpt(txt)));
        case 's':                return  iswspace((wint_t)cdpt(txt));
        case 'S':                return !iswspace((wint_t)cdpt(txt));
        default:                 return  (cdpt(rex) == cdpt(txt));
    }
}

static int matchone(const unsigned char *rex, const unsigned char *txt) {
    if (txt[0] == '\0')          return 0;
    if (rex[0] == '.')           return 1;
    if (rex[0] == '\\')          return matchclass(next(rex), txt);
    if (cdpt(rex) == cdpt(txt))  return 1;
    return 0;
}

static int matchhere(const unsigned char *rex, const unsigned char *txt) {
    const unsigned char *nextrex = next(rex);
    if (rex[0] == '\0') return 1;
    if (rex[0] == '\\') nextrex = next(nextrex);
    if (nextrex[0] == '?')       return match_maybe(rex, txt, next(nextrex));
    if (nextrex[0] == '*')       return match_xplus(rex, txt, next(nextrex), 0);
    if (nextrex[0] == '+')       return match_xplus(rex, txt, next(nextrex), 1);
    if (rex[0]=='$' && nextrex[0]=='\0')
                                 return (txt[0]=='\0')  ?  1  :  0;
    if (matchone(rex, txt))      return matchhere(nextrex, next(txt));
    return 0;
}

static int match_maybe(const unsigned char *rex, const unsigned char *txt, const unsigned char *rexafter) {
    if (matchhere(rexafter, txt))                                return 1;
    if (matchone(rex, txt) && matchhere(rexafter, next(txt)))    return 1;
    return 0;
}

static int match_xplus(const unsigned char *rex, const unsigned char *txt, const unsigned char *rexafter, int min) {
    const unsigned char *minmatchloc = txt;
    while (min-- > 0) minmatchloc = next(minmatchloc);
    while (matchone(rex, txt)) txt = next(txt);
    while (txt >= minmatchloc) {
        if (matchhere(rexafter, txt)) return 1;
        else txt = prev(txt);
    }
    return 0;
}

int rexmatch(const unsigned char *rex, const unsigned char *txt) {
    if (rex[0] == '^') return matchhere((rex = next(rex)), txt);
    while (txt[0] != '\0') {
        if (matchhere(rex, txt)) return 1;
        else txt = next(txt);
    }
    return 0;
}
