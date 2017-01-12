/*

  quick and dirty program to convert human readable time to seconds
  from epoch

  takes the human time in the format:

    YYYYMMDDHHSS

  as the only argument

  fails ungracefully if no argument is supplied

  test value:

    20121212132637 --> 1355336797

*/

#include <stdio.h>
#include <time.h>
#include <string.h>

int main (int argc, char *argv[])
{
  struct tm t;
  time_t t_of_day;
  char datestr[15];
  char yearstr[5];
  char monthstr [3];
  char daystr[3];
  char hourstr[3];
  char minstr[3];
  char secstr[3];
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  /*  remove carriage returns from the input string  */

  month = 0;
  for ( year=0 ; year<strlen(argv[1]) ; year++ )
    if ( argv[1][year] != 13 ) {
      datestr[month] = argv[1][year];
      month++;
    }
  datestr[month]='\0';

  yearstr[0]=datestr[0];
  yearstr[1]=datestr[1];
  yearstr[2]=datestr[2];
  yearstr[3]=datestr[3];
  yearstr[4]='\0';
  monthstr[0]=datestr[4];
  monthstr[1]=datestr[5];
  monthstr[2]='\0';
  daystr[0]=datestr[6];
  daystr[1]=datestr[7];
  daystr[2]='\0';
  hourstr[0]=datestr[8];
  hourstr[1]=datestr[9];
  hourstr[2]='\0';
  minstr[0]=datestr[10];
  minstr[1]=datestr[11];
  minstr[2]='\0';

  if ( datestr[12] != '\0' ) {
    secstr[0]=datestr[12];
    secstr[1]=datestr[13];
    secstr[2]='\0';
  } else
    strcpy(secstr,"00");


  sscanf(yearstr,"%d",&year);
  sscanf(monthstr,"%d",&month);
  sscanf(daystr,"%d",&day);
  sscanf(hourstr,"%d",&hour);
  sscanf(minstr,"%d",&minute);
  sscanf(secstr,"%d",&second);

  /*
  printf("%d %d %d %d %d %d\n",
	 year, month, day, hour, minute, second);
  */

  t.tm_year = year-1900;
  t.tm_mon = month - 1; // Month, 0 - jan
  t.tm_mday = day; // Day of the month
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
  t_of_day = mktime(&t);
  printf("%ld\n", (long) t_of_day);
}
