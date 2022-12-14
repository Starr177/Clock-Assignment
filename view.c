/* view.c -- view module for clock project (the V in MVC)
 *
 * Darren Provine, 17 July 2009
 * Jason Bempong 
 * Nov 27th, 2022
 * Copyright (C) Darren Provine, 2009-2022, All Rights Reserved
 */

#include "clock.h"
#include "view.h"
#define  AMPM_MODE  0x01
#define  DATE_MODE  0x02
#define  LED_MODE   0x04
#define  TEST_MODE  0x10


/* see "view.h" for list of bits that set properties */


int view_props = 0x00; // default is 24-hour mode, plain text

// returns old properties so you can save them if needed
void set_view_properties(int viewbits)
{
    view_props = viewbits;
}

int get_view_properties()
{
    return view_props;
}


void do_test(struct tm *dateinfo)
{
    // turn display bits on and off
    
    display();
    fflush(stdout);
}

#define MAX_TIMESTR 40 // big enough for any valid data
// make_timestring
// returns a string formatted from the "dateinfo" object.
char * make_timestring (struct tm *dateinfo, int dividers)
{
    // Declaring the pointer like this can lead to bugs!
    // This crashes if it's not set!
    // We will do './clock -a', it will crash, we'll see how to fix it.
    // NEVER DO THIS!
    char *timeformat = "(unset)"; // see strftime(3)
     
              
        // if dividers is true:
        //   make a string such as "10/31/12 dt" or " 3/17/12 dt"
        //   (note: no leading zero on month!)
        // if dividers is false:
        //   make a string such as "103112d" or " 31712d"
        //   (note: no leading zero on month!)
      if ( view_props & DATE_MODE ) {
        if ( dividers ) {
           timeformat = "%-m/%d/%Y dt";
             
          } else {
	   timeformat = "%-m%d%y dt";

      } }else  { 

        // if dividers is true:
        //   am/pm: make a string such as "11:13:52 am" or " 4:21:35 pm"
        //          (note: no leading zero on hour!)
        //   24 hr: make a string such as "14:31:25 24"
        //          (include leading zero on hour)
        // if dividers is false:
        //   am/pm: make a string such as "111352a" or " 42135p"
        //          (note: no leading zero on hour!)
        //   24 hr: make a string such as "1431252"
        //          (include leading zero on hour)
        // see strftime(3) for details
        if ( dividers ) {
            if ( view_props & AMPM_MODE ) {
               timeformat = "%l:%M:%S %P";

            } else {
                timeformat = "%H:%M:%S 24";
            }
        } else {
                timeformat = "%H%M%S 24";
        }
    }

    // make the timestring and return it
    static char timestring[MAX_TIMESTR];
    strftime(timestring, MAX_TIMESTR, timeformat, dateinfo);
    return timestring;
}

/* We get a pointer to a "struct tm" object, put it in a string, and
 * then send it to the screen.
 */
void show_led(struct tm *dateinfo)
{

    digit *where = get_display_location();
    int i;
    digit  bitvalues = 0;
    int hour;
    int indicator;

    if ( view_props & TEST_MODE ) {
        do_test(dateinfo);
        return;
    }

    // This is wrong for two reasons:
    //  1: hard to put in debug print statement
    //  2: calls 'make_timestring() six times instead of once
    // The result should be in a variable, and then switch on
    // the variable.
    for (i = 0; i < 6; i++) {
        switch ( make_timestring(dateinfo, 0)[i] ) {
            case ' ': bitvalues = 0x00; break;
            case '1': bitvalues = 0x03; break;
            case '2': bitvalues = 0x76; break;
            case '3': bitvalues = 0x57; break;
            case '4': bitvalues = 0x1B; break;
            case '5': bitvalues = 0x5D; break;
            case '6': bitvalues = 0x7D; break;
            case '7': bitvalues = 0x07; break;
            case '8': bitvalues = 0x7F; break;
            case '9': bitvalues = 0x1F; break;
            case '0': bitvalues = 0x6F; break;
        }
        where[i] = bitvalues;
    }
    display();
    fflush(stdout);
}

void show_text(struct tm *dateinfo)
{
    printf("\r%s ", make_timestring(dateinfo, 1));
    fflush(stdout);
}


void show(struct tm *dateinfo)
{
    if ( view_props & LED_MODE )
        show_led(dateinfo);
    else
        show_text(dateinfo);        
}
