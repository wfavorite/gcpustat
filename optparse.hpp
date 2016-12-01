#ifndef OPTPARSE_HPP
#define OPTPARSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

const unsigned long NOFEATURES =   0;
const unsigned long IS_POS_ARG =   1;
const unsigned long HAS_SD_ARG =   2;
const unsigned long HAS_DD_ARG =   4;
const unsigned long TYPE_O_ARG = IS_POS_ARG | HAS_SD_ARG | HAS_DD_ARG;
const unsigned long READ_A_STR =   8;
const unsigned long READ_A_INT =  16;
const unsigned long READ_A_LNG =  32;
const unsigned long READ_AULNG =  64;
const unsigned long READ_ACHAR = 128;
const unsigned long READ_A_VAL = READ_A_STR | READ_A_INT | READ_A_LNG | READ_AULNG |  READ_ACHAR;
const unsigned long ALLOW_MULT = 512; /* STUB: Not supported at this time */


/* ========================================================================= */
/* NOTE: This could be templated... But then that makes differentating the
         input somewhat difficult. We need to know if a proper int was parsed
         vs a string. Consider re-implementing this as a templated struct.

         template <typename OIT>
         struct OptItem
         {
         ...
         OIT value;
         ...
         };

         If that is the case... then everything might as well be saved as a
         string, and then checked later. Or... the checking would need to go
         to the OptParse class.

         Furthermore... templates are a compile time concept. The types are
         not known until runtime. This means that we need to cast or have
         lots of type references in the code. I considered inheriting a
         templated class to hold the type... The answer (if this really
         wants to be "solved" is to implement a union or C++17 variant).

         Basically... I am not sure that templates are the answer here.
         Implementing this as a templated class does not avoid the code
         required to parse and check each type. It just makes it more complex
         as you have to jump through hoops to figure out what type is used.
         Then handling all the different types at runtime means a lot of
         code is required to manage that. So... just overload.

   Notes (other):
    - This class does not support move semantics. It should never be copied.
      You create it once and let it fall out of scope - because we only care
      about the parsed values... not the class/code used to parse them.
    - Furthermore... This should be wrapped by an Options class that hides
      all the noise from main(). The code in main() should look like:
       #include "options.h"

       main(int argc, char *argv[])
       {
          Options o(argc, argv); <------ This handles all parsing in the
                                         constructor (including help() and
                                         calling exit() on errors.

          DoMyFileThing(o.filename); <-- Just access members set in the
                                         constructor that did the parsing.
          ...
       }
      Note that Options is not a "generic" class... it is code very specific
      to your command line needs. It just hides (obfsucates) it all away
      from main().
    - Why did you not use someone else's library?
      Because this is an exercise in writing my own, not the ability to
      integrate some other library code. I am sure Boost does it better,
      I just wanted to try it myself. It is a matter of exercise and
      design effort. That is all.
*/

/* ========================================================================= */
/* Struct: OptItem
 * Notes:
 *  This is a data-item struct that holds each of the *expected* options. Once
 *  the data is parsed, it holds the parsed status/values. It is used only
 *  by the OptParse class.
 */
struct OptItem
{
   string label;       /* The label of the item (how we refer to it)         */
   unsigned long features;

   char sdopt;
   string ddopt;
   int position;

   unsigned long parsedf;   /* Features that have been parsed                */
   int found;
   
   /* Each of the types */
   string         str_value;
   int            int_value;
   long           lng_value;
   unsigned long  ulng_value;
   char           char_value;
   
   OptItem(string newlabel);
   int ParseItem(char *arg);
};


/* ========================================================================= */
/* Class: OptParse
 * Notes:
 *  The class works by registering options such as:
 *   op.RegisterOption("help", 'h');  <------ Registers -h
 *   op.RegisterOption("help", "help"); <---- Registers --help
 *  When Resolve() is called, the class can be queried to see if either of
 *  the two help flag types were specified by the user.
 *
 *  The option argument values can be specified using the optional third
 *  paramater to RegisterOption(). For example:
 *   op.RegisterOption("cfgfile", 'f', READ_A_STR); <--- -f <filename>
 *    -or-
 *   op.RegisterOption("filename", 1, READ_A_STR); <---- Positional arg #1
 *  Again, when Resolve() is called, these rules will be used to parse what
 *  the user supplied.
 *
 *  But why two steps? Because a ruleset must be established before the options
 *  can be parsed. For example:
 *   -fx
 *  can be -f with an arg of x, or -f and -x, which are two different things.
 *  Another example... Pass a value of -1 to the -o(ffset) option! ==> -o -1
 *  Without the ruleset in place, this is unparsable.
 */
class OptParse
{
public:
   /* ----------------------------------------------------------------------
    * Name: OptParse
    * Description: The constructors for the class
    * Params: One takes the argc & argv params from main(), the other takes
    *         no params. Whichever one is called, the opposite param version
    *         of Resolve() should be called.
    * Notes: Does basic initialization and saves the argc/argv values that
    *        will be used in the Resolve() function(s).
    */
   OptParse(int argc, char *argv[]);
   OptParse(void);
   
   /* ----------------------------------------------------------------------
    * Name: Resolve
    * Description: This resolves argv against the registered options
    * Params: Like the constructor, call the opposite (one of the two, the
    *         constructor, or the Resolv()er needs to have argc and argv
    *         passed.)
    * Notes: 
    */
   int Resolve(int argc, char *argv[]);
   int Resolve(void);

