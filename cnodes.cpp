#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>
#include <cstdio>

#include "cnodes.hpp"

Nodes::Nodes(Options &o)
{
   string line;
   string fvalue; /* Found value */

   int processor;
   int physical_id = 0;
   int core_id;
   string model_name;
   string cpu_mhz;

   CSocket *s;
   PCore *p;

   /* Initialize some values. (This is the constructor after all.) */
   max_physical_id = -1;
   cpu_count = 0;
   column_display = o.column_display;
   denote_sockets = o.denote_sockets;
   output_format = o.output_format;
   show_timestamp = o.show_timestamp;
   monochrome = o.monochrome;

   ifstream cpuinfo("/proc/cpuinfo");
   size_t i;
   
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

            fvalue = line.substr(i);
            processor = stoi(fvalue);

            /* Set defaults for other values */
            model_name = "???";
            cpu_mhz = "???";
            core_id = processor;
            
         }

         if ( 0 == line.find("physical id") )
         {
            i = line.find(':');

            i++; /* Move off the ':' char */
            
            while ( line[i] == ' ' )
               i++;

            fvalue = line.substr(i);
            physical_id = stoi(fvalue);
         }

         if ( 0 == line.find("model name") )
         {
            i = line.find(':');

            i++; /* Move off the ':' char */
            
            while ( line[i] == ' ' )
               i++;

            model_name = line.substr(i);
         }

         if ( 0 == line.find("cpu MHz") )
         {
            i = line.find(':');

            i++; /* Move off the ':' char */
            
            while ( line[i] == ' ' )
               i++;

            cpu_mhz = line.substr(i);
         }

         if ( 0 == line.find("core id") )
         {
            i = line.find(':');

            i++; /* Move off the ':' char */
            
            while ( line[i] == ' ' )
               i++;

            fvalue = line.substr(i);
            core_id = stoi(fvalue);
         }

         if ( 0 == line.size() )
         {
            /* End of a logical processor section. Add the logical */
            
            /* DEBUGGERY
               cerr << "DEBUG:" << physical_id << ":" << core_id << ":" << processor << endl;
            */

            if ( physical_id > max_physical_id )
               max_physical_id = physical_id;
            
            auto this_socket = sockets.find(physical_id);

            if ( this_socket == sockets.end() )
            {
               /* Not found. Add a socket */
               s = new CSocket(physical_id, model_name);

               sockets[physical_id] = s;
            }
            else
            {
               /* Socket was found. Use it. */
               s = this_socket->second;
            }

            /* Add a core (if it exists, the existing will be returned) */
            p = s->AddCore(core_id);

            /* Every section is a logical. Always add one. */
            p->AddLogical(processor, cpu_mhz);

            cpu_count++;
         }
         
      }
      
   } /* if ( cpuinfo.is_open() ) */

   /* Close the file */
   cpuinfo.close();
}

/* ========================================================================= */
PCore *CSocket::AddCore(int cid)
{
   PCore *c = NULL;

   if ( max_core_id < cid )
      max_core_id = cid;

   auto this_core = pcores.find(cid);

   if ( this_core == pcores.end() )
   {
      /* No core exists. Add one. */
      c = new PCore(cid);

      pcores[cid] = c;
   }
   else
   {
      /* This core already exists. Return the existing. */
      c = this_core->second;
   }

   return(c);
}

/* ========================================================================= */
LCore *PCore::AddLogical(int lid, string &mhz)
{
   /* Logical adds are always done. They are never redundant. */
   LCore *l = new LCore(lid, mhz);

   lcores.push_back(l);

   return(l);
}

/* ========================================================================= */
CSocket::CSocket(int pid, string model)
{
   physical_id = pid;
   model_name = model;

   /* We will use this for iteration. Set to an impossible negative value. */
   max_core_id = -1;

   /* Clear the map - Being pedantic */
   pcores.clear();
}

/* ========================================================================= */
PCore::PCore(int cid)
{
   core_id = cid;

   /* Clear the vector - Being pedantic */
   lcores.clear();
}

