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
                   - Cleanup of options.Xpp, fix help, about, and comments.
                   - $TUB removal, documentation, cleanup.
   0.5.0   12/7/16 - Updated README.md documentation.
                   - Added todos.
                   - Added support for -o(rdered output).
                   - Added support for -t(imestamp).
                   - Added support for -M(onochrome).
                   - Fixed bug/omission of "most" and "full" stats mode.
                   - Added support for testing for required files.
                   - Some code cleanup.
   0.6.0   12/8/16 - Minor upgrades to file/dir tests.
                   - Added the max speed output (does not handle exceptions)
   0.7.0  12/12/16 - Port to Raspberry Pi (no cache or cpu speed info).
                   - Resulting clean-up in lcore handing of results.
*/
#define VERSION_STRING "0.7.0"
/*
  ToDo:
   [ ] cnodes.hpp needs some documentation for the class definitions and
       members.
   [ ] Vague, but true: "More items" should be parsed from /proc/cpuinfo.
   [ ] Validate socket parsing on a multi-socket system.

  ToDo (options parsing library):
   [ ] Insure that we cannot be IS_POS_ARG and HAS_xD_ARG at the same time.
   [_] Add support for ALLOW_MULT.
   [ ] Normalize all the structure names to socket, core, logical (except
       where they clearly reference /proc/cpuinfo fields).

  Done:
   [X] Not all CPUs have the max speed in the string identifier. The max speed
       should be parsed during initialization and printed in the -d mode, and
       perhaps be used (optionally) as a % of max in the output.
   [X] Move all stat collection into the appropriate classes. One class should
       not collect data on the behalf of another. That was a poor design...
       or at least it *grew into* a poor design.
   [X] Probe for required files on startup and report if something is amiss.
   [X] The spacing between "SOCKET" and the socket number in -d mode is wrong.
   [X] The GuestNice value goes extremely negative in -f(ull stats) mode.
       (Some idiot just skipped that code in GetLastRead()!)
   [X] A monochrome mode and timestamp are required if this will be sent to
       a file. They can be implemented as a single item, or as two independent
       options.
   [X] Consider adding support for -n(umerical ordering) that dumps in a
       simple single column format (in numerical order). (Used -o(rdered))
   [X] The first iteration of stats should be collected *before* the main
       loop. My idea: Collect, wait one iteration, begin loop.
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
