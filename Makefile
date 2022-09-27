#############################################################################
##          Operating system detection and OS-specific constants
#############################################################################
HOSTOS	:= 	$(shell uname -s)
ifeq ($(HOSTOS), Darwin)
	TIMECMD	:=	time
else
	TIMECMD	:=	/usr/bin/time --format "\t%e real\t%U user\t%S sys"
endif
ifeq ($(shell cc -v 2>&1 | grep clang),"")
	CC	:=	gcc
else
	CC	:=	clang
endif

#############################################################################
##                          Compiler-specific flags
#############################################################################
ifeq ($(CC),clang)
	OPT_FLAG	=	-Oz
	DBUG_FLAG	=	-gfull -O0
	WEXCLUDE	=	-Wno-gnu-binary-literal -Wno-c++98-compat -Wno-padded \
	                -Wno-c99-compat -Wno-poison-system-directories
	WEVERYTHING	=	-Weverything 
else ifeq ($(CC),gcc)
	OPT_FLAG	=	-Os
	DBUG_FLAG	=	-pg -O0
	WEXCLUDE	=	-Wno-parentheses -Wno-padded -Wno-unused-value
	WEVERYTHING	=	
endif


#############################################################################
##      Basic build flags for different levels of strictness/debugging
#############################################################################
CFLAGS	= -std=c2x -funsigned-char 
RELEASE	= $(CFLAGS) $(OPT_FLAG) -DNDEBUG
DEBUG	= $(CFLAGS) $(DBUG_FLAG)
STRICT1	= -W -Wall -Werror 
STRICT2	= $(STRICT1) -pedantic                                          \
		  -Wstrict-prototypes -Wmissing-prototypes -Wchar-subscripts    \
		  -Wpointer-arith -Wcast-qual -Wswitch -Wshadow -Wcast-align    \
		  -Wreturn-type -Wwrite-strings -Winline -Wredundant-decls      \
		  -Wmisleading-indentation -Wunused-parameter -Wnested-externs 
STRICT3	= $(STRICT2) -Wextra 
STRICT4	= $(STRICT3) $(WEVERYTHING)



#############################################################################
##                             Target constants 
#############################################################################
EXEC	:=	$(shell basename `pwd`)
HEADERS	:=	*.h
SOURCE	:=	*.c
ALLSRC	:=  $(SOURCE) $(HEADERS)



#############################################################################
##                               Build targets 
#############################################################################
all:		test
test:		testsuiteprologue testsuite testsuiteepilogue
testsuiteprologue:
	@echo
	@echo "RUNNING TEST SUITE"
	@echo "------------------"
testsuiteepilogue:
	@echo

TESTCC		=	$(CC) $(RELEASE) $(WEXCLUDE)
TESTSTART	=	printf "%s %-36s" "Testing" $(subst test_,,$@)
TESTEND		=	printf $(SUCCESS) || printf $(FAILURE)
TESTTGT		=	TEST/testexec
SUCCESS		=	"\342\234\205 OK\n"
FAILURE		=	"\342\235\214\033[1;31m FAILED!!!\033[m\n"

testsuite:	test_regexes


test_regexes: $(ALLSRC)
	@$(TESTSTART)
	@$(TESTCC) -I. $(SOURCE) TEST/testregexes.c -o $(TESTTGT)
	@$(TESTTGT)
	@rm $(TESTTGT)



clean:
	@rm -Rf .DS_Store core *.o *~
	@rm -Rf *.dSYM/ */*.dSYM
	@rm -Rf Info.plist */Info.plist
	@rm -Rf *-output* TEST/*-output*
	@rm -Rf $(TESTTGT)
	@rm -Rf $(EXEC)
	@echo Repository cleaned
