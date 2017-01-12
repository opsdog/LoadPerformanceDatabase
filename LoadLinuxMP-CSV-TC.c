/*

  reads a file of mpstat output and parses it into CSV to be loaded
  into a mysql table

*/

/*
#undef SHOWPROGRESS
#define SHOWPROGRESS
*/

/*
#undef DEBUG
#define DEBUG
*/

/*
#undef DBDEBUG
#define DBDEBUG
*/

/*
#undef DUMPDATA
#define DUMPDATA
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <time.h>

#include <TokenParse.h>

int main(int argc, char *argv[]) {

  double DataEsttime;
  double EpochInc;

  long int Key1;

  int MPstatFileDes;
  int Status;
  int EpochStart;
  int EpochEnd;
  int EpochSpan;
  int NumCollections;

  int DataServerID;
  int Datacpu;

  float Datausr;
  float Datanice;
  float Datasys;
  float Dataiowait;
  float Datairq;
  float Datasoft;
  float Datasteal;
  float Dataguest;
  float Dataidle;

  char DataJunk[25];
  char DataColDate[20];
  char ColAdjDate[20];

  char MPstatInputFileName[512];
  char Token[256];
  char Line[2014];

  FILE *OutputCSV;

  /*  for the time conversion to get EpochStart  */

  struct tm t;
  time_t t_of_day;
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


