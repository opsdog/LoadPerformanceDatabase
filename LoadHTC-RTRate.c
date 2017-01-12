/*

  reads a LDEV_Read_ResponseXX.csv file from a Hitachi array

  parses it

  generates a CSV for loading into MySQL

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

#include <TokenParse.h>

extern char *DECtoHEX(long int decimalNumber);

int main(int argc, char *argv[]) {

  double DataEsttime;
  double EpochInc;

  long int Key1;

  int RRespFileDes;
  int Status;
  int TokenCount;
  int LUNperLINE = 128;
  int LUNadd;

  char Line[4096];
  char RRespInputFileName[512];
  char strTEMP[512];
  char Token[256];

  char DataArraySN[10];
  char DataLDEVControl[3];
  char DataLDEV[6];
  char DataLDEVlun[4];
  char DataRTRate[100];
  time_t  DataEpoch;

  FILE *OutputCSV;


  char strDay[5];
  char strMonth[5];
  char strDate[5];
  char strHour[5];
  char strMin[5];
  char strSec[5];
  char strYear[5];

  char datestr[15];

  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  struct tm t;
  time_t t_of_day;



#ifdef DEBUG
  printf("LoadHTC-RResp:  entry\n");
  printf("LoadHTC-RResp:  %d args\n", argc);
#endif

  if ( argc != 3) {
    printf("Usage:  %s CSVInputFileName NextKey \n", argv[0]);
    return(1);
  }
  strcpy(RRespInputFileName,argv[1]);
  sscanf(argv[2],"%ld",&Key1);

#ifdef DEBUG
  printf("LoadHTC-RResp:  %s\n", RRespInputFileName);
  printf("LoadHTC-RResp:  %ld\n", Key1);
#endif

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*               process the Read_Response file                     */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( RRespFileDes = open (RRespInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",RRespInputFileName, errno);
    return(errno);
  }

  if (( OutputCSV = fopen("HTC-RTRateLoad.csv","a") ) == (FILE *)NULL ) {
    printf("Open of RRespLoad.csv failed:  %d\n", errno);
    return(errno);
  }


  /*
    burn headers
  */

  ReadLine2(Line,RRespFileDes,&Status);

  /*  get the array serial number */

  ReadLine2(Line,RRespFileDes,&Status);

#ifdef DUMPDATA
  printf("Should have serial number --> %s\n", Line);
#endif

  /*  burn the first 3 tokens "Serial number :"  */

  strcpy(Token,GetNextToken(' ',Line));
  strcpy(Token,GetNextToken(' ',Line));
  strcpy(Token,GetNextToken(' ',Line));

  /*  next token should be the serial number and some junk  */

  strcpy(Token,GetNextToken(' ',Line));
#ifdef DUMPDATA
  printf("Should be serial number + --> %s\n",Token);
#endif
  strcpy(DataArraySN,GetNextToken('(',Token));
#ifdef DUMPDATA
  printf("Should be serial number   --> %s\n",DataArraySN);
#endif

  /*  burn the rest of the headers  */

  ReadLine2(Line,RRespFileDes,&Status);
  ReadLine2(Line,RRespFileDes,&Status);
  ReadLine2(Line,RRespFileDes,&Status);

  /*  next line should be data header  */

  ReadLine2(Line,RRespFileDes,&Status);
  ReadLine2(Line,RRespFileDes,&Status);

#ifdef DUMPDATA
  printf("Should be data header --> %s\n",Line);
#endif

  /*  get the LDEV control portion  */

  strcpy(Token,GetNextToken(',',Line));
  strcpy(Token,GetNextToken(',',Line));
  strcpy(Token,GetNextToken(',',Line));

#ifdef DUMPDATA
  printf("Should have LDEV entry for XX:00 --> %s\n",Token);
#endif

  DataLDEVControl[2]='\0';
  DataLDEVControl[1]=Token[5];
  DataLDEVControl[0]=Token[4];

#ifdef DUMPDATA
  printf("DataLDEVControl --> %s\n",DataLDEVControl);
#endif

  /*
      now we're into real data
  */

  LUNadd=0;
  ReadLine4(Line,RRespFileDes,&Status);
  while ( !EndOfFile ) {
#ifdef DUMPDATA
    printf("\nLine (%d) --> %s\n",Status,Line);
#endif

    /*  discard the internal line number get the line timestamp  */

    strcpy(Token,GetNextToken(',',Line));
    if ( strcmp(Token,"\"No.\"") == 0 ) {

#ifdef DUMPDATA
      printf("\n\n\nNEXT SECTION BEGINS\n\n\n");
#endif

      LUNadd=128;

      /*  consume this line and set up the next line of data  */

      ReadLine4(Line,RRespFileDes,&Status);
#ifdef DUMPDATA
      printf("\nLine (%d) --> %s\n",Status,Line);
#endif
      strcpy(Token,GetNextToken(',',Line));
      
      /*      exit(8);*/
    }

    strcpy(Token,GetNextToken(',',Line));
#ifdef DUMPDATA
    printf("  should be the quoted date string --> %s\n",Token);
#endif

    strYear[0]=Token[1];
    strYear[1]=Token[2];
    strYear[2]=Token[3];
    strYear[3]=Token[4];
    strYear[4]='\0';

    strMonth[0]=Token[6];
    strMonth[1]=Token[7];
    strMonth[2]='\0';

    strDate[0]=Token[9];
    strDate[1]=Token[10];
    strDate[2]='\0';

    strHour[0]=Token[12];
    strHour[1]=Token[13];
    strHour[2]='\0';

    strMin[0]=Token[15];
    strMin[1]=Token[16];
    strMin[2]='\0';

    strcpy(strSec,"00");

    sprintf(datestr,"%s%s%s%s%s%s",strYear,strMonth,strDate,strHour,strMin,strSec);

#ifdef DUMPDATA
    printf("  date components --> %s / %s / %s  %s : %s : %s\n",
	   strYear, strMonth, strDate,
	   strHour, strMin, strSec);
    printf("  date string     --> %s\n",datestr);
#endif

    sscanf(strYear,"%d",&year);
    sscanf(strMonth,"%d",&month);
    sscanf(strDate,"%d",&day);
    sscanf(strHour,"%d",&hour);
    sscanf(strMin,"%d",&minute);
    sscanf(strSec,"%d",&second);

    t.tm_year = year-1900;
    t.tm_mon = month - 1; // Month, 0 - jan
    t.tm_mday = day; // Day of the month
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown                      
    DataEpoch = mktime(&t);
#ifdef DUMPDATA
    printf("  epoch date --> %ld\n", (long) DataEpoch);
#endif

    /*
        now read the LDEV entries - should be 128 of 'em
    */

    for ( TokenCount = 0 ; TokenCount < LUNperLINE ; TokenCount++ ) {
      strcpy(Token,GetNextToken(',',Line));

#ifdef DUMPDATA
      printf("(%d) %s ",TokenCount,Token);
#endif

      /*  create the full LDEV serial number  */

      strcpy(strTEMP,DECtoHEX((long int)(TokenCount + LUNadd)));
      if ( strlen(strTEMP) == 1 ) {
	DataLDEVlun[0] = '0';
	DataLDEVlun[1] = strTEMP[0];
      } else
	strcpy(DataLDEVlun, strTEMP);

#ifdef DUMPDATA
      printf("-%s- ",DataLDEVlun);
#endif

      sprintf(DataLDEV,"%s:%s",DataLDEVControl,DataLDEVlun);

#ifdef DUMPDATA
      printf("-%s- ",DataLDEV);
#endif

      /*

          generate a CSV and combine all these intermediate CSVs to populate
          the DB

      */


      /*  
          a value of -3 seems to be magick code for "no data collected"

	  turn them into zeros
      */

      /*  if ( strcmp(Token,"-3") == 0 )  */
      if ( Token[0] == '-' )
	strcpy(DataRTRate,"0");
      else
	strcpy(DataRTRate,Token);

      fprintf(OutputCSV,"%s,%s,%ld,%s\n",
	      DataArraySN, DataLDEV, DataEpoch, DataRTRate);


    }  /*  for each LDEV on a line of data  */
#ifdef DUMPDATA
    printf("\n");
#endif


    ReadLine4(Line,RRespFileDes,&Status);
  }  /*  while not EOF  */


  /*  clean up and go home  */

#ifdef DEBUG
  printf("\n");
  printf("\n");
  printf("LoadMPstat-CSV-TC:  Ending stats:\n");

#endif

  fclose(OutputCSV);
  close(RRespFileDes);

}
