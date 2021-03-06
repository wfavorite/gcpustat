#ifndef RFILES_HPP
#define RFILES_HPP

#include <string>
#include <vector>
#include <iostream>

using namespace std;

/* Why make this pointless class?
   - It is a fairly pointless exercise but intended as a reusable class that
     can be called easily.

   What else?
   - Files with a trailing / should be tested as directories. They currently
     are not. They are (now) labeled as such, but still tested with access()
     and not stat(). This will likely be fixed when it is actually required.
*/
const int PATH_IS_ERROR = -1;
const int PATH_IS_FILE  =  0;
const int PATH_IS_DIR   =  1;

class ReqFiles
{
public:
   ReqFiles(vector <string> rfiles);
   bool IsGood(void);
   int DumpMissingFiles(void);
   
private:
   int path_is_a(string path);
   bool all_good;
   vector <string> missing_files;
};

#endif
   
   
