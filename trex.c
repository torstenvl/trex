//                                    trex
//                      tiny regular expression library
//
// Written in 2022 by Joshua Lee Ockert <https://github.com/torstenvl/>
//
// Hereby released into the public domain. In jurisdictions that do not 
// recognize the dedication of work to the public domain, this work is also
// released under the ISC license. https://opensource.org/licenses/ISC

#include <stdlib.h>
#include <ctype.h>
#include "trex.h" 

// These two rely on and are relied upon by matchhere(); need upfront decls
static int match_xplus(const unsigned char *rex, const unsigned char *txt, const unsigned char *rexafter, int min);
static int match_maybe(const unsigned char *rex, const unsigned char *txt, const unsigned char *rexafter);

//  For byte patterns matching this...                                            ... get the correct bits from them, big-endian style
#define cdpt(s) ((s[0]<128) ? s[0] :\
    (s[0]>>5== 6 && s[1]>>6==2                            ) ?                                         (s[0] & 0x1F)<<6 | (s[1] & 0x3F) :\
    (s[0]>>4==14 && s[1]>>6==2 && s[2]>>6==2              ) ?                     (s[0] & 0x0F)<<12 | (s[1] & 0x3F)<<6 | (s[2] & 0x3F) :\
    (s[0]>>3==30 && s[1]>>6==2 && s[2]>>6==2 && s[3]>>6==2) ? (s[0] & 0x07)<<18 | (s[1] & 0x3F)<<12 | (s[2] & 0x3F)<<6 | (s[3] & 0x3F) :\
    s[0]) 

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
        case 'd':                return  isdigit(txt[0]);
        case 'D':                return !isdigit(txt[0]);
        case 'x':                return  isxdigit(txt[0]);
        case 'X':                return !isxdigit(txt[0]);
        case 'w':                return  (txt[0]=='_'||isalnum(txt[0]));
        case 'W':                return !(txt[0]=='_'||isalnum(txt[0]));
        case 's':                return  isspace(txt[0]);
        case 'S':                return !isspace(txt[0]);
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
