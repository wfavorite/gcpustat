#include <unistd.h>
#include <cassert>

#include "rfiles.hpp"

/* ========================================================================= */
ReqFiles::ReqFiles(vector <string> rfiles)
{
   /* Assume all_good to start */
   all_good = true;
   
   for ( auto vi : rfiles )
   {
      if ( PATH_IS_ERROR == path_is_a(vi) )
      {
         cerr << "ERROR: Invalid string passed as a path." << endl;
         assert(0);
         exit(1);
      }
      
      if ( 0 != access( vi.c_str(), F_OK | R_OK ) )
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

   cerr << "ERROR: The following required dirs/files are missing:\n";
   
   for ( auto vi : missing_files )
   {
      cerr << "  ";
      
      switch ( path_is_a(vi) )
      {
      case PATH_IS_DIR:
         cerr << "DIR : ";
         break;
      case PATH_IS_FILE:
         cerr << "FILE: ";
         break;
      }
            
      cerr << vi << "\n";
   }

   return(1);
}

/* ========================================================================= */
int ReqFiles::path_is_a(string path)
{
   size_t len = path.size();

   if ( len == 0 )
      return(PATH_IS_ERROR);
   
   if ( path[len - 1] == '/' )
      return(PATH_IS_DIR);

   return(PATH_IS_FILE);
}
