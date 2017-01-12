/*

  reads an iostat -xne file (argv[1]), parses it, and loads
  it into a pair of CSVs to be loaded into the DB

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

  double  DataEpoch;

  long int Key1;

  int IOstatFileDes;
  int Status;
  int DataServerID;
  int i;

  char DataColDate[20];
  char IOInputFileName[512];
  char Token[256];
  char Line[1024];

  /*  time_t  DataEpoch;*/

  /*  the data from iostat  */

  float Datars;
  float Dataws;
  float Datakrs;
  float Datakws;
  float Datarrqms;
  float Datawrqms;
  float Dataavgrqsz;
  float Dataavgqusz;
  float Dataawait;
  float Datasvctm;
  float Datautil;
  char Datadevice[51];

  /*  derived data  */

  float Dataavgread;
  float Dataavgwrit;
  int Datadevtype;

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

  FILE *OutputFILEstats;

  /* arg checking */

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  entry\n");
  printf("LoadLinuxIO-CSV-TC:  %d args\n", argc);
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

  EpochInc = (double)EpochSpan / (double)EpochCount;

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  args processed\n");
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

#ifdef DEBUG
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

  if (( OutputFILEstats = fopen("IOStatLoadLinux.csv","w") ) == (FILE *)NULL ) {
    printf("Open of IOStatLoadLinux.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  input and output files open\n");
#endif

  /*  discard header server info line  */

  ReadLine2(Line,IOstatFileDes,&Status);

  /*  discard blank line */

  ReadLine2(Line,IOstatFileDes,&Status);

  /*  discard cumulative header line  */

  ReadLine2(Line,IOstatFileDes,&Status);
#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  Header line is %d long\n", strlen(Line));
#endif


  /*  discard the cumulative lines  */

  ReadLine2(Line,IOstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  Should be %d real data --> %s\n",strlen(Line), Line);
#endif

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  About to burn cumulative entries...\n");
#endif
  /*  while ( strlen(Line) != 105 ) {  */
  while ( Line[0] != 'D' ) {
#ifdef DEBUG
    printf("LoadLinuxIO-CSV-TC:    burning --> (%d) %s\n",strlen(Line),Line);
#endif
    ReadLine2(Line,IOstatFileDes,&Status);
  }

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  out of cumulative burn loop --> %s\n", Line);
#endif

  /*  burn the next 2 header lines */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadLinuxIO-CSV-TC:  processing real data with --> %s\n",Line);
#endif

  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadLinuxIO-CSV-TC:  Top of while --> (%d) %s\n",strlen(Line),Line);
    printf("LoadLinuxIO-CSV-TC:  EpochInc:  %lf\n",EpochInc);
#endif
    
    while ( ( strlen(Line) != 77 ) && ( strlen(Line) != 0 ) ) {
#ifdef DEBUG
      printf("LoadLinuxIO-CSV-TC:    inner while --> processing line --> (%d) %s\n",
	     strlen(Line),Line);
#endif

      /*  process each data entry  */

      /*
      sscanf(Line," %f %f %f %f %f %f %f %f %d %d %d %d %d %d%s",
	     &Datars, &Dataws, &Datakrs, &Datakws, &Datawait, &Dataactv,
	     &Datawsvct, &Dataasvct, &Datapctw, &Datapctb, &Dataerrorssw, 
	     &Dataerrorshw, &Dataerrorstrn, &Dataerrorstot, Datadevice);
      */

      sscanf(Line,"%s %f %f %f %f %f %f %f %f %f %f %f",
	     Datadevice, &Datarrqms, &Datawrqms, &Datars, &Dataws,
	     &Datakrs, &Datakws, &Dataavgrqsz, &Dataavgqusz,
	     &Dataawait, &Datasvctm, &Datautil);

#ifdef DEBUG
      printf("%s %f %f %f %f %f %f %f %f %f %f %f\n",
	     Datadevice, Datarrqms, Datawrqms, Datars, Dataws,
	     Datakrs, Datakws, Dataavgrqsz, Dataavgqusz,
	     Dataawait, Datasvctm, Datautil);
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
      printf("LoadLinuxIO-CSV-TC:    start determine device type\n");
      printf("LoadLinuxIO-CSV-TC:      device name: %s\n",Datadevice);
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
	printf("LoadLinuxIO-CSV-TC:      %d %c\n",i, Datadevice[i]);
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
      printf("LoadLinuxIO-CSV-TC:      HasSlash : %d\n",HasSlash);
      printf("LoadLinuxIO-CSV-TC:      HasT     : %d\n",HasT);
      printf("LoadLinuxIO-CSV-TC:      indexT   : %d\n",indexT);
      printf("LoadLinuxIO-CSV-TC:      HasD     : %d\n",HasD);
      printf("LoadLinuxIO-CSV-TC:      indexD   : %d\n",indexD);
#endif

      /*
	NFS will have a : in the name
	tape will have a / in the name
	controller with have no t or d
	SAN will have both t and d
	internal will have both t and d "close" together
      */

      Datadevtype = 2;
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

#ifdef DEBUG
      if ( Datadevtype == 0 )
	printf("LoadLinuxIO-CSV-TC:    device type is CONTROLLER\n");
      else 
	if ( Datadevtype == 1 )
	  printf("LoadLinuxIO-CSV-TC:    device type is INT DISK\n");
	else
	  if ( Datadevtype == 2 )
	    printf("LoadLinuxIO-CSV-TC:    device type is SAN\n");
	  else
	    if ( Datadevtype == 3 )
	      printf("LoadLinuxIO-CSV-TC:    device type is TAPE\n");
	    else
	      if ( Datadevtype == 4 )
		printf("LoadLinuxIO-CSV-TC:    device type is NFS\n");		
	      else
		printf("LoadLinuxIO-CSV-TC:    device type is UNKNOWN\n");
#endif
      

      fprintf(OutputFILEstats,"%ld,%d,%s,%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d\n",
	      Key1,
	      DataServerID, DataColDate, (int)DataEpoch,
	      Datadevice, Datarrqms, Datawrqms, Datars, Dataws,
	      Datakrs, Datakws, Dataavgrqsz, Dataavgqusz,
	      Dataawait, Datasvctm, Datautil, 
	      Dataavgread, Dataavgwrit, Datadevtype);
      Key1++;

      ReadLine2(Line,IOstatFileDes,&Status);
    }  /*  end while processing chunk of data  */

    /*  done processing a chunk of data - update epoch seconds  */

#ifdef DEBUG
    printf("LoadLinuxIO-CSV-TC:  About to add %lf to %lf --> ",DataEpoch, EpochInc);
#endif

    DataEpoch = DataEpoch + EpochInc;

#ifdef DEBUG
    printf("LoadLinuxIO-CSV-TC:    %lf\n",DataEpoch);
#endif

    /*  and burn the second header line  */

    ReadLine2(Line,IOstatFileDes,&Status);
    ReadLine2(Line,IOstatFileDes,&Status);


#ifdef DEBUG
    printf("LoadLinuxIO-CSV-TC:  Bot of while --> %s\n",Line);
#endif


  }  /*  while not EOF  */



  /*
    clean up and go home
  */

  close(IOstatFileDes);
  fclose(OutputFILEstats);

}
