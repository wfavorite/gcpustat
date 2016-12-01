#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>

#include "optparse.hpp"


struct Options
{
   unsigned long interval;
   unsigned long iterations;
   bool dump;
   bool debug;

   Options(int argc, char *argv[]);

   void help(void);
   void about(void);
};



















#endif