/* ========================================================================= */
int Nodes::BuildCPUList(void)
{
   int physical_id;
   int core_id;
   CSocket *s;
   PCore *c;
   LCore *l;

   olist.insert(olist.begin(), cpu_count, NULL);
   
   /* cpu_count is set in the constructor. width and socket_height could be,
      but are currently not. */
   width = 0;
   socket_height = 0;
   
   physical_id = 0;
   while ( physical_id <= max_physical_id )
   {
      s = sockets[physical_id];

      core_id = 0;
      while ( core_id <= s->max_core_id )
      {
         /* STUB: Note that this will break on a multiple socket system.
                  By starting at core_id=0 and directly accessing without
                  checking to see that it exists, then a new will be 
                  instantiated with an empty lcores list. I believe this
                  will break in a non-noticable manner, but that remains
                  to be tested on an actual multiple-socket system. */
         c = s->pcores[core_id];

         for ( vector< LCore * >::iterator li = c->lcores.begin();
               li != c->lcores.end();
               li++)
         {
            /* Logical list insert */
            llist.push_back(*li);

            /* Ordered list insert */
            l = *li; /* For the compiler (or my inability to cast away) */
            olist[l->processor] = l;
            
            if ( 0 == core_id )
               width++;
            
            if ( 0 == physical_id )
               socket_height++;

         }
         core_id++;

      }
      physical_id++;
   }

   return(0);
}

/* ========================================================================= */
void Nodes::PrintTopInfo(void)
{
   cout << "Total logical cores (threads in system): " << cpu_count << endl;
   cout << "Core width (threads per core): " << width << endl;
   cout << "Socket height (threads per socket): " << socket_height << endl;
   cout << "Socket height (cores per socket): " << (socket_height / width) << endl;
   cout << endl;
}

/* ========================================================================= */
void Nodes::PrintLayout(int level)
{
   int physical_id;
   int core_id;
   CSocket *s;
   PCore *c;
   LCore *p;

   PrintTopInfo();
   
   physical_id = 0;
   while ( physical_id <= max_physical_id )
   {
      s = sockets[physical_id];

      cout << "SOCKET " << s->physical_id;

      if ( level == PRINT_LEVEL_FULL )
      {
         cout << "   ";
         cout << s->model_name;
      }

      cout << endl;

      core_id = 0;
      while ( core_id <= s->max_core_id )
      {
         c = s->pcores[core_id];

         cout << "  CORE " << c->core_id << endl;

         for ( vector< LCore * >::iterator li = c->lcores.begin();
               li != c->lcores.end();
               li++)
         {
            p = *li;

            cout << "    LOGICAL " << p->processor;
            
            if ( level == PRINT_LEVEL_FULL )
            {
               cout << "   ";
               p->DumpSpeedInfo(); /* Will include a LF (Cache will be on different lines) */

               /* This is not conditional at this time. Detailed == cache. */
               p->DumpCacheLevels();
            }
            else
               cout << endl;
         }

         core_id++;
      }
      physical_id++;
   }

   cout << flush;
}

/* ========================================================================= */
int Nodes::PrintLLayout(void)
{
   unsigned int l = 1;
   LCore *lc;

   PrintTopInfo();

   for ( vector< LCore * >::iterator li = llist.begin();
         li != llist.end();
         li++)
   {
      lc = *li; /* Compiler complains. I give up on casting. */
      cout << "cpu" << lc->processor;
      
      if ( 0 == l % width )
         cout << endl;
      else
         cout << "   ";

      if (( 0 == l % socket_height ) && ( l != cpu_count ))
      {
         cout << endl;
      }

      l++;
   }

#ifdef LIST_ORDERED
   /* This section is really about debuggery. */
   for ( vector< LCore * >::iterator li = olist.begin();
         li != olist.end();
         li++)
   {
      lc = *li;
      cout << "cpu" << lc->processor << "\n";
   }
#endif
   cout << flush;
   
   return(0);
}

