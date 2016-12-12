#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cstring>

#include "lcores.hpp"

/* ========================================================================= */
LCCache::LCCache(string &nlevel, string &ntype, string &nsize)
{
   level = nlevel;
   type = ntype;
   size = nsize;
}

/* ========================================================================= */
SpeedInfo::SpeedInfo(unsigned int lcore)
{
   size_t len;
   size_t i;

   this_cpu = lcore;
   is_valid = true; /* Assume true to start */
   use_sysfs = true;
   fhardmax = 1000; /* Anything that is not 0! */
   scurrent_mhz = "UNK";

   
   /* Check before trying to collect data */
   if ( ! CanGather(lcore) )
   {
      is_valid = false;
      use_sysfs = false;

      get_from_proc();
      return;
   }

   /* Now - gather from /sys */
   string basepath = "/sys/devices/system/cpu/cpu" + to_string(lcore) + "/cpufreq";

   /* Find the current driver */
   string driver_fname = basepath + "/scaling_driver";
   ifstream driver_file(driver_fname.c_str());
   if ( driver_file.is_open() )
   {
      if ( ! getline(driver_file, scaling_driver) )
      {
         scaling_driver = "UNK";
         is_valid = false;
         return;
      }
      driver_file.close();
   }

   string governor_fname = basepath + "/scaling_governor";
   ifstream governor_file(governor_fname.c_str());
   if ( governor_file.is_open() )
   {
      if ( ! getline(governor_file, scaling_governor) )
      {
         scaling_governor = "UNK";
         is_valid = false;
         return;
      }
      governor_file.close();
   }

   /* Soft */
   string smax_freq_fname = basepath + "/scaling_max_freq";
   ifstream smax_freq_file(smax_freq_fname.c_str());
   if ( smax_freq_file.is_open() )
   {
      if ( ! getline(smax_freq_file, scaling_max_freq) )
      {
         scaling_max_freq = "UNK";
         is_valid = false;
         return;
      }

      len = scaling_max_freq.size();
      i = 0;
      soft_max_mhz = 0;
      while ( i < len )
      {
         soft_max_mhz *= 10;
         soft_max_mhz += (scaling_max_freq[i] - '0');
         i++;
      }

      /* Convert from Hz to MHz */
      soft_max_mhz /= 1000;
      
      smax_freq_file.close();
   }

   /* Hard */
   string cmax_freq_fname = basepath + "/cpuinfo_max_freq";
   ifstream cmax_freq_file(cmax_freq_fname.c_str());
   if ( cmax_freq_file.is_open() )
   {
      if ( ! getline(cmax_freq_file, cpuinfo_max_freq) )
      {
         cpuinfo_max_freq = "UNK";
         is_valid = false;
         return;
      }
      
      len = cpuinfo_max_freq.size();
      i = 0;
      hard_max_mhz = 0;
      while ( i < len )
      {
         hard_max_mhz *= 10;
         hard_max_mhz += (cpuinfo_max_freq[i] - '0');
         i++;
      }

      /* Convert from Hz to MHz */
      hard_max_mhz /= 1000;
      fhardmax = hard_max_mhz; /* This is used for maths later */

      cmax_freq_file.close();
   }

   /* Grab the *current* speed */
   get_from_sys();
}

/* ========================================================================= */
int SpeedInfo::get_from_proc(void)
{
   ifstream cpuinfo("/proc/cpuinfo");
   string line;
   size_t i;
   int was_set_rv = 1;
   unsigned int processor;
   
   if ( cpuinfo.is_open() )
   {
      while(getline(cpuinfo, line))
      {
         if ( 0 == line.find("processor") )
         {
            i = line.find(':');
               
            i++; /* Move off the ':' char */
               
            while ( line[i] == ' ' )
               i++;
               
            processor = stoi(line.substr(i));
         }

         /* If we are on the wrong CPU, then skip parsing this line */
         if ( processor != this_cpu )
            continue;
         
         if ( 0 == line.find("cpu MHz") )
         {
            i = line.find(':');
               
            i++; /* Move off the ':' char */
               
            while ( line[i] == ' ' )
               i++;

            scurrent_mhz = line.substr(i);

            /* Convert - regardless */
            fcurrent_mhz = atol(scurrent_mhz.c_str());

            was_set_rv = 0;
         }

      } /* while(getline()) */

      cpuinfo.close();
   } /* if ( cpuinfo.is_open() */

   return(was_set_rv);
}

