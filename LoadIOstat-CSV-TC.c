/*

  reads an iostat -xn file (argv[1]), parses it, and loads
  it into a CSV to be loaded into the DB

*/

/*
#undef SHOWPROGRESS
#define SHOWPROGRESS
*/


#undef DEBUG
#define DEBUG


/*
#undef DBDEBUG
#define DBDEBUG
*/

/*
#undef DUMPDATA
#define DUMPDATA
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/errno.h>
#include <fcntl.h>

/*
#include <my_global.h>
#include <mysql.h>
#include <my_sys.h>
*/

#include <TokenParse.h>

int main(int argc, char *argv[]) {

  int IOstatFileDes;
  int Status;
  long int Key;

  char IOInputFileName[512];
  char Token[256];
  char Line[1024];

  int  DataServerID;
  char DataColDate[20];
  /*  time_t  DataEpoch;*/
  double  DataEpoch;

  float Datars;
  float Dataws;
  float Datakrs;
  float Datakws;
  float Datawait;
  float Dataactv;
  float Datawsvct;
  float Dataasvct;
  int Datapctw;
  int Datapctb;
  char Datadevice[41];
  float Dataavgread;
  float Dataavgwrit;

  char strDay[5];
  char strMonth[5];
  char strDate[5];
  char strHour[5];
  char strMin[5];
  char strSec[5];
  char strYear[5];

  char datestr[20];

  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  struct tm t;
  time_t t_of_day;

  int EpochStart,
    EpochEnd,
    EpochSpan,
    EpochCount;
  double EpochInc;

  FILE *OutputFILE;

  /*  DB vars  */
  /*
  FILE *DBvip;
  char DBLocation[20];
  char DBHost[30];
  char DBUser[30];
  char DBPassword[30];
  char DBName[50];
  char DBCommand[2048];
  int DBCommandLength;

  MYSQL QueryDB;
  MYSQL_RES *QueryResult;
  MYSQL_ROW QueryRow;
  */

  /* arg checking */

#ifdef DEBUG
  printf("LoadIOstat-CVS-TC:  Enter\n");
#endif

  if ( argc != 7) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate EpochStart EpochEnd NextKey\n", argv[0]);
    return(1);
  }
  strcpy(IOInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%d",&EpochStart);
  sscanf(argv[5],"%d",&EpochEnd);
  sscanf(argv[6],"%ld",&Key);
  DataEpoch = (double)EpochStart;

  EpochSpan = EpochEnd - EpochStart;

#ifdef DEBUG
  printf("LoadIOstat-CVS-TC:  %s %d %d\n",IOInputFileName, DataServerID, EpochStart);
#endif

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                  process the iostat file                         */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/


  /********************************************************************/
  /*                                                                  */
  /*  open the file - parse it like it was being processed            */
  /*  but only count the number of collections                        */
  /*                                                                  */
  /********************************************************************/


  if (( IOstatFileDes = open (IOInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",IOInputFileName, errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadIOstat-CSV-TC:  input file opened\n");
#endif

  /*  discard header lines  */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);

  /*  discard the cumulative lines  */

  ReadLine2(Line,IOstatFileDes,&Status);

  /*  printf("Should be %d real data --> %s\n",strlen(Line), Line);*/
  
  while ( strlen(Line) != 60 ) {
    /*    printf("burning --> (%d) %s\n",strlen(Line),Line);*/
    ReadLine2(Line,IOstatFileDes,&Status);
  }

  /*  printf("out of burn loop --> %s\n", Line);*/

  /*  burn the next 2 header lines */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);

  /*  printf("processing real data with --> %s\n",Line);*/

  EpochCount = 0;

  while ( !EndOfFile ) {
    /*    printf("Top of while --> (%d) %s\n",strlen(Line),Line);*/

    while ( ( strlen(Line) != 60 ) && ( strlen(Line) != 0 ) ) {
      /*
      printf("inner while --> processing line --> (%d) %s\n",
	     strlen(Line),Line);
      */

      /*  burn each data entry  */

      ReadLine2(Line,IOstatFileDes,&Status);
    }  /*  end while processing chunk of data  */

    /*  done processing a chunk of data - update epoch seconds  */

    EpochCount++;

    /*  and burn the second header line  */

    ReadLine2(Line,IOstatFileDes,&Status);
    ReadLine2(Line,IOstatFileDes,&Status);

    /*    printf("Bot of while --> %s\n",Line);*/

  }  /*  while not EOF  */

  close(IOstatFileDes);

  /*  EpochInc = ( (float)EpochSpan / (float)EpochCount ) + 1.0;*/
  EpochInc = (double)EpochSpan / (double)EpochCount;

  /*
  printf("\n\n");
  printf("\tServerID   = %d\n",DataServerID);
  printf("\tCol Date   = %s\n",DataColDate);
  printf("\tEpochStart = %d\n",EpochStart);
  printf("\tEpochEnd   = %d\n",EpochEnd);
  printf("\tEpochSpan  = %d\n",EpochSpan);
  printf("\tEpochCount = %d\n",EpochCount);
  printf("\tEpochInc   = %lf\n",EpochInc);
  printf("\n\tDataEpoch  = %lf\n",DataEpoch);
  printf("\n\n");
  */

  /********************************************************************/
  /*                                                                  */
  /*  open the file - parse it for real                               */
  /*                                                                  */
  /********************************************************************/

  EndOfFile = 0;

  if (( IOstatFileDes = open (IOInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",IOInputFileName, errno);
    return(errno);
  }

  if (( OutputFILE = fopen("IOStatLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of IOStatLoad.csv failed:  %d\n", errno);
    return(errno);
  }

  /*  discard header lines  */

  /*
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);


  /*  discard the cumulative lines  */

  /*  strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
  ReadLine2(Line,IOstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadIOstat-CSV-TC:  Should be %d real data --> %s\n",strlen(Line), Line);
#endif
  
  while ( strlen(Line) != 60 ) {
    /*    printf("burning --> (%d) %s\n",strlen(Line),Line);*/
    /*    strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
    ReadLine2(Line,IOstatFileDes,&Status);
  }

  /*  printf("out of burn loop --> %s\n", Line);*/

  /*  burn the next 2 header lines */

  /*
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);

  /*  printf("processing real data with --> %s\n",Line);*/

  while ( !EndOfFile ) {
    /*    printf("Top of while --> (%d) %s\n",strlen(Line),Line);*/
    /*    printf("  EpochInc:  %lf\n",EpochInc);*/
    
    while ( ( strlen(Line) != 60 ) && ( strlen(Line) != 0 ) ) {
      /*
      printf("inner while --> processing line --> (%d) %s\n",
	     strlen(Line),Line);
      */

      /*  process each data entry  */

      sscanf(Line," %f %f %f %f %f %f %f %f %d %d %s",
	     &Datars, &Dataws, &Datakrs, &Datakws, &Datawait, &Dataactv,
	     &Datawsvct, &Dataasvct, &Datapctw, &Datapctb, Datadevice);

      /*
      printf("%f %f %f %f %f %f %f %f %d %d %s\n",
	     Datars, Dataws, Datakrs, Datakws, Datawait, Dataactv,
	     Datawsvct, Dataasvct, Datapctw, Datapctb, Datadevice);
      */

      if ( Datars != 0.0 )
	Dataavgread = Datakrs / Datars;
      else
	Dataavgread = 0.0;

      if ( Dataws != 0.0 )
	Dataavgwrit = Datakws / Dataws;
      else
	Dataavgwrit = 0.0;

      fprintf(OutputFILE,"%ld,%d,%s,%d,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%s,%f,%f\n",
	      Key,
	      DataServerID, DataColDate, (int)DataEpoch,
	      Datars, Dataws, Datakrs, Datakws, Datawait, Dataactv,
	      Datawsvct, Dataasvct, Datapctw, Datapctb, Datadevice,
	      Dataavgread, Dataavgwrit);
      Key++;


      ReadLine2(Line,IOstatFileDes,&Status);
    }  /*  end while processing chunk of data  */

    /*  done processing a chunk of data - update epoch seconds  */

    /*    printf("  About to add %lf to %lf --> ",DataEpoch, EpochInc);*/

    /*    DataEpoch+=EpochInc;*/
    DataEpoch = DataEpoch + EpochInc;

    /*    printf("%lf\n",DataEpoch);*/

    /*  and burn the second header line  */

    /*
    strcpy(Line,ReadLine(IOstatFileDes,&Status));
    strcpy(Line,ReadLine(IOstatFileDes,&Status));
    */

    ReadLine2(Line,IOstatFileDes,&Status);
    ReadLine2(Line,IOstatFileDes,&Status);


    /*    printf("Bot of while --> %s\n",Line);*/


    /*    strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
  }  /*  while not EOF  */



  /*
    clean up and go home
  */

  /*
  mysql_free_result(QueryResult);
  mysql_close(&QueryDB);
  */

  close(IOstatFileDes);
  fclose(OutputFILE);


}
