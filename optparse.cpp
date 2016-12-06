
/* Why I assert() in this library:
   - All the assert errors are library usage errors. The most notable is the 
     calling of the API with wrong or insufficient params. For example:
       OptParse op();
       op.RegisterOption(...);
       op.Resolve(); <----------- argc & argv were never set!
     Another would be misspellings of labels. These would be compile-time
     errors if the label was an enum-ish type that the compiler would catch.
     I used string labels as they can be used in the error messages (if
     necessary - ie: both single dash and double dash items are used or
     the name of the positional paramater).
   - It is more appropriate to flag an immediate runtime error on assertion 
     that the developer will see (if it is run even once), than for the 
     developer to check return values for each of the called functions.
*/
#include <cassert>

/* See notes on templating later in GetValue().
   This requires C++11
#include <type_traits>
*/

#include "optparse.hpp"

/* ========================================================================= */
OptItem::OptItem(string newlabel)
{
   label = newlabel;
   features = NOFEATURES;
   parsedf = NOFEATURES;
   found = 0;

   sdopt = 0;
   ddopt = "";
   position = 0;
};

/* ========================================================================= */
/* A few comments on this funciton...
   1. It could read hex. I just did not bother to write that capability.
   2. atoi() & atol() could do this. I wanted to check for invalid chars.
   3. Datatype overflows are not caught.
   4. Summary: Not a great implemenation.
*/
int OptItem::ParseItem(char *arg)
{
   int i;

   if ( NULL == arg )
      return(1);

   if ( arg[0] == 0 )
      return(1);

   /* ==================== */
   if ( features & READ_A_STR )
   {
      str_value = string(arg);

      parsedf |= READ_A_STR;

      return(0);
   }

   /* ==================== */
   if ( features & READ_A_INT )
   {
      int sign = 1;

      int_value = 0;
      
      i = 0;

      if ( arg[i] == '-' )
      {
         sign = -1;
         i++;
      }

      while ( arg[i] != 0 )
      {
         if (( arg[i] >= '0' ) && ( arg[i] <= '9' ))
         {
            int_value *= 10;
            int_value += arg[i] - '0';
         }
         else
            return(1);

         i++;
      }

      int_value *= sign;

      parsedf |= READ_A_INT;

      return(0);
   }

   /* ==================== */
   if ( features & READ_A_LNG )
   {
      long sign = 1;

      lng_value = 0;
      
      i = 0;

      if ( arg[i] == '-' )
      {
         sign = -1;
         i++;
      }

      while ( arg[i] != 0 )
      {
         if (( arg[i] >= '0' ) && ( arg[i] <= '9' ))
         {
            lng_value *= 10;
            lng_value += arg[i] - '0';
         }
         else
            return(1);

         i++;
      }

      lng_value *= sign;

      parsedf |= READ_A_LNG;

      return(0);
   }

   /* ==================== */
   if ( features & READ_AULNG )
   {
      ulng_value = 0;
      
      i = 0;

      if ( arg[i] == '-' )
         return(1);

      while ( arg[i] != 0 )
      {
         if (( arg[i] >= '0' ) && ( arg[i] <= '9' ))
         {
            ulng_value *= 10;
            ulng_value += arg[i] - '0';
         }
         else
            return(1);

         i++;
      }

      parsedf |= READ_AULNG;

      return(0);
   }

   /* ==================== */
   if ( features & READ_ACHAR )
   {
      char_value = arg[0];

      parsedf |= READ_ACHAR;

      return(0);
   }

   return(0);
}

/* ========================================================================= */
OptParse::OptParse(int argc, char *argv[])
{
   /* Basic initialization (could be functionized) */
   arg_zero = "";

   /* Specific initialization */
   arg_count = argc;
   arg_vector = argv;

}

/* ========================================================================= */
OptParse::OptParse(void)
{
   /* Basic initialization (could be functionized) */
   arg_zero = "";

   /* Specific initialization */
   arg_count = 0;
   arg_vector = NULL;

}

/* ========================================================================= */
bool OptParse::optitem_exists(string olabel)
{
   if ( optlist.find(olabel) == optlist.end() )
      return(false);

   return(true);
}

