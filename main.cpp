#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>

#include "options.hpp"
#include "cnodes.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   long itcnt; /* The iteration count */
   long iterations;
   
   /* Parse command line options */
   Options o(argc, argv);

   /* Parse out all the CPU nodes */
   Nodes n(o);

   /* Required to continue (for most options) */
   n.BuildCPUList();

   /* Handle our dump options. */
   switch ( o.dump )
   {
   case DUMP_DETAILED:
      n.PrintLayout();
      return(0);
   case DUMP_SIMPLE:
      n.PrintLLayout();
      return(0);
   default:
      break;
   }

   if ( n.GatherInterrupts() )
      return(0);
   
   /* The "normal" usage scenario. */
   itcnt = 0;
   iterations = o.iterations; /* Move local ( and to signed val) */
   if ( 0 == iterations )
      itcnt = -1;
   
   while ( itcnt < iterations )
   {
      n.GatherCPUStat();

      n.ScatterCPUStat();

      cout << endl; /* STUB: Move to Scatter */

      /* Only increment this if iterations is > 0 (see code above) */
      if ( itcnt >= 0 )
         itcnt++;

      /* STUB: We sleep *after* the last iteration. This *should* be fixed. */
      sleep(o.interval);
   }

   return(0);
};