/* ========================================================================= */
int SpeedInfo::get_from_sys(void)
{
   string smf_fname = "/sys/devices/system/cpu/cpu" + to_string(this_cpu) + "/cpufreq/scaling_cur_freq";
   string line;
   size_t len;
   size_t i;
   int was_set_rv = 1;
   
   ifstream smf_file(smf_fname.c_str());
   if ( smf_file.is_open() )
   {
      if ( ! getline(smf_file, line) )
      {
         return(was_set_rv);
      }

      /* Chop the last three digits (convert from Hz to MHz) */
      len = line.size();
      if ( len > 3 )
         scurrent_mhz = line.erase(len - 3);
      else
         scurrent_mhz = line;
      
      i = 0;
      fcurrent_mhz = 0;
      while ( i < len )
      {
         fcurrent_mhz *= 10;
         fcurrent_mhz += (line[i] - '0');
         i++;
      }
      
      /* Convert from Hz to MHz */
      fcurrent_mhz /= 1000;
      
      smf_file.close();

      was_set_rv = 0;
   }

   return(was_set_rv);
}

/* ========================================================================= */
/* This function was originally created (as a static function) to check to
   see if it was appropriate to instantiate the class. When all data
   collection regardless of source was moved into the class, this became
   a moot point. */
bool SpeedInfo::CanGather(unsigned int lcore)
{
   string basepath = "/sys/devices/system/cpu/cpu" + to_string(lcore) + "/cpufreq"; 

   if ( 0 != access(basepath.c_str(), F_OK) )
   {
      /* The CPU speed scaling driver is not installed. */
      return(false);
   }

   return(true);
}


/* ========================================================================= */
int SpeedInfo::GetCurrentStat(void)
{
   if ( use_sysfs )
      return(get_from_sys());
   else
      return(get_from_proc());
}

/* ========================================================================= */
int SpeedInfo::DumpLine(void)
{
   if ( ! is_valid )
   {
      cout << "\n";
      return(1);
   }

   cout << "driver=" << scaling_driver << "; governor=" << scaling_governor << ";";
   cout << " hard=" << hard_max_mhz << "; soft=" << soft_max_mhz << "\n";

   return(0);
}

/* ========================================================================= */
float SpeedInfo::CurrentAsPct(void)
{
   if ( is_valid )
      return((fcurrent_mhz / fhardmax) * 100);

   /* We don't know. So say it is 100% */
   return(100);
}

/* ========================================================================= */
LCore::LCore(int lid, string &mhz)
{
   string line;

   /* Lay in the initializing ("local") values */
   processor = lid;
   speed = nullptr;
   backup_mhz = mhz;

   /* Initialize all the last values - Just being pedantic. */
   last_user = 0;
   last_nice = 0;
   last_system = 0;
   last_idle = 0;
   last_iowait = 0;
   last_irq = 0;
   last_softirq = 0;
   last_steal = 0;
   last_guest = 0;
   last_guest_nice = 0;
   last_interrupts = 0;

   /* Two different approaches are made to collect per-(logical-)core
      information. One is telling the sub-class what those values are
      (Which is basically why the LCCache is a struct) and to rely 
      upon the sub-class to collect the data on your behalf (which is
      why SpeedInfo is a class). Each has problems and we cannot be
      design purists here because some data comes in a single read of
      a file, and others may come in different ways (individual files).
      Neither option is really the best - one of the reasons [frankly]
      I like writing this in C. Because I will do a single read() of
      the file, and parse it out from a buffer into structs as I see
      fit. Of course, this can be done in C++, but then you are breaking
      all kinds of rules (!RAII, C-strings, pointers, etc...) that C++
      purists complain about. The good news is that the load here is
      not significant, and the usage of such items is really only in
      edge cases, and in the cache info collection - only once at
      startup. */
   
   /* Cache info is generated in LCore and placed into the LCCache struct */
   string cpudn = "/sys/devices/system/cpu/cpu" + to_string(lid) + "/cache";

   /* Moving these local - they are only used here. This is a
      characteristically un-C way of doing this. The point is to put var
      declarations next to where they are used, not at the top of a larger
      funciton. Also... this *sort-of* prevents varaible reuse which can be
      misleading for someone who may try to read my code. */
   int cache_index = 0;
   bool try_next_index = true;

   /* Don't bother if this is some odd platform (or the dir structure does
      not exist. */
   if ( 0 != access(cpudn.c_str(), F_OK | R_OK) )
      try_next_index = false;

   while ( try_next_index )
   {
      string index_dir_name = cpudn + "/index" + to_string(cache_index) + "/";
      string index_file_name = index_dir_name + "level";

      string level_string;
      string type_string;
      string size_string;
      
      ifstream level_file(index_file_name.c_str());
      if ( level_file.is_open() )
      {
         if ( ! getline(level_file, level_string) )
         {
            /* This really should never happen (open file but ! read). */
            try_next_index = false;
            continue;
         }

         level_file.close();
      }
      else
      {
         try_next_index = false;
         continue;
      }

      index_file_name = index_dir_name + "type";
      ifstream type_file(index_file_name.c_str());
      if ( type_file.is_open() )
      {
         if ( ! getline(type_file, type_string) )
         {
            /* This really should never happen (open file but ! read). */
            try_next_index = false;
            continue;
         }

         type_file.close();
      }
      else
      {
         try_next_index = false;
         continue;
      }

      index_file_name = index_dir_name + "size";
      ifstream size_file(index_file_name.c_str());
      if ( size_file.is_open() )
      {
         if ( ! getline(size_file, size_string) )
         {
            /* This really should never happen (open file but ! read). */
            try_next_index = false;
            continue;
         }

         size_file.close();
      }
      else
      {
         try_next_index = false;
         continue;
      }
     
      clevels.push_back(LCCache(level_string, type_string, size_string));
      
      cache_index++;
   }

   /* Collect speed info (in the SpeedInfo constructor) */
   speed = new SpeedInfo(processor);
}

