//                                    trex
//                      tiny regular expression library
//
// Written in 2022 by Joshua Lee Ockert <https://github.com/torstenvl/>
//
// Hereby released into the public domain. In jurisdictions that do not 
// recognize the dedication of work to the public domain, this work is also
// released under the ISC license. https://opensource.org/licenses/ISC

#ifndef TREX__
#define TREX__

#define regexmatch(...) rexmatch(__VA_ARGS__)
int rexmatch(const unsigned char *rex, const unsigned char *txt);

#endif
