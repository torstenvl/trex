#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
#                                CONFIGURATION
#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
override \
CFLAGS     +=  -funsigned-char \
               -W -Wall -Wextra -Werror \
               -Wno-unknown-warning -Wno-unknown-warning-option -Wno-padded \
               -Wno-parentheses -Wno-c99-compat -Wno-unused-function \
               -Isrc -Itemplib \
			   -DBUILDSTAMP=$(shell date +"%Y%m%d.%H%M%S")

ifdef STACKDEBUG
override CFLAGS += -O1 -ggdb3 -DulNEEDTRACE
else ifdef DEBUG
override CFLAGS += -O1 -ggdb3
else
override CFLAGS += -Ofast -DNDEBUG
endif

FORCEPREP  :=  $(shell mkdir -p templib &&  find . -iregex ".*src/.*\.[ch].*" -exec cp -a {} templib/ \;)
VPATH       =  templib



#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
#                                   TARGETS
#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
.PHONY:			clean test
all:			test

%.o : %.c
	@$(CC) $(CFLAGS) -c $<

clean:
	@rm -fr   .DS_Store    Thumbs.db    core    *.dSYM    *.o
	@rm -fr */.DS_Store  */Thumbs.db  */core  */*.dSYM  */*.o
	-@$(foreach dir,$(shell find modules -type d -depth 1 2>/dev/null),$(MAKE) clean -C $(dir) 2>/dev/null;)
	@rm -fr $(TESTEXE) templib



#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
#                                 TEST HARNESS
#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
TESTEXE     =   ./testprog
TESTSTART   =   printf "%s %-36s" "Testing" $(subst test_,,$@...)
TESTEND     =   printf "\342\234\205 OK\n" || printf "\342\235\214\033[1;31m FAILED!!!\033[m\n"
TESTCC      =   $(CC) $(CFLAGS) -o $(TESTEXE)
test: 			testbegin testsuite testfinish
testbegin:	;	@printf "RUNNING TEST SUITE\n——————————————————\n" 
testfinish:	;	@rm -fr $(TESTEXE) templib temp.rtf test/latepartial-output.rtf test/letter-output.rtf



#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
#                                SPECIFIC TESTS
#–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
testsuite:	test_regexes

test_regexes: trex.o TEST/testtrex.c
	@$(TESTSTART)
	@$(TESTCC) trex.o TEST/testtrex.c -o $(TESTEXE)
	@$(TESTEXE) && $(TESTEND)
