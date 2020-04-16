# Makefile do EP1 - Dormedorme Turtle

STD = -ansi
OP1 = -pedantic -Wall -Wextra -Wpedantic -Wshadow
OP2 = -Wno-unused-result -Wno-unused-function -Wno-unused-parameter
OP3 = -fdiagnostics-color=always
OPM = -O3
GDB = -g
# LM = -lm

VERBOSE = -v

.FORCE :

default : 
	@echo "Para compilar prog.c diga make prog"
	@echo ""

%: %.c .FORCE
	gcc $< -o $@.bin $(STD) $(OP1) $(OP2) $(OP3) $(OPTM) $(LM)

tar:
	rm      -rf                     $(NUSP)
	rm      -f                      $(NUSP).tar

	mkdir                           $(NUSP)
	cp              turtledorm.c    $(NUSP)/turtledorm.c
	cp              Makefile        $(NUSP)/Makefile
	echo            $(EMAIL)      > $(NUSP)/email.txt
	echo            $(NUSP)       > $(NUSP)/nusp.txt

	tar     -cvf    $(NUSP).tar     $(NUSP)
	rm -rf                          $(NUSP)
