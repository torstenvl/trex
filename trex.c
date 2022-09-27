#include <stdlib.h>
#include <ctype.h>
#include "trex.h" 

int codepointof(const unsigned char *s);
const unsigned char *getnext(const unsigned char *regex);
const unsigned char *getprev(const unsigned char *regex);
int   matchhere(const unsigned char *regex, const unsigned char *text);
int    matchone(const unsigned char *regex, const unsigned char *text);
int  matchclass(const unsigned char *regex, const unsigned char *text);
int match_maybe(const unsigned char *regex, const unsigned char *text, const unsigned char *regexafter);
int match_xplus(const unsigned char *regex, const unsigned char *text, const unsigned char *regexafter, int min);

int codepoint(const unsigned char *s) {
  return (s[0]<=0x7F) ? s[0] : 
    (0xC0<=s[0] && s[0]<=0xDF && 0x80<=s[1] && s[1]<=0xBF) ? (s[0]&0x1F)<<6  | (s[1]&0x3F) :
    (0xE0<=s[0] && s[0]<=0xEF && 0x80<=s[1] && s[1]<=0xBF && 0x80<=s[2] && s[2]<=0xBF) ? (s[0]&0x0F)<<12 | (s[1]&0x3F)<<6  | (s[2]&0x3F) : 
    (0xF0<=s[0] && s[0]<=0xF7 && 0x80<=s[1] && s[1]<=0xBF && 0x80<=s[2] && s[2]<=0xBF && 0x80<=s[3] && s[3]<=0xBF) ?
    (s[0]&0x07)<<18 | (s[1]&0x3F)<<12 | (s[2] & 0x3F)<<6 | (s[3] & 0x3F) : s[0]; 
}

const unsigned char *getnext(const unsigned char *regex) {
    const unsigned char *p = regex + 1;
    while (*p >> 6 == 0x02) p++; // UTF-8 continuation byte
    return p;
}

const unsigned char *getprev(const unsigned char *regex) {
    const unsigned char *p = regex - 1;
    while (*p >> 6 == 0x02) p--; // UTF-8 continuation byte
    return p;
}

int regexmatch(const unsigned char *regex, const unsigned char *text) {
    if (regex[0] == '^') return matchhere((regex = getnext(regex)), text);
    while (text[0] != '\0')
        if (matchhere(regex, text)) return 1;
        else text = getnext(text);
    return 0;
}

int matchhere(const unsigned char *regex, const unsigned char *text) {
    const unsigned char *nextregex = getnext(regex);
    if (regex[0] == '\0') 
        return 1;
    if (regex[0] == '\\')
        nextregex = getnext(nextregex);
    if (nextregex[0] == '?')
        return match_maybe(regex, text, getnext(nextregex));
    if (nextregex[0] == '*')
        return match_xplus(regex, text, getnext(nextregex), 0);
    if (nextregex[0] == '+')
        return match_xplus(regex, text, getnext(nextregex), 1);
    if (regex[0] == '$'  &&  nextregex[0] == '\0') 
        return (text[0] == '\0')  ?  1  :  0;
    if  (matchone(regex, text))
        return matchhere(nextregex, getnext(text));
    return 0;
}

int matchone(const unsigned char *regex, const unsigned char *text) {
    if (text[0] == '\0')     return 0;
    if (regex[0] == '.')     return 1;
    if (regex[0] == '\\')    return matchclass(getnext(regex), text);
    if (codepoint(regex) == codepoint(text)) 
                             return 1;
    return 0;
}

int matchclass(const unsigned char *regex, const unsigned char *text) {
    switch (regex[0]) {  // Add custom classes here in lieu of ranges
        case 'd': return  isdigit(text[0]);
        case 'D': return !isdigit(text[0]);
        case 'x': return  isxdigit(text[0]);
        case 'X': return !isxdigit(text[0]);
        case 'w': return  (text[0]=='_'||isalnum(text[0]));
        case 'W': return !(text[0]=='_'||isalnum(text[0]));
        case 's': return  isspace(text[0]);
        case 'S': return !isspace(text[0]);
        default:  return  (regex[0] == text[0]);
    }
}

int match_maybe(const unsigned char *regex, const unsigned char *text, const unsigned char *regexafter) {
    if (matchhere(regexafter, text)) return 1;
    if (matchone(regex, text) && matchhere(regexafter, getnext(text)) )
        return 1;
    return 0;
}

int match_xplus(const unsigned char *regex, const unsigned char *text, const unsigned char *regexafter, int min) {
    const unsigned char *minmatchloc = text;
    while (min-- > 0) minmatchloc = getnext(minmatchloc);
    while (matchone(regex, text)) text = getnext(text);
    while (text >= minmatchloc)
        if (matchhere(regexafter, text)) return 1;
        else text = getprev(text);
    return 0;
}
