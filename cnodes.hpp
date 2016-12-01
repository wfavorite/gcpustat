#ifndef CNODES_HPP
#define CNODES_HPP

#include <string>
#include <vector>
#include <map>
#include <array>

using namespace std;

typedef unsigned long rstat_t;
/* ========================================================================= */
class LCore
{
public:
   LCore(int lid, string &mhz);

   int InsertNewRead(rstat_t this_user,
                     rstat_t this_nice,
                     rstat_t this_system,
                     rstat_t this_idle,
                     rstat_t this_iowait,
                     rstat_t this_irq,
                     rstat_t this_softirq,
                     rstat_t this_steal,
                     rstat_t this_guest,
                     rstat_t this_guest_nice);

   int GetLastRead(float &user,
                   float &nice,
                   float &system,
                   float &idle,
                   float &iowait,
                   float &irq,
                   float &softirq,
                   float &steal,
                   float &guest,
                   float &guest_nice);

   int processor;
   string cpu_mhz;

   rstat_t this_user;
   rstat_t this_nice;
   rstat_t this_system;
   rstat_t this_idle;
   rstat_t this_iowait;
   rstat_t this_irq;
   rstat_t this_softirq;
   rstat_t this_steal;
   rstat_t this_guest;
   rstat_t this_guest_nice;

   rstat_t last_user;
   rstat_t last_nice;
   rstat_t last_system;
   rstat_t last_idle;
   rstat_t last_iowait;
   rstat_t last_irq;
   rstat_t last_softirq;
   rstat_t last_steal;
   rstat_t last_guest;
   rstat_t last_guest_nice;

   rstat_t total;
};

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
   Nodes();
   void PrintLayout(int level = PRINT_LEVEL_FULL);
   int BuildCPUList(void);
   int PrintLLayout(void);

   int GatherCPUStat(void);
   int ScatterCPUStat(void);
   
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
};


#endif
