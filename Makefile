.PHONY: clean lean ccstart

CPP=g++
LD_OPTS=-Wall -Werror -std=c++0x
CC_OPTS=-Wall -Werror -std=c++0x
export LANG=C

gcpu: ccstart main.o options.o optparse.o cnodes.o lcores.o rfiles.o
	@printf ".Done.\nLinking.."
	@$(CPP) $(LD_OPTS) -o gcpu main.o optparse.o options.o cnodes.o lcores.o rfiles.o
	@printf ".Done.\n"

main.o: main.cpp options.hpp cnodes.hpp rfiles.hpp
	@printf "."
	@$(CPP) $(CC_OPTS) -c main.cpp

optparse.o: optparse.cpp optparse.hpp
	@printf "."
	@$(CPP) $(CC_OPTS) -c optparse.cpp

options.o: options.cpp options.hpp optparse.hpp
	@printf "."
	@$(CPP) $(CC_OPTS) -c options.cpp

cnodes.o: cnodes.cpp cnodes.hpp
	@printf "."
	@$(CPP) $(CC_OPTS) -c cnodes.cpp

lcores.o: lcores.cpp lcores.hpp cnodes.hpp
	@printf "."
	@$(CPP) $(CC_OPTS) -c lcores.cpp

rfiles.o: rfiles.cpp rfiles.hpp
	@printf "."
	@$(CPP) $(CC_OPTS) -c rfiles.cpp

ccstart:
	@printf "Compiling."

clean: lean
	@printf "Cleaning code directory."
	@rm -f gcpu
	@printf ".Done.\n"

lean:
	@printf "Leaning code directory."
	@rm -f *.o
	@printf "."
	@rm -f core
	@printf "."
	@rm -f *~
	@printf ".Done.\n"
