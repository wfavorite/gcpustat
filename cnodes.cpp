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
   int physical_id;
   int core_id;
   string model_name;
   string cpu_mhz;

   CSocket *s;
   PCore *p;
   //LCore *l;

   /* Initialize some values. (This is the constructor after all.) */
   max_physical_id = -1;
   cpu_count = 0;
   column_display = o.column_display;
   denote_sockets = o.denote_sockets;


   
   ifstream cpuinfo("/proc/cpuinfo");
   size_t i;
   
   if ( cpuinfo.is_open() )
   {
      //cproc = -1;
      
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

         /* STUB: This must be set to UNK at the top of each processor */
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
            
            cerr << "DEBUG:" << physical_id << ":" << core_id << ":" << processor << endl;

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

   /* We will use this for iteration. Set to an impossible negative value. */
   max_processor = -1;
   
   /* STUB: Clear the vector - Being pedantic */
   lcores.clear();
}

LCore::LCore(int lid, string &mhz)
{
   processor = lid;
   cpu_mhz = mhz;
}

/* ========================================================================= */
void Nodes::PrintLayout(int level)
{
   int physical_id;
   int core_id;
   CSocket *s;
   PCore *c;
   LCore *p;

   
   physical_id = 0;
   while ( physical_id <= max_physical_id )
   {
      s = sockets[physical_id];

      cout << "SOCKET   " << s->physical_id;

      if ( level == PRINT_LEVEL_FULL )
      {
         cout << "     ";
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
               cout << "  ";
               cout << p->cpu_mhz;
            }

            cout << endl;
         }

         core_id++;
      }
      physical_id++;
   }

   cout << flush;
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
int Nodes::PrintLLayout(void)
{
   unsigned int l = 1;
   LCore *lc;

   cout << "Core width (threads per core): " << width << endl;
   cout << "Socket height (threads per socket): " << socket_height << endl;
   cout << "Socket height (cores per socket): " << (socket_height / width) << endl;
   
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
      cout << "cpu" << lc->processor << endl;
   }
#endif
   cout << flush;
   
   return(0);
}

/* ========================================================================= */
int Nodes::GatherCPUStat(void)
{
   string line;
   ifstream procstat("/proc/stat");

   /* STUB: Need to find and store strlen (rather than looking for NULLs */
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
               cerr << "CPU(" << cpunum << ") [" << line[i] << "] " << line << endl;
               cerr << "  user = " << user << endl;
               cerr << "  nice = " << nice << endl;
               cerr << "  system = " << system << endl;
               cerr << "  idle = " << idle << endl;
               cerr << "  iowait = " << iowait << endl;
               cerr << "  irq = " << irq << endl;
               cerr << "  softirq = " << softirq << endl;
               cerr << "  steal = " << steal << endl;
               cerr << "  guest = " << guest << endl;               
               cerr << "  guest_nice = " << guest_nice << endl;
               cerr << "  bogus = " << bogus << endl;
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

   } /* if ( procstat.is_open() ) */
   
   procstat.close();

   return(0);
}

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

   const int RED = 31;
   const int NORMAL = 0;
   const int GREEN = 32;
   const int CYAN = 36;
   const int MAGENTA = 35;
   const int YELLOW = 33;
   
   int color;
   
   for ( unsigned int i = 0; i < width; i++ )
   {
      cout << "      User Nice  Sys Idle";
      if ( column_display >= COL_DISP_MOST )
         cout << " IOwt  Irq SIrq";
      if ( column_display == COL_DISP_FULL )
         cout << "  Stl  Gst  GNi";
      cout << "   ";

   }
   cout << endl;

   
   for ( vector< LCore * >::iterator li = llist.begin();
         li != llist.end();
         li++)
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

      switch ( column_display )
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
         cout << "sock" << s << endl;


      color = CYAN;
      if ( idle < 99 )
         color = GREEN;
      if ( idle < 30 )
         color = YELLOW;
      if ( idle < 15 )
         color = RED;
      if ( idle <= 1 )
         color = MAGENTA;
      
      printf("%c[%dm", 27, color);
      fflush(stdout); /* I flush because I don't know if printf and cout use 
                         the same buffer. */
      
      cout << fixed;
      cout << setprecision(0);

      cout << "cpu" << lc->processor << "  " << setw(3) << user << "% " << setw(3) << nice << "% " << setw(3) << system << "% " << setw(3) << idle << "%";
      if ( column_display >= COL_DISP_MOST )
         cout << " " << setw(3) << iowait << "% " << setw(3) << irq << "% " << setw(3) << softirq << "%";
      if ( column_display == COL_DISP_FULL )
         cout << " " << setw(3) << steal << "% " << setw(3) << guest << "% " << setw(3) << guest_nice << "%";

      cout.unsetf(ios::fixed);
      cout << flush; /* See note above on fflush() */
      
      printf("%c[%dm", 27, NORMAL);
      fflush(stdout); /* Again. The same reason for gratitious flushing. */
      
      if ( 0 == l % width )
         cout << endl;
      else
         cout << "   ";

      /* STUB: This is likely an extra space if denoting sockets. */
      if (( denote_sockets ) && ( 0 == l % socket_height ) && ( l != cpu_count ))
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
      cout << "cpu" << lc->processor << endl;
   }
#endif
   cout << flush;
   
   return(0);
}

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



   return(0);
}
