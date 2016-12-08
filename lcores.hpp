#ifndef LCORES_HPP
#define LCORES_HPP

#include <string>
#include <vector>

using namespace std;

/* This is the datatype used to hold r(aw) stats */
typedef unsigned long long rstat_t;

/* ========================================================================= */
class SpeedInfo
{
public:
   SpeedInfo(unsigned int lcore);
   static bool CanGather(unsigned int lcore);
   int DumpLine(void);
   float GetMaxHardMHz(void) { if ( is_valid ) { return(hard_max_mhz); }; return(0); }

private:
   bool is_valid;
   unsigned int cpu;
   unsigned long hard_max_mhz;
   unsigned long soft_max_mhz;
   
   string scaling_driver;
   string scaling_governor;
   string cpuinfo_max_freq; /* Hz */
   string scaling_max_freq; /* Hz */
   //string scaling_cur_freq;
};

/* ========================================================================= */
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
   ~LCore();

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

   int GatherSpeedInfo(void);
   int DumpSpeedInfo(void);
   
   int processor;
   string cpu_mhz;
   SpeedInfo *speeds;
   /* Used to calc pct of max */
   float max_speed;
   float cur_speed;

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

   /* Optionally used interrupt counters. Note: There is no gettor, settor,
      or (re)settor/roller for these values. They are only set by one 
      function in the Nodes class, so it makes sense to just access them
      directly to save performance over the jumps required to call the
      various gettors/settors... (LCore is a class - it should probably
      be a struct. The gettors and settors are for convenience, and most
      values are accessed directly.) */
   rstat_t this_interrupts;
   rstat_t last_interrupts;
   
   vector <LCCache> clevels;
};




#endif
