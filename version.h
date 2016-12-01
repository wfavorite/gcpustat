/*
  Version history:
   0.1.0   12/1/16 - Original creation (after "lost" code).

*/
#define VERSION_STRING "0.1.0"
/*
  ToDo:
   [ ] Insure that we cannot be IS_POS_ARG and HAS_xD_ARG at the same time.
   [_] Add support for ALLOW_MULT.
   [ ] OptParse::IsExclusive() allows for list members that do not exist.
   [ ] Normalize all the structure names to socket, core, logical (except
       where they clearly reference /proc/cpuinfo fields).
   [ ] The -d option should be stackable.
   [ ] assert() is not enforced on OptParse::WasFound() label mismatches.

  Done:
*/
