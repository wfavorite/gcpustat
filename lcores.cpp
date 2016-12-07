#include <iostream>
#include <fstream>
#include <string>

#include "lcores.hpp"

/* ========================================================================= */
LCCache::LCCache(string &nlevel, string &ntype, string &nsize)
{
   level = nlevel;
   type = ntype;
   size = nsize;
}

/* ========================================================================= */
LCore::LCore(int lid, string &mhz)
{
   string line;

   /* Lay in the initializing ("local") values */
   processor = lid;
   cpu_mhz = mhz;

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

   
   string cpudn = "/sys/devices/system/cpu/cpu" + to_string(lid) + "/cache/index";
   
   /* Moving these local - they are only used here. This is a
      characteristically un-C way of doing this. The point is to put var
      declarations next to where they are used, not at the top of a larger
      funciton. Also... this *sort-of* prevents varaible reuse which can be
      misleading for someone who may try to read my code. */
   int cache_index = 0;
   bool try_next_index = true;
   while ( try_next_index )
   {
      string index_dir_name = cpudn + to_string(cache_index) + "/";
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
   
   /* STUB: Consider doing this in integer math? */
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
      cerr << "      CACHE level=" << vi.level << "; type=" << vi.type << "; size=" << vi.size << ";" << endl;
      cnt++;
   }

   return(cnt);
}

