/*
  Version history:
   0.1.0   12/1/16 - Original creation.
   0.2.0   12/2/16 - Forward movement on features and cleanup.
                   - Added comments and todos.
                   - Fixed bug with bad move in OptParse::IsExclusive()
                   - Cleanup work on parsing options.
   0.3.0   12/5/16 - Created cache info - per-logical core
                   - Added MHz parsing and output.
                   - Changed colors to more recognizable palate, and made
                     them brighter.
   0.4.0   12/6/16 - Added interrupt support to output statistics.

*/
#define VERSION_STRING "0.4.0"
/*
  ToDo:
   [ ] The first iteration of stats should be collected *before* the main
       loop. My idea: Collect, wait one iteration, begin loop.
   [ ] Vague, but true: "More items" should be parsed from /proc/cpuinfo.
   [ ] Validate socket parsing on a multi-socket system.
   [ ] Insure that we cannot be IS_POS_ARG and HAS_xD_ARG at the same time.
   [_] Add support for ALLOW_MULT.
   [ ] Normalize all the structure names to socket, core, logical (except
       where they clearly reference /proc/cpuinfo fields).

  Done:
   [X] Add the -i option and supporting code.
   [X] The last_interrupts values in the LCore class needs to be initialized.
   [X] There should be a total count of physical cores in the dump pages.
       The individual per-something lines are insufficient.
   [X] MHz output should be conditional. (Uses -p switch)
   [X] OptParse::IsExclusive() allows for list members that do not exist.
   [X] Utilize interval and iterations values.
   [X] assert() is not enforced on OptParse::WasFound() label mismatches.
   [X] The -d option should be stackable. (Resolved by using -D and -d.)
   [X] Overflow issue on first iteration on long-running system. (Changed core
       data type to unsigned long long. This is a typedef in cnodes.hpp.)
*/