/* ========================================================================= */
int Nodes::GatherCPUStat(void)
{
   string line;

   int len;
   int i;
   int cpunum;
   rstat_t user;
   rstat_t nice;
   rstat_t system;
   rstat_t idle;
   rstat_t iowait;
   rstat_t irq;
   rstat_t softirq;
   rstat_t steal;
   rstat_t guest;
   rstat_t guest_nice;
   rstat_t bogus; /* This is to insure that we can('t) walk off the end
                     of the string, and to check if a new item has been
                     added. */

   ifstream procstat("/proc/stat");
   if ( procstat.is_open() )
   {
      while(getline(procstat, line))
      {
         if ( 0 == line.find("cpu") )
         {
            len = line.length(); /* This is size_t, but will never be
                                    > sizeof(int). */
            
            if ( line[3] != ' ' )
            {
               /* This is a (single) cpu line */

               /* Reset our values */
               cpunum = 0;
               user = 0;
               nice = 0;
               system = 0;
               idle = 0;
               iowait = 0;
               irq = 0;
               softirq = 0;
               steal = 0;
               guest = 0;
               guest_nice = 0;
               bogus = 0;

               /* All these len compares are *safe* but really unnessary
                  at least early in the collection. As we walk out the
                  line, they become more necessary because some kernels
                  will omit the later values. */
               i = 3;
               while (( line[i] != ' ' ) && ( i < len ))
               {
                  cpunum *= 10;
                  cpunum += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  user *= 10;
                  user += line[i] - '0';
                  i++;
               }
               
               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  nice *= 10;
                  nice += line[i] - '0';
                  i++;
               }
               
               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  system *= 10;
                  system += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  idle *= 10;
                  idle += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  iowait *= 10;
                  iowait += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  irq *= 10;
                  irq += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  softirq *= 10;
                  softirq += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  steal *= 10;
                  steal += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  guest *= 10;
                  guest += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  guest_nice *= 10;
                  guest_nice += line[i] - '0';
                  i++;
               }

               while (( line[++i] == ' ' ) && ( i < len )); /* Move off space */

               while (( line[i] != ' ' ) && ( i < len ))
               {
                  bogus *= 10;
                  bogus += line[i] - '0';
                  i++;
               }

               
#ifdef DEBUGGERY
               cerr << "CPU(" << cpunum << ") [" << line[i] << "] " << line << "\n";
               cerr << "  user = " << user << "\n";
               cerr << "  nice = " << nice << "\n";
               cerr << "  system = " << system << "\n";
               cerr << "  idle = " << idle << "\n";
               cerr << "  iowait = " << iowait << "\n";
               cerr << "  irq = " << irq << "\n";
               cerr << "  softirq = " << softirq << "\n";
               cerr << "  steal = " << steal << "\n";
               cerr << "  guest = " << guest << "\n";               
               cerr << "  guest_nice = " << guest_nice << "\n";
               cerr << "  bogus = " << bogus << "\n";
#endif

               
               olist[cpunum]->InsertNewRead(user,
                                            nice,
                                            system,
                                            idle,
                                            iowait,
                                            irq,
                                            softirq,
                                            steal,
                                            guest,
                                            guest_nice);

            }
            
         } 

      } /* while (getline()) */

      procstat.close();
   } /* if ( procstat.is_open() ) */




   
   /* Collect data for the (current) CPU speed */
   if (( column_display & COL_FLG_SPEED ) || ( column_display & COL_FLG_PCTSP ))
      GatherCPUSpeeds();

#ifdef COLLECT_LOCALLY
   /* I am saving this code because it *might* be advantageous to collect stats
      in one read of the file. As it stands, with the speed governor driver the
      stats can be read in short individual reads. In the worst case scenario,
      this code is repeated for EACH CPU! At this time, I am leaving this out
      based on the design concept of pushing the logic into the class responsible
      for the data. The problem is that this design does not match the Linux
      interface in all cases. */
   {
      ifstream cpuinfo("/proc/cpuinfo");
      int processor;
   
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
            
            if ( 0 == line.find("cpu MHz") )
            {
               i = line.find(':');
               
               i++; /* Move off the ':' char */
               
               while ( line[i] == ' ' )
                  i++;
               
               /* cpu_mhz = line.substr(i); */
               olist[processor]->cpu_mhz = line.substr(i);

               /* Convert - regardless */
               unsigned long cur_speed = atol(olist[processor]->cpu_mhz.c_str());
               /* Note that cur_speed is no longer a valid class member. */
               olist[processor]->cur_speed = cur_speed;
               
            }

         } /* while(getline()) */

         cpuinfo.close();
      } /* if ( cpuinfo.is_open() */
   }  /* if ( COL_FLG_SPEED ) */
