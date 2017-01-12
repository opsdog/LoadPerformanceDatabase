/*

  quick and dirty program to convert seconds from epoch to 
  human readable time in local timezone

  takes the seconds as the only argument

  fails ungracefully if the arg is not given

  test value:
    1355336797 == Wed 2012-12-12 13:26:37 EST

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main (int argc, char *argv[])
{
  int RetVal;
  time_t *now;
  struct tm *ts;
  char buf[80];

  char istr[80];

  strcpy(istr,argv[1]);
  if ( strlen(istr) > 10 )
    istr[10]='\0';


  /*  now=malloc(sizeof(time_t));*/
  ts=malloc(sizeof(struct tm));

  /*  sscanf(argv[1],"%d",&now);  */
  sscanf(istr,"%d",&now);
  /*  printf("%s\n%d\n",argv[1],now);*/
  tzset();

  /*  ts = localtime(&now);*/
  ts = localtime((time_t *)&now);
  /*  printf("out of localtime\n");*/

  if ( argc > 2 ) {
    if ( strcmp(argv[2],"-s") == 0 )
      RetVal=strftime(buf, sizeof(buf), "%Y%m%d%H%M", ts);
    if ( strcmp(argv[2],"-sl") == 0 )
      RetVal=strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", ts);
    }
  else
    RetVal=strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
  /*  printf("%s (%d)\n", buf,RetVal);*/
  printf("%s\n", buf);

  /*
  RetVal=strftime(buf, sizeof(buf), "%s", ts);
  printf("%s (%d)\n", buf,RetVal);
  printf("argv:  %s\n",argv[1]);
  printf("now :  %d\n",now);
  printf("ts  :  %s\n",buf);
  printf("tm_sec:  %d\n",ts->tm_sec);
  printf("tm_min:  %d\n",ts->tm_min);
  printf("tm_year: %d\n",ts->tm_year);
  */

  /*
  strcpy(buf,(char *)ctime(&now));
  printf("%s\n", buf);
  */

  /*
  ts = gmtime(&now);
  strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
  printf("%s\n", buf);
  */
  
}
