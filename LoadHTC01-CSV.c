/*

  reads the HTC-METRIC.csv files into a single CSV to load into the DB

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

int main(int argc, char *argv[]) {

  long int Key1;

  int RRespFileDes;
  int WRespFileDes;
  int RTRateFileDes;
  int WTRateFileDes;
  int RIOPsFileDes;
  int WIOPsFileDes;
  int Status;

  float DataRResp;
  float DataWResp;
  long int DataArrayNum;
  int DataRTRate;
  int DataWTRate;
  int DataRIOPs;
  int DataWIOPs;
  int DataServerID = -1;
  char DataLDEV[6];
  char DataEsttime[15];

  char RRespLine[4096];
  char WRespLine[4096];
  char RTRateLine[4096];
  char WTRateLine[4096];
  char RIOPsLine[4096];
  char WIOPsLine[4096];
  char Token[256];

  FILE *OutputCSV;



#ifdef DEBUG
  printf("LoadHTC01-CSV:  entry\n");
  printf("LoadHTC01-CSV:  %d args\n", argc);
#endif

  if ( argc != 2) {
    printf("Usage:  %s NextKey \n", argv[0]);
    return(1);
  }
  sscanf(argv[1],"%ld",&Key1);

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*              process the HTC-METRIC.csv files                    */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( RRespFileDes = open ("HTC-RRespLoad.csv", O_RDONLY) ) == -1 ) {
    printf("Open of HTC-RRespLoad.csv failed:  %d\n",errno);
    return(errno);
  }
  if (( RTRateFileDes = open ("HTC-RTRateLoad.csv", O_RDONLY) ) == -1 ) {
    printf("Open of HTC-RTRateLoad.csv failed:  %d\n",errno);
    return(errno);
  }
  if (( RIOPsFileDes = open ("HTC-RIOPsLoad.csv", O_RDONLY) ) == -1 ) {
    printf("Open of HTC-RIOPsLoad.csv failed:  %d\n",errno);
    return(errno);
  }
  if (( WRespFileDes = open ("HTC-WRespLoad.csv", O_RDONLY) ) == -1 ) {
    printf("Open of HTC-WRespLoad.csv failed:  %d\n",errno);
    return(errno);
  }
  if (( WTRateFileDes = open ("HTC-WTRateLoad.csv", O_RDONLY) ) == -1 ) {
    printf("Open of HTC-WTRateLoad.csv failed:  %d\n",errno);
    return(errno);
  }
  if (( WIOPsFileDes = open ("HTC-WIOPsLoad.csv", O_RDONLY) ) == -1 ) {
    printf("Open of HTC-WIOPsLoad.csv failed:  %d\n",errno);
    return(errno);
  }

  if (( OutputCSV = fopen("HTC-FullLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of HTC-FullLoad.csv failed:  %d\n", errno);
    return(errno);
  }

  /*
      open each of the HTC-METRIC.csv files

      read each line and combine into a single CSV
  */

  ReadLine4(RRespLine,RRespFileDes,&Status);
  while ( !EndOfFile ) {

    /*  pull the array, LDEV, and timestamp from RResp  */

#ifdef DUMPDATA
    printf("1st line of RResp --> %s\n",RRespLine);
#endif

    strcpy(Token,GetNextToken(',',RRespLine));
    sscanf(Token,"%ld",&DataArrayNum);
#ifdef DUMPDATA
    printf("  should be arraynum --> %s (%ld)\n",Token,DataArrayNum);
#endif

    strcpy(DataLDEV,GetNextToken(',',RRespLine));
#ifdef DUMPDATA
    printf("  should be ldev     --> %s\n",DataLDEV);
#endif

    strcpy(DataEsttime,GetNextToken(',',RRespLine));
#ifdef DUMPDATA
    printf("  should be esttime  --> %s\n",DataEsttime);
#endif

    strcpy(Token,GetNextToken(',',RRespLine));
    sscanf(Token,"%f",&DataRResp);
    DataRResp = DataRResp / 1000.0;
#ifdef DUMPDATA
    printf("  should be rresp    --> %f\n",DataRResp);
#endif

    /*  get RTRate  */

    ReadLine4(RTRateLine,RTRateFileDes,&Status);

#ifdef DUMPDATA
    printf("\n1st line of RTRate --> %s\n",RTRateLine);
#endif

    strcpy(Token,GetNextToken(',',RTRateLine));
    strcpy(Token,GetNextToken(',',RTRateLine));
    strcpy(Token,GetNextToken(',',RTRateLine));

    strcpy(Token,GetNextToken(',',RTRateLine));
    sscanf(Token,"%d",&DataRTRate);
#ifdef DUMPDATA
    printf("  should be rtrate   --> %d\n",DataRTRate);
#endif

    /*  get RIOPs  */

    ReadLine4(RIOPsLine,RIOPsFileDes,&Status);

#ifdef DUMPDATA
    printf("\n1st line of RIOPs  --> %s\n",RIOPsLine);
#endif

    strcpy(Token,GetNextToken(',',RIOPsLine));
    strcpy(Token,GetNextToken(',',RIOPsLine));
    strcpy(Token,GetNextToken(',',RIOPsLine));

    strcpy(Token,GetNextToken(',',RIOPsLine));
    sscanf(Token,"%d",&DataRIOPs);
#ifdef DUMPDATA
    printf("  should be riops    --> %d\n",DataRIOPs);
#endif

    /*  get WResp  */

    ReadLine4(WRespLine,WRespFileDes,&Status);

#ifdef DUMPDATA
    printf("\n1st line of WResp  --> %s\n",WRespLine);
#endif

    strcpy(Token,GetNextToken(',',WRespLine));
    strcpy(Token,GetNextToken(',',WRespLine));
    strcpy(Token,GetNextToken(',',WRespLine));

    strcpy(Token,GetNextToken(',',WRespLine));
    sscanf(Token,"%f",&DataWResp);
    DataWResp = DataWResp / 1000.0;
#ifdef DUMPDATA
    printf("  should be wresp    --> %f\n",DataWResp);
#endif

    /*  get WTRate  */

    ReadLine4(WTRateLine,WTRateFileDes,&Status);

#ifdef DUMPDATA
    printf("\n1st line of WTRate --> %s\n",WTRateLine);
#endif

    strcpy(Token,GetNextToken(',',WTRateLine));
    strcpy(Token,GetNextToken(',',WTRateLine));
    strcpy(Token,GetNextToken(',',WTRateLine));

    strcpy(Token,GetNextToken(',',WTRateLine));
    sscanf(Token,"%d",&DataWTRate);
#ifdef DUMPDATA
    printf("  should be wtrate   --> %d\n",DataWTRate);
#endif

    /*  get WIOPs  */

    ReadLine4(WIOPsLine,WIOPsFileDes,&Status);

#ifdef DUMPDATA
    printf("\n1st line of WIOPs  --> %s\n",WIOPsLine);
#endif

    strcpy(Token,GetNextToken(',',WIOPsLine));
    strcpy(Token,GetNextToken(',',WIOPsLine));
    strcpy(Token,GetNextToken(',',WIOPsLine));

    strcpy(Token,GetNextToken(',',WIOPsLine));
    sscanf(Token,"%d",&DataWIOPs);
#ifdef DUMPDATA
    printf("  should be wiops    --> %d\n",DataWIOPs);
#endif

    /*  printf("\n\nShould be line of CSV to load:\n");  */
    fprintf(OutputCSV, "%d,%d,%d,%s,%s,%f,%f,%d,%d,%d,%d\n",
	   Key1, DataServerID, DataArrayNum, DataLDEV, DataEsttime,
	   DataRResp, DataWResp, DataRTRate, DataWTRate, DataRIOPs, DataWIOPs);


    /*  set up next iteration  */

    Key1++;
    ReadLine4(RRespLine,RRespFileDes,&Status);
  }  /*  while not EOF  */





  /*  clean up and go home  */

  close(RRespFileDes);
  close(RTRateFileDes);
  close(RIOPsFileDes);
  close(WRespFileDes);
  close(WTRateFileDes);
  close(WIOPsFileDes);

  fclose(OutputCSV);

#ifdef DEBUG
  printf("\n");
  printf("\n");
  printf("LoadHTC01-CSV:  Ending stats:\n");

#endif


}