#ifdef DEBUG
  printf("LoadLinuxMP-CSV-TC:  entry\n");
  printf("LoadLinuxMP-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 5) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate NextKey \n", argv[0]);
    return(1);
  }
  strcpy(MPstatInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%ld",&Key1);
  ColAdjDate[0] = DataColDate[0];
  ColAdjDate[1] = DataColDate[1];
  ColAdjDate[2] = DataColDate[2];
  ColAdjDate[3] = DataColDate[3];
  ColAdjDate[4] = DataColDate[4];
  ColAdjDate[5] = DataColDate[5];
  ColAdjDate[6] = DataColDate[6];
  ColAdjDate[7] = DataColDate[7];
  ColAdjDate[8]='\0';

#ifdef DEBUG
  printf("LoadLinuxMP-CSV-TC:    server id %d starting %s\n", 
	 DataServerID, DataColDate);
  printf("LoadLinuxMP-CSV-TC:    AdjDate base %s\n", ColAdjDate);

#endif

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                    process the mpstat file                       */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( MPstatFileDes = open (MPstatInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",MPstatInputFileName, errno);
    return(errno);
  }

  if (( OutputCSV = fopen("MPstatLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of MPstatLoad.csv failed:  %d\n", errno);
    return(errno);
  }

  /*  first 3 lines are header - burn 'em  */

  ReadLine2(Line,MPstatFileDes,&Status);
  ReadLine2(Line,MPstatFileDes,&Status);
  ReadLine2(Line,MPstatFileDes,&Status);

  /*  this should be the first CPU = all line - sorta burn it too  */

  ReadLine2(Line,MPstatFileDes,&Status);

  ColAdjDate[8] = Line[0];
  ColAdjDate[9] = Line[1];
  ColAdjDate[10] = Line[3];
  ColAdjDate[11] = Line[4];
  ColAdjDate[12] = Line[6];
  ColAdjDate[13] = Line[7];
  ColAdjDate[14] = '\0';

  /*  
      ColAdjDate now holds the string format of the starting collection

      turn it into EpochStart
  */

  yearstr[0]=ColAdjDate[0];
  yearstr[1]=ColAdjDate[1];
  yearstr[2]=ColAdjDate[2];
  yearstr[3]=ColAdjDate[3];
  yearstr[4]='\0';
  monthstr[0]=ColAdjDate[4];
  monthstr[1]=ColAdjDate[5];
  monthstr[2]='\0';
  daystr[0]=ColAdjDate[6];
  daystr[1]=ColAdjDate[7];
  daystr[2]='\0';
  hourstr[0]=ColAdjDate[8];
  hourstr[1]=ColAdjDate[9];
  hourstr[2]='\0';
  minstr[0]=ColAdjDate[10];
  minstr[1]=ColAdjDate[11];
  minstr[2]='\0';

  if ( ColAdjDate[12] != '\0' ) {
    secstr[0]=ColAdjDate[12];
    secstr[1]=ColAdjDate[13];
    secstr[2]='\0';
  } else
    strcpy(secstr,"00");

  sscanf(yearstr,"%d",&year);
  sscanf(monthstr,"%d",&month);
  sscanf(daystr,"%d",&day);
  sscanf(hourstr,"%d",&hour);
  sscanf(minstr,"%d",&minute);
  sscanf(secstr,"%d",&second);

#ifdef DEBUG
  printf("LoadLinuxMP-CSV-TC:  Converting ColAdjDate - intermediate values:\n");
  printf("LoadLinuxMP-CSV-TC:    %d %d %d %d %d %d\n",
         year, month, day, hour, minute, second);
#endif

  t.tm_year = year-1900;
  t.tm_mon = month - 1; // Month, 0 - jan
  t.tm_mday = day; // Day of the month
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
  t_of_day = mktime(&t);
  EpochStart = t_of_day;

#ifdef DEBUG
  printf("LoadLinuxMP-CSV-TC:  We just read this line - should be CPU = all:\n");
  printf("LoadLinuxMP-CSV-TC:    Got the EpochStart from this line:\n");
  printf("LoadLinuxMP-CSV-TC:    %s\n", Line);
  printf("LoadLinuxMP-CSV-TC:  ColAdjDate %s\n", ColAdjDate);
  printf("LoadLinuxMP-CSV-TC:  EpochStart %d\n", EpochStart);
#endif

  /*  enter the loop pointing at data...  */

  ReadLine2(Line,MPstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadLinuxMP-CSV-TC:  About to enter main while with Line:\n");
  printf("LoadLinuxMP-CSV-TC:    %s\n\n", Line);
#endif

  DataEsttime = EpochStart;
  while ( !EndOfFile ) {

#ifdef DEBUG
    printf("LoadLinuxMP-CSV-TC:  Top of read while - about to read a block...\n");
#endif

    while ( strlen(Line) != 0 ) {

#ifdef DEBUG
      printf("LoadLinuxMP-CSV-TC:    %s\n", Line);
#endif

      sscanf(Line, "%s %d %f %f %f %f %f %f %f %f %f",
	     DataJunk, &Datacpu, &Datausr, &Datanice, &Datasys, &Dataiowait,
	     &Datairq, &Datasoft, &Datasteal, &Dataguest, &Dataidle);

      fprintf(OutputCSV, "%ld,%d,%s,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
              Key1, DataServerID, DataColDate, (int)DataEsttime,
	      Datacpu, Datausr, Datanice, Datasys, Dataiowait,
	      Datairq, Datasoft, Datasteal, Dataguest, Dataidle);
      Key1++;


      ReadLine2(Line,MPstatFileDes,&Status);
    }  /*  while reading a block of data  */

    DataEsttime += 15;

#ifdef DEBUG
    printf("\n\n");
    printf("LoadLinuxMP-CSV-TC:  out of read block - Line is:\n");
    printf("LoadLinuxMP-CSV-TC:    %s\n", Line);
#endif
    
    /*  burn the header line */

    ReadLine2(Line,MPstatFileDes,&Status);

    /*  burn the CPU = all line  */

    ReadLine2(Line,MPstatFileDes,&Status);

    /*  read the CPU = 0 line to prep the data for the next block  */

    ReadLine2(Line,MPstatFileDes,&Status);



  }  /*  while not end of file  */


  /*  clean up and go home  */

#ifdef DEBUG
  printf("\n");
  printf("\n");
  printf("LoadMPstat-CSV-TC:  Ending stats:\n");
  /*  printf("LoadMPstat-CSV-TC:  DataEsttime - EpochStart:  %f\n",*/
  /*	 ((int)DataEsttime - EpochStart) );*/
  /*  printf("LoadMPstat-CSV-TC:   StartEpoch    EndEpoch  EpochSpan Collections   EpochInc CalcEpochEnd   MissedBy\n");*/
  /*  printf("LoadMPstat-CSV-TC:  %11d %11d %12d %11d %8.4f %10.2lf %10d\n",*/
  /*	 EpochStart, EpochEnd, EpochSpan, NumCollections, EpochInc, DataEsttime,*/
  /*	 (EpochEnd - (int)DataEsttime) );*/
  printf("\n");
#endif

  fclose(OutputCSV);
  close(MPstatFileDes);

}
