#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <locale.h>
#include "trex.h"

struct testline {
    int expect; 
    int exttab;
    unsigned char regex[40];
    unsigned char text[40];
};

static struct testline testdata[] = {
{1, 0, u8"^fcharset\\d\\d\\d",                      u8"fcharset178"          },
{0, 0, u8"^fcharset\\d\\d\\d",                      u8"fcharset17"           },
{1, 0, u8"^fcharset«+",                             u8"fcharset««"           },
{0, 0, u8"^fcharset«+",                             u8"fcharset"             },
{1, 0, u8"^fcharset«*",                             u8"fcharset"             },
{0, 0, u8"Hello\\sWorld",                           u8"Hello_World"          },
{1, 0, u8"Hello ?World",                            u8"Hello World"          },
{1, 0, u8"Hello ?World",                            u8"HelloWorld"           },
{0, 0, u8"Hello ?World",                            u8"Hello  World"         },
{1, 0, u8"abc",                                     u8"abdcbabcdefg"         },
{0, 0, u8"^abc",                                    u8"abdcbabcdefg"         },
{1, 0, u8"^.*s://",                                 u8"https://google.com"   },
{0, 0, u8"^.*s://",                                 u8"http://google.com"    },
{1, 0, u8"\\\\\\'\\x\\x",                           u8"\\\'ab"               },
{1, 0, u8"\\\\\\'\\x\\x",                           u8"\\uc1\\u171\\\'ab"    },
{0, 0, u8"^\\\\\\'\\x\\x",                          u8"\\uc1\\u171\\\'ab"    },
{0, 0, u8"\\\\\\'\\x\\x",                           u8"\\\'fg"               },
{1, 0, u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",   u8"(505) 867-5309"       },
{1, 0, u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d$",  u8"(865) 409-1021"       },
{0, 0, u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",   u8"(865) 409-I021"       },
{0, 0, u8"^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",   u8"(505) 86765309"       },
{1, 0, u8"\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",  u8"(865) 409-1021"       },
{0, 0, u8"^\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d", u8"(865) 409-1021"       },
{1, 0, u8"\\d",                                     u8"5"                    },
{1, 0, u8"\\w+",                                    u8"hej"                  },
{1, 0, u8"\\D",                                     u8"hej"                  },
{0, 0, u8"\\d",                                     u8"hej"                  },
};

static struct testline ctestdata[] = {
{0, 0, u8"^\\w+$",                                  u8"élan"                 },
{0, 0, u8"^\\w+$",                                  u8"flêche"               },
{0, 0, u8"^\\w+$",                                  u8"régulières"           },
{0, 0, u8"^\\w+$",                                  u8"zÇUùÑ_yÿáCêÑ5Ä6ÑøýCT" },
{0, 0, u8"^\\w+$",                                  u8"NéÊêËµ62aßªÈÈfÞÀvFlï" },
{0, 0, u8"^\\w+$",                                  u8"îÍz9ÅaåGÌÎhàjþõÖUÿõÀR"},
{0, 1, u8"^\\w+$",                                  u8"Բարեւ_Ձեզ։"           },
{0, 1, u8"^\\w+$",                                  u8"Բարեւ_Ձեզ"            },
{0, 1, u8"^\\w+$",                                  u8"مرحبًا؟"               },
{0, 1, u8"^\\w+$",                                  u8"مرحبًا"                },
{0, 1, u8"^\\w+$",                                  u8"🇺🇸"                   },
};

static struct testline loctestdata[] = {
{1, 0, u8"^\\w+$",                                  u8"élan"                 },
{1, 0, u8"^\\w+$",                                  u8"flêche"               },
{1, 0, u8"^\\w+$",                                  u8"régulières"           },
{1, 0, u8"^\\w+$",                                  u8"zÇUùÑ_yÿáCêÑ5Ä6ÑøýCT" },
{1, 0, u8"^\\w+$",                                  u8"NéÊêËµ62aßªÈÈfÞÀvFlï" },
{1, 0, u8"^\\w+$",                                  u8"îÍz9ÅaåGÌÎhàjþõÖUÿõÀR"},
{0, 1, u8"^\\w+$",                                  u8"Բարեւ_Ձեզ։"           },
{1, 1, u8"^\\w+$",                                  u8"Բարեւ_Ձեզ"            },
{0, 1, u8"^\\w+$",                                  u8"مرحبًا؟"               },
{1, 1, u8"^\\w+$",                                  u8"مرحبًا"                },
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

        printf("%-20s\t\t%s%-30s\t%3s %3s  %s %s\n", 
               td[i].text, (td[i].exttab)?"\t":"", td[i].regex,
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

        printf("%-20s\t\t%s%-30s\t%3s %3s  %s %s\n", 
               td[i].text, (td[i].exttab)?"\t":"", td[i].regex,
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

        printf("%-20s\t\t%s%-30s\t%3s %3s  %s %s\n", 
               td[i].text, (td[i].exttab)?"\t":"", td[i].regex,
               (result)?YES:NO, (td[i].expect)?YES:NO,
               (result == td[i].expect)?LOUDYES:LOUDNO,
               (result == td[i].expect)?"good":"FAIL");
    }


    return 0;
}
