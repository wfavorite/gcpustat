#ifndef LCORES_HPP
#define LCORES_HPP

#include <string>
#include <vector>

using namespace std;

/* This is the datatype used to hold r(aw) stats */
typedef unsigned long long rstat_t;


struct LCCache /* Logical core cache */
{
   /* Just keep these as strings (rather than convert back-n-forth). */
   string level;
   string type;
   string size;

   LCCache(string &nlevel, string &ntype, string &nsize);
};

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

   int DumpCacheLevels(void);
   
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

   vector <LCCache> clevels;
};




#endif
