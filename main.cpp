#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>

#include "options.hpp"
#include "cnodes.hpp"
#include "rfiles.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   long itcnt; /* The iteration count */
   long iterations;
   
   /* Parse command line options */
   Options o(argc, argv);

   /* Check for required files. FWIW: Does not fall out of scope here. */
   ReqFiles rf({"/proc/cpuinfo", "/proc/stat", "/proc/interrupts", "/sys/devices/system/cpu/cpu0/cache/"});
   if ( rf.DumpMissingFiles() )
      return(1);
   
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

   /* We are going to display, so gather a set of stats now (so our first
      iteration will not be nonsensical). */
   n.GatherCPUStat();
   
   /* The "normal" usage scenario. */
   itcnt = 0;
   iterations = o.iterations; /* Move local ( and to signed val) */
   if ( 0 == iterations )
      itcnt = -1;
   
   while ( itcnt < iterations )
   {
      sleep(o.interval);

      n.GatherCPUStat();

      n.ScatterCPUStat();

      /* Only increment this if iterations is > 0 (see code above) */
      if ( itcnt >= 0 )
         itcnt++;
   }

   return(0);
};