#endif

   /* Collect interrupt data */
   if ( column_display & COL_FLG_IRQ )
      GatherInterrupts();

   
   return(0);
}

/* ========================================================================= */
int Nodes::ScatterCPUStat(void)
{
   unsigned int l = 1;  /* The logical core count */
   unsigned int s = 0;  /* The socket count */
   LCore *lc;

   float user;
   float nice;
   float system;
   float idle;
   float iowait;
   float irq;
   float softirq;
   float steal;
   float guest;
   float guest_nice;

   unsigned int dwidth; /* Derived width. Used to modify output. */

   /* These numbers have 3x area equivelants that are not as nice
      looking. I stick to the brighter 9x values, and stay away
      from the extended codes that may not have the same support. */
   const int RED = 91;
   const int NORMAL = 0;
   const int GREEN = 92;
   const int CYAN = 96;  /* 36 */
   const int MAGENTA = 95;
   const int YELLOW = 93; /* 33 */
   
   int color;

   if ( show_timestamp )
   {
      time_t t;
      time(&t);

      cout << ctime(&t);
   }

   
   if ( output_format == OUT_FORMAT_LOGICAL )
      dwidth = width;
   else
      dwidth = 1;

   for ( unsigned int i = 0; i < dwidth; i++ )
   {
      cout << "      User Nice  Sys Idle";
      if ( ( column_display & COL_DISP_MASK ) >= COL_DISP_MOST )
         cout << " IOwt  Irq SIrq";
      if ( ( column_display & COL_DISP_MASK ) == COL_DISP_FULL )
         cout << "  Stl  Gst  GNi";


      if ( column_display & COL_FLG_SPEED )
         cout << "   MHz";

      if ( column_display & COL_FLG_PCTSP )
         cout << " Speed";
      
      if ( column_display & COL_FLG_IRQ )
         cout << "   IrqCnt";

      cout << "   ";
      
   }
   cout << "\n";

   vector< LCore * >::iterator li;
   vector< LCore * >::iterator liend;
   if ( output_format == OUT_FORMAT_LOGICAL )
   {
      li = llist.begin();
      liend = llist.end();
   }
   else
   {
      li = olist.begin();
      liend = olist.end();
      /* Here we set the sockets specifier as false. We are changing the
         actual value, not a derived value. This is kind of bad, but... we
         checked for it elsewhere, and this *should not* be set. So this
         piece of code *should* have no effect. */
      denote_sockets = false;
   }
      

   
   while ( li != liend )
   {
      lc = *li; /* Compiler complains. I give up on casting. */

      lc->GetLastRead(user,
                      nice,
                      system,
                      idle,
                      iowait,
                      irq,
                      softirq,
                      steal,
                      guest,
                      guest_nice);

      switch ( column_display & COL_DISP_MASK )
      {  /* YES... I intentionally fall thru here. */
      case COL_DISP_MOST:
         idle += steal;       /* It is not used (by us) so considered idle. */
         user += guest;       /* Spent by virtuals, so effectively user.    */
         nice += guest_nice;  /* Like guest, but nice so roll it into nice. */
      case COL_DISP_BASE:
         system += (iowait + irq + softirq); /* All of these are system     */
      default:                               /* items (assuming softirq is  */
         break;                              /* only the signal and not     */
      }                                      /* signal processing           */

      if (( denote_sockets ) && ( 1 == l % socket_height ))
         cout << "sock" << s << "\n";

      /* This is a more busy-waited coloring scheme. It does not go yellow
         and red until we are nearly out of CPU. */
      if ( ! monochrome )
      {
         color = CYAN;
         if ( idle < 99 )
            color = GREEN;
         if ( idle < 30 )
            color = YELLOW;
         if ( idle < 15 )
            color = RED;
         if ( idle <= 1 )
         color = MAGENTA;
         
         printf("%c[%dm", 27, color); /* Apparently(?) cout does not support 
                                         ANSI code output. printf() to the 
                                         rescue. */
         fflush(stdout); /* I flush because I don't know if printf and cout 
                            use the same buffer. I assume not, so each must be
                            flushed before I can start dumping data into the
                            other. */
      }
      
      cout << fixed;
      cout << setprecision(0);

      cout << "cpu" << lc->processor << "  " << setw(3) << user << "% " << setw(3) << nice << "% " << setw(3) << system << "% " << setw(3) << idle << "%";
      if ( ( column_display & COL_DISP_MASK ) >= COL_DISP_MOST )
         cout << " " << setw(3) << iowait << "% " << setw(3) << irq << "% " << setw(3) << softirq << "%";
      if ( ( column_display & COL_DISP_MASK ) == COL_DISP_FULL )
         cout << " " << setw(3) << steal << "% " << setw(3) << guest << "% " << setw(3) << guest_nice << "%";

      /* Conditional items */
      if ( column_display & COL_FLG_SPEED )
         cout << "  " << setw(4) << lc->CurrentMHz();

      if ( column_display & COL_FLG_PCTSP )
         cout << "  " << setw(3) << lc->CurrentPctOfMaxMhz() << "%";

      if ( column_display & COL_FLG_IRQ )
         cout << "  " << setw(7) << (lc->this_interrupts - lc->last_interrupts);
      
      cout.unsetf(ios::fixed);
      cout << flush; /* See note above on fflush() */

      if ( ! monochrome )
      {
         printf("%c[%dm", 27, NORMAL);
         fflush(stdout); /* Again. The same reason for gratitious flushing. */
      }
      
      if ( 0 == l % dwidth )
         cout << "\n";
      else
         cout << "   ";

      /* STUB: This is likely an extra space if denoting sockets. */
      if ((denote_sockets) && ( 0 == l % socket_height ) && ( l != cpu_count ))
         cout << "\n";

      l++;
      li++;

   }

#ifdef LIST_ORDERED
   /* This section is really about debuggery. */
   for ( vector< LCore * >::iterator li = olist.begin();
         li != olist.end();
         li++)
   {
      lc = *li;
      cout << "cpu" << lc->processor << "\n";
   }
#endif

   cout << "\n" << flush;
   
   return(0);
}

