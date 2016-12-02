#include <iostream>
#include <stdlib.h>

using namespace std;

#include "options.hpp"
#include "optparse.hpp"
#include "version.h"

Options::Options(int argc, char *argv[])
{
   /* Start with basic initializtion */
   interval = 2;
   iterations = 0;
   dump = false;
   debug = false;
   denote_sockets = false;
   column_display = COL_DISP_BASE;

   OptParse op;

   op.RegisterOption("interval", 1, READ_AULNG);
   op.RegisterOption("iterations", 2, READ_AULNG);
   
   op.RegisterOption("data dump", 'd');
   op.RegisterOption("data dump", "dump");

   op.RegisterOption("help", 'h');
   op.RegisterOption("help", "help");
   op.RegisterOption("about", 'a');
   op.RegisterOption("about", "about");

   op.RegisterOption("denote sockets", 's');
   op.RegisterOption("denote sockets", "denote-sockets");

   op.RegisterOption("display most", 'm');
   op.RegisterOption("display most", "display-most");

   op.RegisterOption("display full", 'f');
   op.RegisterOption("display full", "display-full");

   

   op.RegisterOption("debug", '+');
   
   op.Resolve(argc, argv);

   if ( op.DumpErrors() )
      return;

   if ( op.WasFound("debug") )
   {
      cerr << "Debuggery     : On" << endl;
      
      /* ARG[0] */
      cerr << "ArgZero       : " << op.arg_zero << endl;

      /* Iterations */
      cerr << "Interval      : ";
      unsigned long dinterval;
      if ( op.GetState("filename", dinterval) )
         cerr << dinterval;
      else
         cerr << "Unset";
      cerr << endl;






      
      /* Help */
      if ( op.WasFound("help") )
         cerr << "Help          : ";
      else
         cerr << "Help          : un";
      
      cerr << "set" << endl;

      /* About */
      if ( op.WasFound("about") )
         cerr << "About         : ";
      else
         cerr << "About         : un";
      cerr << "set" << endl;

      /* Dump */
      if ( op.WasFound("data dump") )
         cerr << "Dump layout   : ";
      else
         cerr << "Dump layout: un";
      cerr << "set" << endl;
   } /* End of debuggery output */

   if ( ! op.IsExclusive("about", {"dump layout", "interval", "iterations"}) )
   {
      cerr << "ERROR: The -a option is mutually exclusive of all other options." << endl;
      exit(1);
   }

   if ( ! op.IsExclusive("help", {"dump layout", "interval", "iterations"}) )
   {
      cerr << "ERROR: The -h option is mutually exclusive of all other options." << endl;
      exit(1);
   }
   
  if ( op.WasFound("help") )
  {
     int rv = 0;
     
     if ( ! op.IsExclusive("help", {"about"}) )
     {
        cerr << "WARNING: -a and -h options are mutually exclusive. Printing help." << endl;
        rv = 1;
     }

     help();
     exit(rv);
  }

  if ( op.WasFound("about") )
  {
     about();
     exit(0);
  }

  if ( op.WasFound("data dump") )
  {
     dump = true;
     /*
     if ( op.GetValue("dump data", startlno) )
     {
        cerr << "ERROR: Problems understanding the starting line argument." << endl;
        exit(1);
     }
     */
  }

  if ( op.WasFound("denote sockets") )
     denote_sockets = true;

  if ( ! op.IsExclusive("display most", {"display full"}) )
   {
      cerr << "ERROR: The -m(ost) and -f(ull) options are mutually exclusive." << endl;
      exit(1);
   }
   
  if ( op.WasFound("display most") )
     column_display = COL_DISP_MOST;

  if ( op.WasFound("display full") )
     column_display = COL_DISP_FULL;


   
   

  /* Now process the regular params */
#ifdef STUB_REMOVE
  if ( 0 >= op.GetState("filename", filename) )
  {
     cerr << "ERROR: A filename was not specified on the command line." << endl;
     exit(1);
  }

  if ( op.WasFound("number width") )
  {
     if ( op.GetValue("number width", nwidth) )
     {
        cerr << "ERROR: Problems understanding the number width argument." << endl;
        exit(1);
     }
  }

     
  if ( op.WasFound("end line") )
  {
     if ( op.GetValue("end line", endlno) )
     {
        cerr << "ERROR: Problems understanding the ending line argument." << endl;
        exit(1);
     }
  }

  /* Check values for start and end lines. */
  if ( startlno > endlno )
  {
     cerr << "ERROR: The specified start line is after the end line." << endl;
     exit(1);
  }
    
  if ( op.WasFound("no-nums") )
     nonums = true;

  if ( op.WasFound("divider") )
  {
     if ( op.GetValue("divider", divider) )
     {
        cerr << "ERROR: Problems understanding the divider argument." << endl;
        exit(1);
     }
  }
#endif
  
};

void Options::help(void)
{
   cout << "ccpu - Color CPU" << endl;
   cout << "   Usage: ccpu -a | -h | [options] [interval [iterations]]" << endl;
   cout << "   Options:" << endl;
   cout << "     -a/--about     Show about information (and exit)" << endl;
   cout << "     -h/--help      Show (this) help (and exit)" << endl;
   cout << "     -d             Dump cpu info (and exit)" << endl;
   cout << flush;
}

void Options::about(void)
{
   cout << "ccpu - Color CPU" << endl;
   cout << "   Version: " << VERSION_STRING << endl;
   cout << "   A tool for evaluating CPU and CPU scheduling" << endl;
   cout << "   William Favorite <wfavorite@tablespace.net>" << endl;
}
