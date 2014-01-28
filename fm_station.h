#define fm_num  7
#include <avr/pgmspace.h>
prog_char string_0[] PROGMEM = "Inter FM   :76.1MHz";   // "String 0" etc are strings to store - change to suit.
prog_char string_1[] PROGMEM = "Houso Univ.:77.1MHz";
prog_char string_2[] PROGMEM = "G-WIND     :77.6MHz";
prog_char string_3[] PROGMEM = "Tokyo FM   :80.0MHz";
prog_char string_4[] PROGMEM = "J-Wave     :81.3MHz";
prog_char string_5[] PROGMEM = "NHK FM     :82.5MHz";
prog_char string_6[] PROGMEM = "ChofuFM    :83.8MHz";

PROGMEM const char *fm_stations[] =
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  };
 
