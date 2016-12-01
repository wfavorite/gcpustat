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
   Nodes n;

   if ( o.dump )
   {
      n.PrintLayout();
      return(0);
   }

   n.BuildCPUList();
   n.PrintLLayout();

   cerr << "===========================" << endl;

   while ( 1 )
   {
      n.GatherCPUStat();

      n.ScatterCPUStat();

      cout << endl;
      sleep(2);
   }











   return(0);
};
