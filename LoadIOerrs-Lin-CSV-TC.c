/*

  reads an iostat -xne file (argv[1]), parses it, and loads
  it into a pair of CSVs to be loaded into the DB

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

  double  DataEpoch;

  long int Key1;
  /*  long int Key2;  */

  int IOstatFileDes;
  int Status;
  int DataServerID;
  int i;

  char DataColDate[20];
  char IOInputFileName[512];
  char Token[256];
  char Line[1024];

  /*  the iostat data  */

  char Datadevice[75];
  float Datarrqms;
  float Datawrqms;
  float Datars;
  float Dataws;
  float Datakrs;
  float Datakws;
  float Dataavgrqsz;
  float Dataavgqusz;
  float Dataawait;
  float Datasvctm;
  float Datautil;

  float Dataavgread;
  float Dataavgwrit;
  int Datadevtype;

  /*
  char strDay[5];
  char strMonth[5];
  char strDate[5];
  char strHour[5];
  char strMin[5];
  char strSec[5];
  char strYear[5];
  */

  /*
  char datestr[20];
  */

  /*
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  */

  /*
  struct tm t;
  time_t t_of_day;
  */

  int EpochStart,
    EpochEnd,
    EpochSpan,
    EpochCount;
  double EpochInc;

  FILE *OutputFILEstats;
  FILE *OutputFILEerrs;

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
  printf("LoadIOerrs-Lin-CSV-TC:  entry\n");
  printf("LoadIOerrs-Lin-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 8) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate EpochStart EpochEnd EpochCount NextKey1\n", argv[0]);
    return(1);
  }
  strcpy(IOInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%d",&EpochStart);
  sscanf(argv[5],"%d",&EpochEnd);
  sscanf(argv[6],"%d",&EpochCount);
  sscanf(argv[7],"%ld",&Key1);
  DataEpoch = (double)EpochStart;

  EpochSpan = EpochEnd - EpochStart;
  EpochCount = EpochCount--;

  /*  printf("%s %d %d\n",IOInputFileName, DataServerID, EpochStart);*/

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


  /*  if (( IOstatFileDes = open (IOInputFileName, O_RDONLY) ) == -1 ) {*/
  /*    printf("Open of %s failed:  %d\n",IOInputFileName, errno);*/
  /*    return(errno);*/
  /*  }*/

  /*  discard header lines  */

  /*  ReadLine2(Line,IOstatFileDes,&Status);*/
  /*  ReadLine2(Line,IOstatFileDes,&Status);*/

  /*  discard the cumulative lines  */

  /*  ReadLine2(Line,IOstatFileDes,&Status);*/

  /*#ifdef DEBUG*/
  /*  printf("Should be %d real data --> %s\n",strlen(Line), Line);*/
  /*#endif*/
  
  /*  while ( strlen(Line) != 77 ) {*/
  /*#ifdef DEBUG*/
  /*    printf("burning --> (%d) %s\n",strlen(Line),Line);*/
  /*#endif*/
  /*    ReadLine2(Line,IOstatFileDes,&Status);*/
  /*  }*/

  /*#ifdef DEBUG*/
  /*  printf("out of burn loop --> %s\n", Line);*/
  /*#endif*/

  /*  burn the next 2 header lines */

  /*  ReadLine2(Line,IOstatFileDes,&Status);*/
  /*  ReadLine2(Line,IOstatFileDes,&Status);*/

  /*#ifdef DEBUG*/
  /*  printf("processing real data with --> %s\n",Line);*/
  /*#endif*/

  /*  EpochCount = 0;*/

  /*  while ( !EndOfFile ) {*/
    /*    printf("Top of while --> (%d) %s\n",strlen(Line),Line);*/

  /*    while ( ( strlen(Line) != 77 ) && ( strlen(Line) != 0 ) ) {*/
      /*
      printf("inner while --> processing line --> (%d) %s\n",
	     strlen(Line),Line);
      */

      /*  burn each data entry  */

  /*      ReadLine2(Line,IOstatFileDes,&Status);*/
  /*    }*/  /*  end while processing chunk of data  */

    /*  done processing a chunk of data - update epoch seconds  */

  /*    EpochCount++;*/

    /*  and burn the second header line  */

  /*    ReadLine2(Line,IOstatFileDes,&Status);*/
  /*    ReadLine2(Line,IOstatFileDes,&Status);*/

    /*    printf("Bot of while --> %s\n",Line);*/

  /*  }*/  /*  while not EOF  */

  /*  close(IOstatFileDes);*/

  /*  EpochInc = ( (float)EpochSpan / (float)EpochCount ) + 1.0;*/


  EpochInc = (double)EpochSpan / (double)EpochCount;

#ifdef DEBUG
  printf("LoadIOerrs-Lin-CSV-TC:  args processed\n");
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
#endif

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

  if (( OutputFILEstats = fopen("IOStatLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of IOStatLoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadIOerrs-Lin-CSV-TC:  input and output files open\n");
#endif

  /*  discard header lines  */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);


  /*  discard the cumulative lines  */

  ReadLine2(Line,IOstatFileDes,&Status);

#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:  Should be %d real data --> %s\n",strlen(Line), Line);
#endif

#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:  Entering burn loop...\n");
#endif
  while ( strlen(Line) != 0 ) {
#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:    burning --> (%d) %s\n",strlen(Line),Line);
#endif
    ReadLine2(Line,IOstatFileDes,&Status);
  }

#ifdef DEBUG
  printf("LoadIOerrs-Lin-CSV-TC:  OUT OF BURN LOOP --> %s\n\n", Line);
#endif

  /*  burn the next 2 header lines */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadIOerrs-Lin-CSV-TC:  processing real data with --> %s\n",Line);
#endif

  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:    TOP OF WHILE --> (%d) %s\n",strlen(Line),Line);
    printf("LoadIOerrs-Lin-CSV-TC:    EpochInc:  %lf\n",EpochInc);
#endif
    
    while ( strlen(Line) != 0 ) {
#ifdef DEBUG
      printf("LoadIOerrs-Lin-CSV-TC:      inner while --> processing line --> (%d) %s\n",
	     strlen(Line),Line);
#endif

      /*  process each data entry  */

      sscanf(Line,"%s %f %f %f %f %f %f %f %f %f %f %f",
	     Datadevice, &Datarrqms, &Datawrqms, &Datars, &Dataws,
	     &Datakrs, &Datakws, &Dataavgrqsz, &Dataavgqusz, &Dataawait,
	     &Datasvctm, &Datautil);




#ifdef DEBUG
      printf("LoadIOerrs-Lin-CSV-TC:      %s %f %f %f %f %f %f %f %f %f %f %f\n",
	     Datadevice, Datarrqms, Datawrqms, Datars, Dataws,
	     Datakrs, Datakws, Dataavgrqsz, Dataavgqusz, Dataawait,
	     Datasvctm, Datautil);
#endif

      if ( Datars != 0.0 )
	Dataavgread = Datakrs / Datars;
      else
	Dataavgread = 0.0;

      if ( Dataws != 0.0 )
	Dataavgwrit = Datakws / Dataws;
      else
	Dataavgwrit = 0.0;

      /*
	determine type of device
      */

#ifdef DEBUG
      printf("LoadIOerrs-Lin-CSV-TC:  start determine device type\n");
      printf("LoadIOerrs-Lin-CSV-TC:    device name: %s\n",Datadevice);
#endif

      int HasT;
      int indexT;
      int HasD;
      int indexD;
      int HasSlash;
      int HasColon;
      int DTDiff;

      Datadevtype = -1;
      HasSlash = 0;
      HasColon = 0;
      HasT = 0;
      HasD = 0;
      indexT = -1;
      indexD = -1;
      for ( i=0; i<strlen(Datadevice); i++ ) {
#ifdef DEBUG
	printf("LoadIOerrs-Lin-CSV-TC:    %d %c\n",i, Datadevice[i]);
#endif
	if ( Datadevice[i] == 't' ) {
	  HasT = 1 ; indexT = i;
	}  /*  if t  */
	if ( Datadevice[i] == 'd' ) {
	  HasD = 1 ; indexD = i;
	}  /*  if d  */
	if ( Datadevice[i] == '/' )
	  HasSlash = 1;
	if ( Datadevice[i] == ':' )
	  HasColon = 1;
      }  /*  for each character in device name  */

#ifdef DEBUG
      printf("LoadIOerrs-Lin-CSV-TC:    HasSlash : %d\n",HasSlash);
      printf("LoadIOerrs-Lin-CSV-TC:    HasT     : %d\n",HasT);
      printf("LoadIOerrs-Lin-CSV-TC:    indexT   : %d\n",indexT);
      printf("LoadIOerrs-Lin-CSV-TC:    HasD     : %d\n",HasD);
      printf("LoadIOerrs-Lin-CSV-TC:    indexD   : %d\n",indexD);
      printf("LoadIOerrs-Lin-CSV-TC:    HasColon : %d\n",HasColon);
#endif

      /*
	Solaris rules:
	  NFS will have a : in the name
	  tape will have a / in the name
	  controller with have no t or d
	  SAN will have both t and d
	  internal will have both t and d "close" together
      */

      if ( HasColon == 1 )
	Datadevtype = 4;
      else 
	if ( HasSlash == 1 )
	  Datadevtype = 3;
	else
	  if ( ( HasT == 0 ) && ( HasD == 0 ) )
	    Datadevtype = 0;
	  else
	    if ( ( HasT == 1 ) && ( HasD == 1 ) ) {
	      DTDiff = indexD - indexT;
	      if ( DTDiff <= 2 )
		Datadevtype = 1;
	      else
		Datadevtype = 2;
	    }  /*  if has both t and d  */

      /*
	Linux rules:
	  sd* is SAN
	  dm* is mapper
      */

      if ( (Datadevice[0] == 's') && (Datadevice[1] == 'd') )
	Datadevtype = 2;
      if ( (Datadevice[0] == 'd') && (Datadevice[1] == 'm') )
	Datadevtype = 5;
      if ( (Datadevice[0] == 'V') && (Datadevice[1] == 'x') &&
	   (Datadevice[2] == 'V') && (Datadevice[3] == 'M') )
	Datadevtype = 6;

#ifdef DEBUG
      if ( Datadevtype == 0 )
	printf("LoadIOerrs-Lin-CSV-TC:  device type is CONTROLLER\n");
      else 
	if ( Datadevtype == 1 )
	  printf("LoadIOerrs-Lin-CSV-TC:  device type is INT DISK\n");
	else
	  if ( Datadevtype == 2 )
	    printf("LoadIOerrs-Lin-CSV-TC:  device type is SAN\n");
	  else
	    if ( Datadevtype == 3 )
	      printf("LoadIOerrs-Lin-CSV-TC:  device type is TAPE\n");
	    else
	      if ( Datadevtype == 4 )
		printf("LoadIOerrs-Lin-CSV-TC:  device type is NFS\n");
	      else
		if ( Datadevtype == 5 )
		  printf("LoadIOerrs-Lin-CSV-TC:  device type is MAPPER\n");
		else
		  if ( Datadevtype == 4 )
		    printf("LoadIOerrs-Lin-CSV-TC:  device type is LIN_VERITAS\n");
		  else
		    printf("LoadIOerrs-Lin-CSV-TC:  device type is UNKNOWN\n");
#endif
      

      fprintf(OutputFILEstats,"%ld,%d,%s,%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d\n",
	      Key1,
	      DataServerID, DataColDate, (int)DataEpoch,
	      Datadevice, Datarrqms, Datawrqms, Datars, Dataws,
	      Datakrs, Datakws, Dataavgrqsz, Dataavgqusz, Dataawait,
	      Datasvctm, Datautil,
	      Dataavgread, Dataavgwrit, Datadevtype);
      Key1++;


      ReadLine2(Line,IOstatFileDes,&Status);
    }  /*  end while processing chunk of data  */

    /*  done processing a chunk of data - update epoch seconds  */

#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:  About to add %lf to %lf --> ",DataEpoch, EpochInc);
#endif

    DataEpoch = DataEpoch + EpochInc;

#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:  DataEpoch:  %lf\n",DataEpoch);
#endif

    /*  and burn the second header line  */

    ReadLine2(Line,IOstatFileDes,&Status);
    ReadLine2(Line,IOstatFileDes,&Status);


#ifdef DEBUG
    printf("LoadIOerrs-Lin-CSV-TC:  Bot of while --> %s\n",Line);
#endif
  }  /*  while not EOF  */



  /*
    clean up and go home
  */

  close(IOstatFileDes);
  fclose(OutputFILEstats);

}
