#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include "trex.h"

struct testline {
    unsigned char text[40];
    unsigned char regex[40];
    int  expect; 
};

static struct testline testdata[] = {
{ u8"fcharset178",          u8"^fcharset\\d\\d\\d",                       1 },
{ u8"fcharset17",           u8"^fcharset\\d\\d\\d",                       0 },
{ u8"fcharset««",           u8"^fcharset«+",                              1 },
{ u8"fcharset",             u8"^fcharset«+",                              0 },
{ u8"fcharset",             u8"^fcharset«*",                              1 },
{ u8"Hello_World",          u8"Hello\\sWorld",                            0 },
{ u8"Hello World",          u8"Hello ?World",                             1 },
{ u8"HelloWorld",           u8"Hello ?World",                             1 },
{ u8"Hello  World",         u8"Hello ?World",                             0 },
{ u8"abdcbabcdefg",         u8"abc",                                      1 },
{ u8"abdcbabcdefg",         u8"^abc",                                     0 },
{ u8"https://google.com",   u8"^.*s://",                                  1 },
{ u8"http://google.com",    u8"^.*s://",                                  0 },
{ u8"\\\'ab",               u8"\\\\\\'\\x\\x",                            1 },
{ u8"\\uc1\\u171\\\'ab",    u8"\\\\\\'\\x\\x",                            1 },
{ u8"\\uc1\\u171\\\'ab",    u8"^\\\\\\'\\x\\x",                           0 },
{ u8"\\\'fg",               u8"\\\\\\'\\x\\x",                            0 },
{ u8"(505) 867-5309",       u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",    1 },
{ u8"(865) 409-1021",       u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d$",   1 },
{ u8"(865) 409-I021",       u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",    0 },
{ u8"(505) 86765309",       u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",    0 },
{ u8"(865) 409-1021",       u8"\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",   1 },
{ u8"(865) 409-1021",       u8"^\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",  0 },
{ u8"5",                    u8"\\d",                                      1 },
{ u8"hej",                  u8"\\w+",                                     1 },
{ u8"hej",                  u8"\\D",                                      1 },
{ u8"hej",                  u8"\\d",                                      0 },
};

static struct testline ctestdata[] = {
{ u8"élan",                 u8"^\\w+$",                                   0 },
{ u8"flêche",               u8"^\\w+$",                                   0 },
{ u8"régulières",           u8"^\\w+$",                                   0 },
{ u8"zÇUùÑ_yÿáCêÑ5Ä6ÑøýCT", u8"^\\w+$",                                   0 },
{ u8"NéÊêËµ62aßªÈÈfÞÀvFlï", u8"^\\w+$",                                   0 },
{ u8"îÍz9ÅaåGÌÎhàjþõÖUÿõÀR",u8"^\\w+$",                                   0 },
{ u8"Բարեւ_Ձեզ։",           u8"^\\w+$",                                   0 },
{ u8"Բարեւ_Ձեզ",           u8"^\\w+$",                                   0 },
};

static struct testline loctestdata[] = {
{ u8"élan",                 u8"^\\w+$",                                   1 },
{ u8"flêche",               u8"^\\w+$",                                   1 },
{ u8"régulières",           u8"^\\w+$",                                   1 },
{ u8"zÇUùÑ_yÿáCêÑ5Ä6ÑøýCT", u8"^\\w+$",                                   1 },
{ u8"NéÊêËµ62aßªÈÈfÞÀvFlï", u8"^\\w+$",                                   1 },
{ u8"îÍz9ÅaåGÌÎhàjþõÖUÿõÀR",u8"^\\w+$",                                   1 },
{ u8"Բարեւ_Ձեզ։",           u8"^\\w+$",                                   0 },
{ u8"Բարեւ_Ձեզ",           u8"^\\w+$",                                   1 },
};

static const char YES[] = "yes";
static const char NO[]  = "no";
static const char LOUDYES[] = { 0xE2, 0x9C, 0x85, 0x00 };
static const char LOUDNO[]  = { 0xE2, 0x9D, 0x8C, 0x00 };

int main(void) {
    struct testline *td = testdata;
    size_t nrecords = sizeof(testdata) / sizeof(struct testline);
    size_t i;
    int result;

    printf("\n\n");
    printf("%-20s\t\t%-30s\tRes/Exp\n", "String", "Regex");
    printf("————————————————————————————————————————————————————————————————————————————————\n");
    nrecords = sizeof(testdata) / sizeof(struct testline);
    td = testdata;
    for (i = 0; i < nrecords; i++) {
        result = regexmatch(td[i].regex, td[i].text);

        printf("%-20s\t\t%-30s\t%3s %3s  %s %s\n", 
               td[i].text, td[i].regex,
               (result)?YES:NO, (td[i].expect)?YES:NO,
               (result == td[i].expect)?LOUDYES:LOUDNO,
               (result == td[i].expect)?"good":"FAIL");
    }

    setlocale(LC_ALL, "C");
    printf("\nLocale:  C\n");
    printf("%-20s\t\t%-30s\tRes/Exp\n", "String", "Regex");
    printf("————————————————————————————————————————————————————————————————————————————————\n");
    nrecords = sizeof(ctestdata) / sizeof(struct testline);
    td = ctestdata;
    for (i = 0; i < nrecords; i++) {
        result = regexmatch(td[i].regex, td[i].text);

        printf("%-20s\t\t%-30s\t%3s %3s  %s %s\n", 
               td[i].text, td[i].regex,
               (result)?YES:NO, (td[i].expect)?YES:NO,
               (result == td[i].expect)?LOUDYES:LOUDNO,
               (result == td[i].expect)?"good":"FAIL");
    }
    
    setlocale(LC_ALL, "en_US.UTF-8");
    printf("\nLocale:  en_US.UTF-8\n");
    printf("%-20s\t\t%-30s\tRes/Exp\n", "String", "Regex");
    printf("————————————————————————————————————————————————————————————————————————————————\n");
    nrecords = sizeof(loctestdata) / sizeof(struct testline);
    td = loctestdata;
    for (i = 0; i < nrecords; i++) {
        result = regexmatch(td[i].regex, td[i].text);

        printf("%-20s\t\t%-30s\t%3s %3s  %s %s\n", 
               td[i].text, td[i].regex,
               (result)?YES:NO, (td[i].expect)?YES:NO,
               (result == td[i].expect)?LOUDYES:LOUDNO,
               (result == td[i].expect)?"good":"FAIL");
    }


    return 0;
}