/* ========================================================================= */
int OptParse::RegisterOption(string olabel, string nddopt, unsigned long argtype)
{
   if ( ! optitem_exists(olabel) )
   {
      /* Technically... the optlist pair is created before the OptItem(string)
         constructor is called. FOR THIS REASON there needs to be an empty
         constructor (OptItem(void)) that is called before OptItem(string). */
      //optlist[olabel] = OptItem(olabel); // <--- Will fail without OptItem()
      
      /* This is the alternative method that does not require the OptItem(void)
         constructor. */
      optlist.insert(pair<string, OptItem>(olabel, OptItem(olabel)));
   }

   /* See note above about empty constructors. Because one does not
      exist (Don't want one), I use an iterator instead of direct access. */
   map<string, OptItem>::iterator oif = optlist.find(olabel);
   if ( oif != optlist.end() )
   {
      oif->second.ddopt = nddopt;

      /* Look for conflicting types/features. */
      if (( oif->second.features & READ_A_VAL ) && ( argtype != NOFEATURES ))
      {
         /* Something is set - Now check it */
         assert( (oif->second.features & READ_A_VAL) == argtype );
      }
      
      oif->second.features |= argtype;
   }
         
   return(0);
}

int OptParse::RegisterOption(string olabel, char nsdopt, unsigned long argtype)
{
   if ( ! optitem_exists(olabel) )
   {
      optlist.insert(pair<string, OptItem>(olabel, OptItem(olabel)));
   }

   map<string, OptItem>::iterator oif = optlist.find(olabel);
   if ( oif != optlist.end() )
   {
      oif->second.sdopt = nsdopt;

      /* Look for conflicting types/features. */
      if (( oif->second.features & READ_A_VAL ) && ( argtype != NOFEATURES ))
      {
         /* Something is set - Now check it */
         assert( (oif->second.features & READ_A_VAL) == argtype );
      }

      oif->second.features |= argtype;
   }
         
   return(0);
}

int OptParse::RegisterOption(string olabel, int pos, unsigned long argtype)
{
   /* The argtype is REQUIRED here */
   assert( argtype & READ_A_VAL );
   /* None of the other flags should be set */
   assert( 0 == (argtype & TYPE_O_ARG) );
   assert( 0 == (argtype & ALLOW_MULT) );
   
   if ( ! optitem_exists(olabel) )
   {
      optlist.insert(pair<string, OptItem>(olabel, OptItem(olabel)));
   }

   map<string, OptItem>::iterator oif = optlist.find(olabel);
   if ( oif != optlist.end() )
   {
      oif->second.position = pos;

      /* Look for conflicting types/features. */
      if (( oif->second.features & READ_A_VAL ) && ( argtype != NOFEATURES ))
      {
         /* Something is set - Now check it */
         assert( (oif->second.features & READ_A_VAL) == argtype );
      }
      
      oif->second.features |= argtype;
   }
         
   return(0);
}