/* ========================================================================= */
int Nodes::GatherInterrupts(void)
{
   string line;
   long llen;
   unsigned int cpu;
   int i;
   rstat_t curval;

   /* Reset the interrupt values */
   cpu = 0; 
   while ( cpu < cpu_count )
   {
      olist[cpu]->last_interrupts = olist[cpu]->this_interrupts;
      olist[cpu]->this_interrupts = 0;
      cpu++;
   }

   /* Collect the new interrupt values */
   ifstream interrupts("/proc/interrupts");
      
   if ( interrupts.is_open() )
   {
      while(getline(interrupts, line))
      {
         llen = line.length();
           
         if ( llen < 4 )
            continue;

         if ( line[3] != ':' )
            continue;

         if ( ( line[0] != ' ' ) || (( line[0] < '0' ) && (line[0] > '9' )) )
            continue;

         cpu = 0; /* Reset the CPU counter */
         i = 4;

         while ( cpu < cpu_count )
         {
            /* Walk off the space */
            while ( line[i] == ' ' )
               i++;

            curval = 0;
            while (( line[i] >= '0' ) && ( line[i] <= '9' ))
            {
               curval *= 10;
               curval += line[i] - '0';
               i++;
            }

            olist[cpu]->this_interrupts += curval;

            cpu++;
         } /* COLUMNS - while ( cpu < cpu_count ) */
      } /* ROWS - while ( getline() ) */
      interrupts.close();
   }

   return(0);
}

/* ========================================================================= */
int Nodes::GatherCPUSpeeds(void)
{
   int rv = 0;
   
   for ( auto vi : olist )
      rv += vi->GatherCurrentSpeed();

   return(rv);
}
