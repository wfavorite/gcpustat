#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>

#include "optparse.hpp"

/* Option values for what columns to display */
const int COL_DISP_BASE = 1;
const int COL_DISP_MOST = 2;
const int COL_DISP_FULL = 3;

const int DUMP_OFF      = 0;
const int DUMP_SIMPLE   = 1;
const int DUMP_DETAILED = 2;

struct Options
{
   unsigned long interval;
   unsigned long iterations;
   int dump;
   bool debug;

   bool denote_sockets;       /* Denote socket boundaries in output.         */
   int column_display;
   
   /* Potential options:
      -l logical ordering
      -n numerical ordering
      -i show interrupts
      -4 Just user,nice,system,idle
      -s Denote socket boundaries
   */

   Options(int argc, char *argv[]);

   void help(void);
   void about(void);
};



















#endif
