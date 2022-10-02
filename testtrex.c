#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "trex.h"

struct testline {
    unsigned char text[40];
    unsigned char regex[40];
    int  expect; 
};

static struct testline testdata[] = {
{ "fcharset178",            "^fcharset\\d\\d\\d",                         1 },
{ "fcharset17",             "^fcharset\\d\\d\\d",                         0 },
{ "fcharset««",             "^fcharset«+",                                1 },
{ "fcharset",               "^fcharset«+",                                0 },
{ "fcharset",               "^fcharset«*",                                1 },
{ "Hello_World",            "Hello\\sWorld",                              0 },
{ "Hello World",            "Hello ?World",                               1 },
{ "HelloWorld",             "Hello ?World",                               1 },
{ "Hello  World",           "Hello ?World",                               0 },
{ "abdcbabcdefg",           "abc",                                        1 },
{ "abdcbabcdefg",           "^abc",                                       0 },
{ "https://google.com",     "^.*s://",                                    1 },
{ "http://google.com",      "^.*s://",                                    0 },
{ "\\\'ab",                 "\\\\\\'\\x\\x",                              1 },
{ "\\uc1\\u171\\\'ab",      "\\\\\\'\\x\\x",                              1 },
{ "\\uc1\\u171\\\'ab",      "^\\\\\\'\\x\\x",                             0 },
{ "\\\'fg",                 "\\\\\\'\\x\\x",                              0 },
{ "(505) 867-5309",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",      1 },
{ "(865) 409-1021",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d$",     1 },
{ "(865) 409-I021",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",      0 },
{ "(505) 86765309",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",      0 },
{ "(865) 409-1021",         "\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",     1 },
{ "(865) 409-1021",         "^\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",    0 },
{ "5",                      "\\d",                                        1 },
{ "hej",                    "\\w+",                                       1 },
{ "hej",                    "\\D",                                        1 },
{ "hej",                    "\\d",                                        0 },
};

static struct testline ctestdata[] = {
{ "élan",                   "^\\w+$",                                     0 },
{ "flêche",                 "^\\w+$",                                     0 },
{ "régulières",             "^\\w+$",                                     0 },
{ "zÇUùÑ_yÿáCêÑ5Ä6ÑøýCTFoæ","^\\w+$",                                     0 },
{ "NéÊêËµ62aßªÈÈfÞÀvFlïZdc","^\\w+$",                                     0 },
{ "îÍz9ÅaåGÌÎhàjþõÖUÿõÀR",  "^\\w+$",                                     0 },
};

static struct testline frtestdata[] = {
{ "élan",                   "^\\w+$",                                     1 },
{ "flêche",                 "^\\w+$",                                     1 },
{ "régulières",             "^\\w+$",                                     1 },
{ "zÇUùÑ_yÿáCêÑ5Ä6ÑøýCTFoæ","^\\w+$",                                     1 },
{ "NéÊêËµ62aßªÈÈfÞÀvFlïZdc","^\\w+$",                                     1 },
{ "îÍz9ÅaåGÌÎhàjþõÖUÿõÀR",  "^\\w+$",                                     1 },
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
    nrecords = sizeof(frtestdata) / sizeof(struct testline);
    td = frtestdata;
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