/* ========================================================================= */
int OptParse::resolve_args(void)
{
   int i, j;                    /* iterators for the arg_vector */
   bool resolved;               /* Was the argv[i] resolved? */
   OptItem *parse_next = NULL;
   int position = 1;            /* The positional argument */
   
   assert( arg_count != 0 );
   assert( arg_vector != NULL );

   
   /* Saving arg 0 into the class that the user can access. */
   arg_zero = string(arg_vector[0]);

   i = 1; /* Skip over arg 0. */
   while ( i < arg_count )
   {
      resolved = false; /* Nothing has been matched (at this point). */
      
      if ( parse_next )
      {
         if ( parse_next->ParseItem(arg_vector[i]) )
            errmsgs.push_back("Failed to parse argument for " + parse_next->label + ".");

         parse_next = NULL;
         i++;
         continue;
      }
      
      if ( arg_vector[i][0] == '-' )
      {
         /* Is dd or sd option? */
         if ( arg_vector[i][1] == '-' )
         {
            /* Is a dd option */
            map<string, OptItem>::iterator itr = optlist.begin();
            while ( itr != optlist.end() )
            {
               if ( 0 == itr->second.ddopt.compare(&arg_vector[i][2]) )
               {
                  /* We have a match for this dd option */
                  itr->second.parsedf |= HAS_DD_ARG; /* Mark it parsed */
                  itr->second.found++;
                  resolved = true;
                  
                  if ( itr->second.features && READ_A_VAL )
                     parse_next = &itr->second;
                  
                  itr = optlist.end();
               }
               else
                  itr++;

            } /* while ( itr ) */

         } /* if ( ddopt ) */
         else
         {
            /* sd options */
            /* Here we switch over to a local loop version of the resolved flag. */
            bool llresolved;
            resolved = true;

            j = 1;
            while(arg_vector[i][j] != 0)
            {
               llresolved = false;
               
               /* This is a single dash option */
               map<string, OptItem>::iterator itr = optlist.begin();
               while ( itr != optlist.end() )
               {
                  if ( itr->second.sdopt == arg_vector[i][j] )
                  {
                     /* We have a match for this sd option */
                     itr->second.parsedf |= HAS_SD_ARG; /* Mark it parsed */
                     itr->second.found++;
                     llresolved = true;
                     
                     if ( itr->second.features & READ_A_VAL )
                     {
                        if ( arg_vector[i][j + 1] == 0 )
                           parse_next = &itr->second;
                        else
                        {
                           if ( itr->second.ParseItem(&arg_vector[i][j + 1]) )
                           {
                              errmsgs.push_back("Failed to parse argument for " + itr->second.label + ".");
                           }

                           /* Run out the vector item (only to the 0-1th place - j++ later will put us on 0). */
                           while ( arg_vector[i][j + 1] != 0 )
                              j++;
                        }
                     } /* if ( READ_A_VAL ) */
                     
                     itr = optlist.end();
                  } /* if we found a match in the optlist */
                  else
                     itr++;

               } /* while ( OptItems ) */

               if ( llresolved == false )
               {
                  string e;

                  e = "Failed to parse the -";
                  e += arg_vector[i][j];
                  e += " flag.";
                  errmsgs.push_back(e);
               }
                  
               j++;
            }
         } /* if ( ddopt ) else ( sdopt ) */
         
      } /* if ( ddopt or sdopt ) */
      else
      {
         /* This is a positional argument */
         map<string, OptItem>::iterator itr = optlist.begin();
         while ( itr != optlist.end() )
         {
            if ( itr->second.position == position )
            {
               /* We have a match for this positional option */

               /* Read out the value */
               if ( itr->second.ParseItem(arg_vector[i]) )
               {
                  errmsgs.push_back("Failed to parse " + itr->second.label + " positional arg.");
               }
               else
               {
                  itr->second.parsedf |= IS_POS_ARG; /* Mark it parsed */
                  itr->second.found++;
               }
               
               resolved = true;

               itr = optlist.end();
            }
            else
               itr++;
         }

         position++; /* Increment the position if it was parsed or not */
      } /* if ( ddopt/sdopt) else (positional) */
         
      if ( ! resolved )
      {
         /* Push error msg into a vector */
         errmsgs.push_back("Failed to parse " + string(arg_vector[i]) + ".");
      }
      
      i++;
   }

   return(0);
}

/* ========================================================================= */
int OptParse::Resolve(int argc, char *argv[])
{
   arg_count = argc;
   arg_vector = argv;

   return(resolve_args());
}

int OptParse::Resolve(void)
{
   return(resolve_args());
}

/* ========================================================================= */
bool OptParse::WasFound(string olabel)
{
   map<string, OptItem>::iterator itr = optlist.find(olabel);

   if ( itr == optlist.end() )
   {
      /* This means that the *label* was not found. */
      
      assert(1); /* assert() that the developer used a misspelled label */
      return(false);
   }

   /* Check to see if one of the parsed feature flags were set. */
   if ( itr->second.parsedf & TYPE_O_ARG )
      return(true);

   return(false);
}

/* ========================================================================= */
/* Note: This (also) could be templated... The problem is that we validate 
         the input against expected type. Changing that means that the 
         validation would need to be moved elsewhere. Not the end of the
         world, but a later design change that conflicts with the current
         (admittedly C-ish design).

         template <typename GVT> int OptParse::GetValue(string olabel, GVT &val)
*/
int OptParse::GetValue(string olabel, string & val)
{
   OptItem *oi = find_opt_item(olabel);

   /* Check to see if one of the parsed feature flags were set. */
   if ( oi->parsedf & READ_A_STR )
   {
      val = oi->str_value;
      return(0);
   }

   /* Fall-thru to error */
   return(1);
}

int OptParse::GetValue(string olabel, int & val)
{
   OptItem *oi = find_opt_item(olabel);

   /* Check to see if one of the parsed feature flags were set. */
   if ( oi->parsedf & READ_A_INT )
   {
      val = oi->int_value;
      return(0);
   }

   /* Fall-thru to error */
   return(1);
}

int OptParse::GetValue(string olabel, long &val)
{
   OptItem *oi = find_opt_item(olabel);

   /* Check to see if one of the parsed feature flags were set. */
   if ( oi->parsedf & READ_A_LNG )
   {
      val = oi->lng_value;
      return(0);
   }

   /* Fall-thru to error */
   return(1);
}

int OptParse::GetValue(string olabel, unsigned long &val)
{
   OptItem *oi = find_opt_item(olabel);
   
   /* Check to see if one of the parsed feature flags were set. */
   if ( oi->parsedf & READ_AULNG )
   {
      val = oi->ulng_value;
      return(0);
   }

   /* Fall-thru to error */
   return(1);
}

