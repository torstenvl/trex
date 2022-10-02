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
#ifndef TREX__
#define TREX__

#define regexmatch(...) rexmatch(__VA_ARGS__)
int rexmatch(const unsigned char *rex, const unsigned char *txt);

#endif
