#include <stdio.h>
#include <stdlib.h>
#include "trex.h"

struct testline {
    unsigned char text[40];
    unsigned char regex[40];
    int  expect; 
};

static struct testline testdata[] = {
{ "Hello World",            "Hello\\sWorld",                              1 },
{ "Hello_World",            "Hello\\sWorld",                              0 },
{ "HelloRWorld",            "Hello\\sWorld",                              0 },
{ "Hello World",            "Hello ?World",                               1 },
{ "HelloWorld",             "Hello ?World",                               1 },
{ "Hello  World",           "Hello ?World",                               0 },
{ "fcharset178",            "^fcharset\\d\\d\\d",                         1 },
{ "fcharset204",            "^fcharset\\d\\d\\d",                         1 },
{ "fcharset17",             "^fcharset\\d\\d\\d",                         0 },
{ "fcharset17",             "^fcharset\\d+",                              1 },
{ "fcharset2",              "^fcharset\\d+",                              1 },
{ "fcharset1",              "^fcharset\\d+",                              1 },
{ "fcharset07",             "^fcharset\\d+",                              1 },
{ "fcharset",               "^fcharset\\d+",                              0 },
{ "fcharset",               "^fcharset\\d*",                              1 },
{ "fchaéset",               "^fchaéset\\d*",                              1 },
{ "fcharset««",             "^fcharset«+",                                1 },
{ "fcharset",               "^fcharset«+",                                0 },
{ "fcharset",               "^fcharset«*",                                1 },
{ "http://google.com",      "^.*tp",                                      1 },
{ "http://google.com",      "tp",                                         1 },
{ "Fellows of Harvard",     "l+",                                         1 },
{ "Fellow chickens",        "ll+",                                        1 },
{ "boo5bear",               "\\d",                                        1 },
{ "boobear",                "\\x",                                        1 },
{ "oobear",                 "\\x",                                        1 },
{ "obear",                  "\\x",                                        1 },
{ "\\\'ab",                 "\\\\\\'\\x\\x",                              1 },
{ "\\rtf\\\'ff",            "\\\\\\'\\x\\x",                              1 },
{ "\\\'fg",                 "\\\\\\'\\x\\x",                              0 },
{ "\\\'00",                 "\\\\\\'\\x\\x",                              1 },
{ "(505) 867-5309",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",      1 },
{ "(865) 409-1021",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d$",     1 },
{ "(865) 409-I021",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",      0 },
{ "(505) 86765309",         "^(\\d\\d\\d) \\d\\d\\d\\-\\d\\d\\d\\d",      0 },
{ "(865) 409-1021",         ".*\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",   1 },
{ "(865) 4O9-1021",         ".*\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",   0 },
{ "(865) 409-1021",         "\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",     1 },
{ "(865) 409-1021",         "^\\d\\d\\d\\D*\\d\\d\\d\\D*\\d\\d\\d\\d",    0 },
{ "abdcbabcdefg",           "abc",                                        1 },
{ "abdcbabcdefg",           "crex",                                       0 },
{ "abdcbabcdefg",           "^abc",                                       0 },
{ "5",                      "\\d",                                        1 },
{ "hej",                    "\\w+",                                       1 },
{ "5",                      "\\D",                                        0 },
{ "hej",                    "\\W+",                                       0 },
{ "hej",                    "\\D",                                        1 },
{ "hej",                    "\\d",                                        0 },
{ "0|",                     "0|",                                         1 },
{ "0s:00:00",               "\\d\\d:\\d\\d:\\d\\d",                       0 },
{ "000:00",                 "\\d\\d:\\d\\d:\\d\\d",                       0 },
{ "00:0000",                "\\d\\d:\\d\\d:\\d\\d",                       0 },
{ "100:0:00",               "\\d\\d:\\d\\d:\\d\\d",                       0 },
{ "00:100:00",              "\\d\\d:\\d\\d:\\d\\d",                       0 },
{ "0:00:100",               "\\d\\d:\\d\\d:\\d\\d",                       0 },
{ "0:0:0",                  "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "0:00:0",                 "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "0:0:00",                 "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "00:0:0",                 "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "00:00:0",                "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "00:0:00",                "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "0:00:00",                "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "00:00:00",               "\\d\\d?:\\d\\d?:\\d\\d?",                    1 },
{ "a:0",                    "\\d\\d?:\\d\\d?:\\d\\d?",                    0 },
{ "c:\\Tools",              "^.*\\\\.*$",                                 1 },
{ "Xaa",                    "a*$",                                        1 },
{ "abcabc",                 ".*c",                                        1 },
{ "abcabc",                 ".+c",                                        1 },
};

static const char YES[] = "yes";
static const char NO[]  = "no";
static const char LOUDYES[] = { 0xE2, 0x9C, 0x85, 0x00 };
static const char LOUDNO[]  = { 0xE2, 0x9D, 0x8C, 0x00 };


int main(void) {
    struct testline *td     = testdata;
    size_t nrecords  = sizeof(testdata) / sizeof(struct testline);
    size_t i;
    int result;

    printf(" \n%-20s\t\t%-30s\tRes/Exp\n", "String", "Regex");
    printf("--------------------------------------------------------------------------------\n");
    for (i = 0; i < nrecords; i++) {
        result = regexmatch(td[i].regex, td[i].text);

        printf("%-20s\t\t%-30s\t%3s %3s  %s %s\n", 
               td[i].text, td[i].regex,
               (result)?YES:NO, (td[i].expect)?YES:NO,
               (result == td[i].expect)?LOUDYES:LOUDNO,
               (result == td[i].expect)?"good":"FAIL");
        // printf("Idx/Match:%3d/%1s  Expect:%3d/%1s    TEST: %2s\n",  
    }


    return 0;
}
