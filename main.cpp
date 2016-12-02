#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>

#include "options.hpp"
#include "cnodes.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   Options o(argc, argv);
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

   /* cerr << "===========================" << endl; */

   while ( 1 ) /* STUB: o.iterations goes here - countdown conditional */
   {
      n.GatherCPUStat(); /* STUB: Pass flags for interrupt collection */

      n.ScatterCPUStat(); /* STUB: Pass flags for output options */

      cout << endl; /* STUB: Move to Scatter */
      
      sleep(2); /* STUB: Pass interval value */
   }

   return(0);
};
