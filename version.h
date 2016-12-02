/*
  Version history:
   0.1.0   12/1/16 - Original creation.
   0.2.0   12/2/16 - Forward movement on features and cleanup.
                   - Added comments and todos.

*/
#define VERSION_STRING "0.2.0"
/*
  ToDo:
   [ ] Add the -i option and supporting code.
   [ ] Utilize interval and iterations values.
   [ ] Vague, but true: "More items" should be parsed from /proc/cpuinfo.
   [ ] Validate socket parsing on a multi-socket system.
   [ ] Overflow issue on first iteration on long-running system. (Changed core
       data type to unsigned long long. This is a typedef in cnodes.hpp.)
   [ ] Insure that we cannot be IS_POS_ARG and HAS_xD_ARG at the same time.
   [_] Add support for ALLOW_MULT.
   [ ] OptParse::IsExclusive() allows for list members that do not exist.
   [ ] Normalize all the structure names to socket, core, logical (except
       where they clearly reference /proc/cpuinfo fields).
   [ ] assert() is not enforced on OptParse::WasFound() label mismatches.

  Done:
   [X] The -d option should be stackable. (Resolved by using -D and -d.)
*/
