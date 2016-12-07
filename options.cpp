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
   output_format = OUT_FORMAT_DEFAULT;
   show_timestamp = false;
   monochrome = false;
   
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

   op.RegisterOption("display irq", 'i');
   op.RegisterOption("display irq", "display-irq");

   op.RegisterOption("output ordered", 'o');
   op.RegisterOption("output ordered", "output-ordered");

   op.RegisterOption("timestamp", 't');
   op.RegisterOption("timestamp", "timestamp");

   op.RegisterOption("monochrome", 'M');
   op.RegisterOption("monochrome", "monochrome");

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

      if ( op.WasFound("display speed") )
         cerr << "Display speed : ";
      else
         cerr << "Display speed : un";
      cerr << "set" << endl;

      if ( op.WasFound("display irq") )
         cerr << "Display irq   : ";
      else
         cerr << "Display irq   : un";
      cerr << "set" << endl;

      if ( op.WasFound("output ordered") )
         cerr << "CPUs -ordered : ";
      else
         cerr << "CPUs -ordered : un";
      cerr << "set" << endl;

      if ( op.WasFound("timestamp") )
         cerr << "Show timestamp: ";
      else
         cerr << "Show timestamp: un";
      cerr << "set" << endl;

      if ( op.WasFound("monochrome") )
         cerr << "Monochrome    : ";
      else
         cerr << "Monochrome    : un";
      cerr << "set" << endl;


   } /* End of debuggery output */

   /* The full list
      - To be used for the exclusivity checks.
      - Don't forget to add new on feature additions!
      - A third bullet point

   "interval", "simple dump", "detailed dump", "help", "about", "denote sockets", "display most", "display full", "display speed", "display irq", "output ordered", "timestamp", "monochrome"

   */

   /* Intentionally exclude help and about from both lists. We want the user to "gravitate" towards -h, allowing -a as a "warning. */
   if ( ! op.IsExclusive("about", {"interval", "simple dump", "detailed dump", "denote sockets", "display most", "display full", "display speed", "display irq", "output ordered", "timestamp", "monochrome"}) )
   {
      cerr << "ERROR: The -a option is mutually exclusive of all other options." << endl;
      exit(1);
   }

   if ( ! op.IsExclusive("help", {"interval", "simple dump", "detailed dump", "denote sockets", "display most", "display full", "display speed", "display irq", "output ordered", "timestamp", "monochrome"}) )
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

  if ( ! op.IsExclusive("denote sockets", { "output ordered" }) )
  {
     cerr << "ERROR: The -s (show sockets) and -o (ordered output) are mutually exclusive." << endl;
     exit(1);
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

  if ( op.WasFound("display speed") )
     column_display |= COL_FLG_SPEED;

  if ( op.WasFound("display irq") )
     column_display |= COL_FLG_IRQ;

  if ( op.WasFound("output ordered") )
     output_format = OUT_FORMAT_ORDERED;

  if ( op.WasFound("timestamp") )
     show_timestamp = true;

  if ( op.WasFound("monochrome") )
     monochrome = true;

  op.GetState("interval", interval);
  op.GetState("iterations", iterations);
};

/* ========================================================================= */
void Options::help(void)
{
   cout << "gcpu - Graphical CPU Statistics" << endl;
   cout << "   Usage: gcpu -a | -h | [options] [interval [iterations]]" << endl;
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
   cout << "     -p/--display-speed    Display CPU per-core speeds" << endl;
   cout << "     -i/--display-irq      Display per-CPU interrupt statistics" << endl;
   cout << "     -o/--output-ordered   Display CPU stats in ordered, not logical, layout" << endl;
   cout << "     -t/--timestamp        Show timestamp of each stats iteration" << endl;
   cout << "     -M/--monochrome       Display stats monochrome / without colour" << endl;
   cout << flush;
}

/* ========================================================================= */
void Options::about(void)
{
   cout << "gcpu - Graphical CPU Stat" << endl;
   cout << "   Version: " << VERSION_STRING << endl;
   cout << "   A tool for evaluating CPU and CPU scheduling" << endl;
   cout << "    Ethan Allen <eallen@reason.vt>" << endl;
   cout << "    William Favorite <wfavorite@tablespace.net>" << endl;
   cout << flush;
}
