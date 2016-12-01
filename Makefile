.PHONY: clean lean ccstart

CPP=g++
LD_OPTS=-Wall -Werror -std=c++0x
CC_OPTS=-Wall -Werror -std=c++0x
export LANG=C

ccpu: ccstart main.o options.o optparse.o cnodes.o
	@printf ".Done.\nLinking.."
	@$(CPP) $(LD_OPTS) -o ccpu main.o optparse.o options.o cnodes.o
	@printf ".Done.\n"

main.o: main.cpp options.hpp cnodes.hpp
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

ccstart:
	@printf "Compiling."

clean: lean
	@printf "Cleaning code directory."
	@rm -f ccpu
	@printf ".Done.\n"

lean:
	@printf "Leaning code directory."
	@rm -f *.o
	@printf "."
	@rm -f core
	@printf "."
	@rm -f *~
	@printf ".Done.\n"
