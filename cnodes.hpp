#ifndef CNODES_HPP
#define CNODES_HPP

#include <string>
#include <vector>
#include <map>
#include <array>

#include "options.hpp"
#include "lcores.hpp"

using namespace std;

/* ========================================================================= */
class PCore
{
public:
   PCore(int cid);
   LCore *AddLogical(int lid, string &mhz);

   int core_id;
   int max_processor;

   /* Use a vector here because the logical cores are not indexed in the
      same manner as sockets and cores. They have an "absolute" mapping.
      STUB: Yea, but what about when there is no hyperthreading? 
   */
   vector< LCore * > lcores;
};

/* ========================================================================= */
class CSocket
{
public:
   CSocket(int pid, string model);
   PCore *AddCore(int cid);
   
   int physical_id;
   int max_core_id;

   map < int, PCore *> pcores;

   /* Data derived from /proc/cpuinfo */
   string model_name;
};

/* ========================================================================= */
const int PRINT_LEVEL_MIN  = 1;
const int PRINT_LEVEL_FULL = 3;

class Nodes
{
public:
   Nodes(Options &o);
   void PrintLayout(int level = PRINT_LEVEL_FULL);
   int BuildCPUList(void);
   int PrintLLayout(void);
   void PrintTopInfo(void);
   
   int GatherCPUStat(void);
   int ScatterCPUStat(void);

   int GatherInterrupts(void);
   
   map < int, CSocket * > sockets;

   int max_physical_id;

   unsigned int cpu_count;     /* Total number of "printable" (logical) CPUs */
   unsigned int width;         /* Number of logicals per physical cpu        */
   unsigned int socket_height; /* Number of rows to print in a socket. Or more
                                  specifically: physicals in a socket.       */

                               /* Have to use a vector, because a C array is
                                  "evil" and I can't pre-declare an array in
                                  the class. The thinking was std::array would
                                  be more performant than a vector. But C++. */
   vector<LCore *> llist;      /* Logically ordered list (for printing).     */
   vector<LCore *> olist;      /* "Ordered" list for gathering stats.        */

   int column_display;
   int denote_sockets;
};

#endif
