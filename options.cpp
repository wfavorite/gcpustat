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
   dump = DUMP_OFF;
   debug = false;
   denote_sockets = false;
   column_display = COL_DISP_BASE;

   OptParse op;

   op.RegisterOption("interval", 1, READ_AULNG);
   op.RegisterOption("iterations", 2, READ_AULNG);
   
   op.RegisterOption("simple dump", 'D');
   op.RegisterOption("simple dump", "simple-dump");

   op.RegisterOption("detailed dump", 'd');
   op.RegisterOption("detailed dump", "detailed-dump");

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

   op.RegisterOption("display speed", 'p');
   op.RegisterOption("display speed", "display-speed");

   op.RegisterOption("debug", '+');

   op.Resolve(argc, argv);

   if ( op.DumpErrors() )
      return;

   if ( op.WasFound("debug") )
   {
      cerr << "Debuggery     : On" << endl;
      
      /* ARG[0] */
      cerr << "ArgZero       : " << op.arg_zero << endl;

      /* Iterations & interval */
      cerr << "Interval      : ";
      unsigned long dinterval;
      if ( op.GetState("interval", dinterval) )
         cerr << dinterval;
      else
         cerr << "Unset";
      cerr << endl;

      cerr << "Iterations    : ";
      unsigned long diterations;
      if ( op.GetState("iterations", diterations) )
         cerr << diterations;
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

      /* Dump types */
      if ( op.WasFound("simple dump") )
         cerr << "Simple dump   : ";
      else
         cerr << "Simple dump   : un";
      cerr << "set" << endl;

      if ( op.WasFound("detailed dump") )
         cerr << "Detailed dump : ";
      else
         cerr << "Detailed dump : un";
      cerr << "set" << endl;

      if ( op.WasFound("denote sockets") )
         cerr << "Denote sockets: ";
      else
         cerr << "Denote sockets: un";
      cerr << "set" << endl;

      if ( op.WasFound("display most") )
         cerr << "Display most  : ";
      else
         cerr << "Display most  : un";
      cerr << "set" << endl;

      if ( op.WasFound("display full") )
         cerr << "Display full  : ";
      else
         cerr << "Display full  : un";
      cerr << "set" << endl;
      
   } /* End of debuggery output */

   if ( ! op.IsExclusive("about", {"simple dump", "detailed dump", "interval", "iterations"}) )
   {
      cerr << "ERROR: The -a option is mutually exclusive of all other options." << endl;
      exit(1);
   }

   if ( ! op.IsExclusive("help", {"simple dump", "detailed dump", "interval", "iterations"}) )
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

  /* Now process the regular params */
  if ( ! op.IsExclusive("simple dump", {"detailed dump"}) )
   {
      cerr << "ERROR: The -d(etailed) and -D (simple) options are mutually exclusive." << endl;
      exit(1);
   }

  if ( op.WasFound("detailed dump") )
     dump = DUMP_DETAILED;

  if ( op.WasFound("simple dump") )
     dump = DUMP_SIMPLE;
  
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

  if ( op.WasFound("display speed") )
     column_display |= COL_FLG_SPEED;

  op.GetState("interval", interval);
  op.GetState("iterations", iterations);
  
};

void Options::help(void)
{
   cout << "ccpu - Color CPU" << endl;
   cout << "   Usage: ccpu -a | -h | [options] [interval [iterations]]" << endl;
   cout << "   Options:" << endl;
   cout << "     -a/--about            Show about information (and exit)" << endl;
   cout << "     -h/--help             Show (this) help (and exit)" << endl;
   cout << "     -d/--detailed-dump    Dump detailed cpu info (and exit)" << endl;
   cout << "     -D/--simple-dump      Dump \"simplified\" cpu layout (and exit)" << endl;
   cout << "     -s/--denote-sockets   Denote socket boundaries in stats output" << endl;
   cout << "     -m/--display-most     Display \"most\" stats" << endl;
   cout << "                           (User,Nice,Sys,Idle)+IOWait,Irq,SoftIRQ" << endl;
   cout << "     -f/--display-full     Display \"full\" stats" << endl;
   cout << "                           (\"most\")+Steal,Guest,GuestNice" << endl;
   /* STUB: Describe "full" */
   cout << "     -p/--display-speed    Display CPU per-core speeds" << endl;
   /* STUB: Not yet implemented
   cout << "     -i             Display per-CPU interrupt statistics." << endl;
   */
   cout << flush;
}



void Options::about(void)
{
   cout << "ccpu - Color CPU" << endl;
   cout << "   Version: " << VERSION_STRING << endl;
   cout << "   A tool for evaluating CPU and CPU scheduling" << endl;
   cout << "   William Favorite <wfavorite@tablespace.net>" << endl;
}