int OptParse::GetValue(string olabel, char &val)
{
   OptItem *oi = find_opt_item(olabel);
   
   /* Check to see if one of the parsed feature flags were set. */
   if ( oi->parsedf & READ_ACHAR )
   {
      val = oi->char_value;
      return(0);
   }

   /* Fall-thru to error */
   return(1);
}

/* ========================================================================= */
int OptParse::DumpErrors(void)
{
   unsigned int errors = errmsgs.size();
   
   if ( errors )
   {
      if ( errors > 1 )
         cerr << "ERROR: " << errors << " command line parsing errors were encountered." << endl;
      else
         cerr << "ERROR: A command line parsing error was encountered." << endl;

      for ( unsigned int i = 0; i < errors; i++ )
      {
         cerr << "       " << errmsgs[i] << endl;
      }

      return(1);
   }

   return(0);
}

/* ========================================================================= */
/* This always returns a proper value (or it assert()s).                     */
OptItem *OptParse::find_opt_item(string olabel)
{
   map<string, OptItem>::iterator itr = optlist.find(olabel);

   if ( itr == optlist.end() )
   {
      /* This means that the *label* was not found. */
      cerr << "ASSERT: BAD LABEL = \"" << olabel << "\"" << endl;
      assert(0); /* assert() that the developer used a misspelled label */
      exit(1);
   }

   return(&itr->second);
}

/* ========================================================================= */
int OptParse::GetState(string olabel, string &val)
{
   OptItem *oi = find_opt_item(olabel);

   if ( oi->found == 0 )
      return(0);
   
   if ( 0 == (oi->parsedf & READ_A_STR) )
      return(-1);
   
   val = oi->str_value;

   return(oi->found);
}

int OptParse::GetState(string olabel, int &val)
{
   OptItem *oi = find_opt_item(olabel);

   if ( oi->found == 0 )
      return(0);
   
   if ( 0 == (oi->parsedf & READ_A_INT) )
      return(-1);
   
   val = oi->int_value;

   return(oi->found);
}

int OptParse::GetState(string olabel, long &val)
{
   OptItem *oi = find_opt_item(olabel);

   if ( oi->found == 0 )
      return(0);
   
   if ( 0 == (oi->parsedf & READ_A_LNG) )
      return(-1);
   
   val = oi->lng_value;

   return(oi->found);
}

int OptParse::GetState(string olabel, unsigned long &val)
{
   OptItem *oi = find_opt_item(olabel);

   if ( oi->found == 0 )
      return(0);
   
   if ( 0 == (oi->parsedf & READ_AULNG) )
      return(-1);
   
   val = oi->ulng_value;

   return(oi->found);
}

int OptParse::GetState(string olabel, char &val)
{
   OptItem *oi = find_opt_item(olabel);

   if ( oi->found == 0 )
      return(0);
   
   if ( 0 == (oi->parsedf & READ_ACHAR) )
      return(-1);
   
   val = oi->char_value;

   return(oi->found);
}

/* ========================================================================= */
bool OptParse::IsExclusive(string olabel, vector<string> ev)
{
   OptItem *voi;
   /* The point of allow_false is to make the function walk the list NO MATTER
      WHAT. This is a means to force out errors in calling that testing might
      not find (unless a specific option was specified). This is designed to
      uncover misuse of the library/function faster and easier. */
   bool allow_false = true;
   
   /* Look for the item we are comparing to. Note that find_opt_item() will
      assert if the item was not in the optlist. */
   OptItem *oi = find_opt_item(olabel);

   /* Exit with no exclusivity conflict if the "comparitor" is not set. */
   if ( 0 == oi->found )
      allow_false = false;

   /* At this point - we know that olabel (the comparitor) exists, and is
      set by the user on the command line. */
   for ( auto vi : ev )
   {
      /* Insure that the olabel is not in the vector. This means that we
         are checking to insure that the caller did not put the "comparitor"
         into the vector they are comparing against. */
      assert( olabel != vi );

      voi = find_opt_item(vi);

      /* Only flag an error (return false) if olabel was not set. */
      if ( allow_false )
      {
         if ( voi->found )
            return(false);
      }
   }

   /* We could be here for two reasons:
      1. The item behind olabel was not flagged on the command line (and
         allow_false was set to false).
      2. The item behind olabel WAS flagged, and none of the other options
         in the supplied vector were set.
   */
   return(true);
}