/* ========================================================================= */
LCore::~LCore(void)
{
   /* SpeedInfo may be conditionally created. */
   if ( nullptr != speed )
      delete speed;

   /* Also of note: We really don't care about cleanup in this class. This
      class exists for the life of the process. Cleaning up as we fall out
      of scope is just about pointless. */
}

/* ========================================================================= */
int LCore::InsertNewRead(rstat_t user,
                         rstat_t nice,
                         rstat_t system,
                         rstat_t idle,
                         rstat_t iowait,
                         rstat_t irq,
                         rstat_t softirq,
                         rstat_t steal,
                         rstat_t guest,
                         rstat_t guest_nice)
{
   last_user = this_user;
   last_nice = this_nice;
   last_system = this_system;
   last_idle = this_idle;
   last_iowait = this_iowait;
   last_irq = this_irq;
   last_softirq = this_softirq;
   last_steal = this_steal;
   last_guest = this_guest;
   last_guest_nice = this_guest_nice;

   this_user = user;
   this_nice = nice;
   this_system = system;
   this_idle = idle;
   this_iowait = iowait;
   this_irq = irq;
   this_softirq = softirq;
   this_steal = steal;
   this_guest = guest;
   this_guest_nice = guest_nice;

   total = (this_user - last_user) +
      (this_nice - last_nice) +
      (this_system - last_system) +
      (this_idle - last_idle) +
      (this_iowait - last_iowait) +
      (this_irq - last_irq) +
      (this_softirq - last_softirq) +
      (this_steal - last_steal) +
      (this_guest - last_guest) +
      (this_guest_nice - last_guest_nice);

   return(0);
}

/* ========================================================================= */
int LCore::GetLastRead(float &user,
                       float &nice,
                       float &system,
                       float &idle,
                       float &iowait,
                       float &irq,
                       float &softirq,
                       float &steal,
                       float &guest,
                       float &guest_nice)
{
   float ftotal = total;
   float temp_f;
   
   temp_f = this_user - last_user;
   user = (temp_f / ftotal) * 100;

   temp_f = this_nice - last_nice;
   nice = (temp_f / ftotal) * 100;

   temp_f = this_system - last_system;
   system = (temp_f / ftotal) * 100;

   temp_f = this_idle - last_idle;
   idle = (temp_f / ftotal) * 100;

   temp_f = this_iowait - last_iowait;
   iowait = (temp_f / ftotal) * 100;

   temp_f = this_irq - last_irq;
   irq = (temp_f / ftotal) * 100;

   temp_f = this_softirq - last_softirq;
   softirq = (temp_f / ftotal) * 100;

   temp_f = this_steal - last_steal;
   steal = (temp_f / ftotal) * 100;

   temp_f = this_guest - last_guest;
   guest = (temp_f / ftotal) * 100;

   temp_f = this_guest_nice - last_guest_nice;
   guest_nice = (temp_f / ftotal) * 100;
   
   return(0);
}

/* ========================================================================= */
int LCore::DumpCacheLevels(void)
{
   int cnt = 0;

   for (auto vi : clevels )
   {
      cerr << "      CACHE level=" << vi.level << "; type=" << vi.type << "; size=" << vi.size << ";\n";
      cnt++;
   }

   return(cnt);
}

/* ========================================================================= */
int LCore::DumpSpeedInfo(void)
{
   if ( nullptr != speed )
      speed->DumpLine();

   return(0);
}

/* ========================================================================= */
string LCore::CurrentMHz(void)
{
   if ( speed )
      return(speed->CurrentAsString());

   return(backup_mhz);
}

/* ========================================================================= */
float LCore::CurrentPctOfMaxMhz(void)
{
   if ( speed )
      return(speed->CurrentAsPct());

   return(100); /* No power control - must be/assume 100% */
}
   
