ifeq ($(shell cc -v 2>&1 | grep clang),"")
	CC           :=  gcc
	WEXCLUDE     :=	
	WEVERYTHING  :=	
    CFLAGS       :=  -std=c11 -funsigned-char -Os
    STRICT       :=  -W -Wall -Werror -Wextra $(WEVERYTHING) $(WEXCLUDE)
else
	CC           :=  clang
	WEXCLUDE     :=  -Wno-poison-system-directories
	WEVERYTHING  :=  -Weverything 
    CFLAGS       :=  -std=c11 -funsigned-char -Oz
    STRICT       :=  -W -Wall -Werror -Wextra $(WEVERYTHING) $(WEXCLUDE)
endif

all:		test
test:		testsuiteprologue testsuite testsuiteepilogue
testsuiteprologue:
	@echo
	@echo "RUNNING TEST SUITE"
	@echo "——————————————————"
testsuiteepilogue:
	@echo

TESTSTART   =   printf "%s %-36s\n" "Testing" $(subst test_,,$@)
TESTCC      :=  $(CC) $(CFLAGS) $(STRICT)
TESTTGT     :=  ./testexec

testsuite:	test_regexes

test_regexes: $(ALLSRC)
	@$(TESTSTART)
	@$(TESTCC) testtrex.c trex.c -o $(TESTTGT)
	@$(TESTTGT)
#	@rm $(TESTTGT)

clean:
	@rm -Rf .DS_Store core *.o *~
	@rm -Rf *.dSYM/ */*.dSYM
	@rm -Rf $(TESTTGT)
	@echo Repository cleaned
