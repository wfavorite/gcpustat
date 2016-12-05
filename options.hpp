#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>

#include "optparse.hpp"

/* Option values for what columns to display. (Used in cnodes.cpp)           */
const int COL_DISP_BASE = 1;
const int COL_DISP_MOST = 2;
const int COL_DISP_FULL = 3;
const int COL_DISP_MASK = 3;
const int COL_FLG_SPEED = 4;  /* This is independent of the others           */

/* Option values for the dump (layout) options. (Used in main.cpp.)          */
const int DUMP_OFF      = 0;
const int DUMP_SIMPLE   = 1;
const int DUMP_DETAILED = 2;

struct Options
{
   unsigned long interval;
   unsigned long iterations;
   int dump;                  /* Uses DUMP_* to determine what kind of layout
                                 dump to use.                                */
   bool denote_sockets;       /* Denote socket boundaries in output.         */
   int column_display;        /* Uses COL_DISP_* to determine what columns to
                                 display in the stats output.                */
   bool show_cur_speed;       /* Simple flag option for showing current 
                                 speed in the iterative output.              */
   bool debug;
   
   /* Potential options:
      -l logical ordering
      -n numerical ordering
      -i show interrupts
   */

   Options(int argc, char *argv[]);

   void help(void);
   void about(void);
};

#endif
