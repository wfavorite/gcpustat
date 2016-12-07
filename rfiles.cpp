#include <unistd.h>

#include "rfiles.hpp"

/* ========================================================================= */
ReqFiles::ReqFiles(vector <string> rfiles)
{
   /* Assume all_good to start */
   all_good = true;
   
   for ( auto vi : rfiles )
   {
      if ( 0 != access( vi.c_str(), F_OK ) )
      {
         all_good = false;
         missing_files.push_back(vi);
      }
   }
}

/* ========================================================================= */
bool ReqFiles::IsGood(void)
{
   return(all_good);
}

/* ========================================================================= */
int ReqFiles::DumpMissingFiles(void)
{
   if ( all_good )
      return(0);

   cerr << "ERROR: The following required files are missing:\n";
   
   for ( auto vi : missing_files )
   {
      cerr << "  " << vi << "\n";
   }

   return(1);
}
