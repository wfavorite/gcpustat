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
     are not.
*/

class ReqFiles
{
public:
   ReqFiles(vector <string> rfiles);
   bool IsGood(void);
   int DumpMissingFiles(void);
   
private:
   bool all_good;
   vector <string> missing_files;
};

#endif
   
   