   /* ----------------------------------------------------------------------
    * Name: RegisterOption
    * Description: Register a command line option
    * Params: Three forms (for the 2nd param):
    *          1. string - Register an --option         ==> myprog --help
    *          2. char   - Register an -o(ption)        ==> myprog -h
    *          3. int    - Register a positional option ==> myprog afile
    *         The three arguments are:
    *          1. The label used to identify the option.
    *          2. One of the three forms listed just above.
    *          3. What kind of arg to read for the option.
    * Notes:
    *     - To register two forms (such as -h and --help) call RegisterOption()
    *       twice, once with each form. Do it thusly:
    *        RegisterOption("help", "help");
    *        RegisterOption("help", 'h');
    *     - To register two forms that require args, you only need to specify
    *       the argument on one of the registrations. Do it thusly:
    *        RegisterOption("config file", "config-file", READ_A_STR);
    *        RegisterOption("config file", 'c');
    */
   int RegisterOption(string olabel, string nddopt, unsigned long argtype = NOFEATURES);
   int RegisterOption(string olabel, char nsdopt, unsigned long argtype = NOFEATURES);
   int RegisterOption(string olabel, int pos, unsigned long argtype = READ_A_STR);

   /* ----------------------------------------------------------------------
    * Name: GetValue
    * Description: Gets the (optional) value of the command line option
    * Params: The label, by-reference the value to be filled in
    * Notes: Overloaded on the type you seek.
    */
   int GetValue(string olabel, string & val);
   int GetValue(string olabel, int & val);
   int GetValue(string olabel, long & val);
   int GetValue(string olabel, unsigned long & val);
   int GetValue(string olabel, char & val);

   /* ----------------------------------------------------------------------
    * Name: WasFound
    * Description: Returns true if the label was used on the command line
    * Params: The label we seek
    * Returns: true if option was found on command line, false if not
    * Notes: No reference is made for the option if it was set or not.
    *        (If it was a positional argument, then WasFound tells if it
    *        was set (because that is the only way to set the item).)
    */
   bool WasFound(string olabel);

   /* ----------------------------------------------------------------------
    * Name: GetState
    * Description: Single function to return state and set argument 
    * Params: Overloaded on final param. Params are:
    *          1. Label of the option
    *          2. Value (option arg) to return (if set)
    * Returns: Number of flags encountered (ie: -vvv = 3). -1 on error.
    * Notes: It is not possible to use references and default params. So...
    *        The final argument MUST be passed I could have used a pointer
    *        type, but opted for a reference. Because. Well, actually, if
    *        you don't like the calling convention, then use the GetValue()
    *        and or WasFound() functions instead.
    *
    *        The code insures that a required param is set. So -1 is not
    *        a possible return value.
    *
    *        Again... templating at this level does not really seem to work.
    *        Although, the compiler did let me assign different types to the
    *        templated type, but the linker did not like the idea.
    */
   int GetState(string olabel, string &val);
   int GetState(string olabel, int &val);
   int GetState(string olabel, long &val);
   int GetState(string olabel, unsigned long &val);
   int GetState(string olabel, char &val);
   
   /* ----------------------------------------------------------------------
    * Name: DumpErrors
    * Description: Prints all error messages encountered when parsing to stderr
    * Params: None
    * Returns: The number of errors printed
    * Notes: 
    */
   int DumpErrors(void);

   /* ----------------------------------------------------------------------
    * Name: IsExclusive
    * Description: Check to see if an option is used exclusively of others
    * Params: The target label, an vector of labels that are exclusive of
    *         the target label.
    * Returns: true if exclusive, false if not exclusive.
    * Notes: 
    *    !!!IMPORTANT!!! - This is a C++11 option! It is 11 because of the
    *    vector iterator I use in the function. So that can be "fixed" if
    *    necessary. But also because I expect that you will use an "anonymous
    *    vector for the second param. Use it thusly:
    *
    *      if ( ! IsExclusive("exclopt", {"opta", "optb", "optc"}) )
    *      {
    *         cerr << "ERROR: -E is mutually exclusive of -a, -b, or -c." << endl;
    *         ...
    *      }
    *
    *    Will assert() if olabel is in the vector. But only if olabel is set!
    *    For example:
    *     IsExclusive("a", {"a", "b", "c"})
    *
    *     ./myprog -a   <---------- Will assert()
    *     ./myprog -b   <---------- Will NOT assert()
    *
    *    The exclusive check is ONLY if the items are found/used together
    *    on the command line. Duh! Just had to be clear about that.
    */
   bool IsExclusive(string olabel, vector<string> ev);

   /* ----------------------------------------------------------------------
    * Name: arg_zero
    * Description: The single member exposed to the library user: argv[0].
    * Notes: Perhaps it should be ArgZero.
    */
   string arg_zero;
   
private:
   OptItem find_opt_item(string olabel); /* Works or asserts                 */

   bool optitem_exists(string olabel); /* Used to find if an OptItem
                                          exists or not.                     */
   int resolve_args(void); /* The actual funciton that Resolve() calls.      */
   
   map<string, OptItem> optlist; /* The list of expected options.            */
   vector<string> errmsgs; /* A list of encountered parsing errors.          */

   int arg_count; /* Local storage for argc.                                 */
   char **arg_vector; /* Local storage for argv.                             */
};

#endif
