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
/* PCore - Physical Core
 *         Contained in (child of) no more than one socket.
 *         Contains (parent of) one or more logical cores.
 *         Tracks (what one considers) a true CPU. (A CPU may have multiple
 *         (hyper/SMT) threads and multiple instruction units (such as fp or
 *         integer... so this definition starts to get a bit *soft* in modern
 *         architectures but not so much that we can't differentiate.)
 */
class PCore
{
public:
   /* Constructor. Sets core ID. */
   PCore(int cid);

   /* Associate a logical core with the physical. The fact that the speed
      is passed is really about *where* the data comes from. (And also
      because Linux reports speeds (in /proc/cpuinfo) at the logical core
      level, not the physical. */
   LCore *AddLogical(int lid, string &mhz);

   int core_id;

   /* Note in the use of a vector here: Because the logical cores are not
      indexed in the same manner as sockets and cores. This means that we 
      cannot refer to them by an index value as the logical cores are bound
      to have "holes" in the indexing. (This is not saying much, as vectors
      were used extensively throughout the classes mapping the logical->
      physical->socket layout. But it does explain why some classes here have
      a max_<child>_id value, and this does not.) */
   vector< LCore * > lcores;
};

/* ========================================================================= */
/* CSocket - A socket
 *         Contained in (child of) the Node (a "computer").
 *         Contains (parent of) one or more physical cores.
 *         Tracks a chip in the computer - typically that you can point at.
 *         While some architecures (Power, I am looking at you) may package
 *         multiple sockets in a physical package - that plug into a socket...
 *         this is what Linux sees, and really is more about determining NUMA
 *         boundaries. [NOTE: This application reads cache (a key factor in
 *         NUMA) on the logical core level - which is really inappropriate,
 *         but it is not easy to determine what really *is* the layout from
 *         /proc and /sys filesystems alone. So - I am admitting that this
 *         is a flawed implementation - for now (and likely the lifetime of
 *         this project in its current form).]
 */
class CSocket
{
public:
   /* Constructor. Grabs key info parsed from /proc/cpuinfo. */
   CSocket(int pid, string model);

   /* Associate a core with the socket. Note that this returns a physical 
      core (PCore pointer). In a "proper RAII implementation" the instantiation
      of an object should initialize all data. BUT... because we are reading
      this info from /proc/cpuinfo, it makes sense to treat these "classes"
      more like structs - in that we create and dump info in from a parent
      function rather than letting them go out and re-parse their way through
      the same files we already have open. Unfortunately, C++ (purity) kind of
      bumps up against what would be a simple struct-data-layout problem in C.
      Why mention this? Because I am trying to be C++-ish here and resist 
      patterns learned in years of C development, but either the problem is
      truly like I say here, or I just missed the boat on recognizing a proper
      C++/OO design approach. All that said... /proc/cpuinfo (and other files
      should be read as *monolithically* as possible (meaning in one pass, into
      a buffer, where it can be parsed at will), because if we allow the file
      to be re-opened and re-read as we instantiate classes, then the values
      may skew and blow our "atomic" read - and *this* causes problems in
      our data. */
   PCore *AddCore(int cid);
   
   int physical_id; /* This is MISLEADING. It refers to a physical id while
                       we are capturing sockets that contain *physical cores*.
                       This is because I am using the string "title" from
                       /proc/cpuinfo! Do not be mislead, this is the "ID" of
                       the socket. */
   int max_core_id; /* DANGER WILL ROBINSON! The use of this value might
                       possibly go sideways on a multiple socket system.
                       Please test this at the first oppurtunity. */

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
   int BuildCPUList(void);

   /* These print information in the -D and -d (command line) options. The
      second (with the extra 'L') prints out a simple logical layout, and
      skips most of the detail info. The first takes a param of different
      levels (quantites) of info to display. Currently, it just prints
      everything that is gathered and multiple "levels" are not really
      supported. */
   void PrintLayout(int level = PRINT_LEVEL_FULL); /* -d */
   int PrintLLayout(void);                         /* -D */

   /* Common functionality used by both PrintLayout() and PrintLLayout() */
   void PrintTopInfo(void);

   /* Two key functions used by the statistical (regular) mode. */
   int GatherCPUStat(void);
   int ScatterCPUStat(void);

   /* Additional data collection for the two "add on" options */
   int GatherInterrupts(void); /* -i      */
   int GatherCPUSpeeds(void);  /* -P & -p */
   
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

   /* Passed options */
   int column_display;         /* How many columns to display (first three 
                                  bits) and other flags (remaining bits)     */
   int denote_sockets;         /* To denote socket boundaries in output      */
   int output_format;          /* ORDERED or LOGICAL layout of CPUs          */
   bool show_timestamp;        /* To show timestamps or not                  */
   bool monochrome;            /* To use monochrome output or not            */
};

#endif
