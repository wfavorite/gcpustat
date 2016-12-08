#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>

#include "optparse.hpp"

/* What is options.Xpp about?
   - It is a wrapper of "local/non-library code" for options parsing. It is
     basically what you would have to write if you were using optind/optarg
     kind of parsing in C.
   - It is typically handed off to functions across the code base as a 
     container for user intent. Here... it is used somewhat differently in
     that most of the *long term* (repeadetly used) options are copied into
     the data collection and displaying classes where they are used locally.
*/


/* Option values for what columns to display. (Used in cnodes.cpp)           */
/* The first three bits store an absolute (mutually exclusive) value of what
   "core" stats to DISPlay. The remainder bits store FLaGs of optional stats
   to tack onto the end of the stats line. The MASK value is used to mask out
   the first three bits. */
const int COL_DISP_BASE =  1;
const int COL_DISP_MOST =  2;
const int COL_DISP_FULL =  3;
const int COL_DISP_MASK =  3;
/* These are independent of the others                                       */
const int COL_FLG_SPEED =  4; /* Actual speed in MHz         */
const int COL_FLG_PCTSP =  8; /* Percentage of maximum speed */
const int COL_FLG_IRQ   = 16;  

/* Option values for the dump (layout) options. (Used in main.cpp.)          */
const int DUMP_OFF      = 0;
const int DUMP_SIMPLE   = 1;
const int DUMP_DETAILED = 2;

const int OUT_FORMAT_LOGICAL = 0;
const int OUT_FORMAT_ORDERED = 1;
const int OUT_FORMAT_DEFAULT = OUT_FORMAT_LOGICAL;

struct Options
{
   unsigned long interval;
   unsigned long iterations;  /* 0 means "forever"                           */
   int dump;                  /* Uses DUMP_* to determine what kind of layout
                                 dump to use.                                */
   bool denote_sockets;       /* Denote socket boundaries in output.         */
   int column_display;        /* Uses COL_DISP_* to determine what columns to
                                 display in the stats output.                */
   bool show_cur_speed;       /* Simple flag option for showing current 
                                 speed in the iterative output.              */
   bool debug;                /* Debuggery option to enable some extra 
                                 printing. Really about debugging OptParse.  */
   int output_format;         /* The format of stats output: ORDERED or
                                 LOGICAL.                                    */
   bool show_timestamp;
   bool monochrome;
   
   /* Everything happens here (in the constructor). */
   Options(int argc, char *argv[]);

   void help(void);
   void about(void);
};

#endif
