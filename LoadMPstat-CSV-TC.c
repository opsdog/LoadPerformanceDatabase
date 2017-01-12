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
  int Dataminf;
  int Datamjf;
  int Dataxcal;
  int Dataintr;
  int Dataithr;
  int Datacsw;
  int Dataicsw;
  int Datamigr;
  int Datasmtx;
  int Datasrw;
  int Datasyscl;
  int Datausr;
  int Datasys;
  int Datawt;
  int Dataidl;

  char DataColDate[20];

  char MPstatInputFileName[512];
  char Token[256];
  char Line[2014];

  FILE *OutputCSV;

#ifdef DEBUG
  printf("LoadMPstat-CSV-TC:  entry\n");
  printf("LoadMPstat-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 8) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate NextKey NumCollections EpochStart EpochEnd\n", argv[0]);
    return(1);
  }
  strcpy(MPstatInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%ld",&Key1);
  sscanf(argv[5],"%d",&NumCollections);
  sscanf(argv[6],"%d",&EpochStart);
  sscanf(argv[7],"%d",&EpochEnd);

  EpochSpan = EpochEnd - EpochStart;
  EpochInc = (double)EpochSpan / ( (double)NumCollections - 1.0 );
  DataEsttime = (double)EpochStart;

#ifdef DEBUG
  printf("LoadMPstat-CSV-TC:    server id %d starting %s\n", 
	 DataServerID, DataColDate);
  printf("LoadMPstat-CSV-TC:    from %d to %d\n",
	 EpochStart, EpochEnd);
  printf("LoadMPstat-CSV-TC:    %d collections\n", NumCollections);
  printf("LoadMPstat-CSV-TC:    first esttime %f", DataEsttime);
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

  /*
    burn the first collecton set
  */

  /*  first line is header - burn it  */

  ReadLine2(Line,MPstatFileDes,&Status);

  /*  loop until the next header  */

  ReadLine2(Line,MPstatFileDes,&Status);
  while ( ( Line[0] != 'C') & ( Line[1] != 'C' ) )
    ReadLine2(Line,MPstatFileDes,&Status);

  /*  out of burn loop - next line should be header  */

#ifdef DEBUG
  printf("\n");
  printf("LoadMPstat-CSV-TC:  out of burn loop\n");
  printf("LoadMPstat-CSV-TC:  this line should be header:\n");
  printf("LoadMPstat-CSV-TC:    -->%s<--\n",Line);
  printf("\n");
  printf("LoadMPstat-CSV-TC:  Starting stats:\n");
  printf("LoadMPstat-CSV-TC:  DataEsttime - EpochStart:  %d\n",
	 ((int)DataEsttime - EpochStart) );
  printf("LoadMPstat-CSV-TC:   StartEpoch    EndEpoch  EpochSpan Collections   EpochInc CalcEpochEnd   MissedBy\n");
  printf("LoadMPstat-CSV-TC:  %11d %11d %12d %11d %8.4f %10.2lf %10d\n",
	 EpochStart, EpochEnd, EpochSpan, NumCollections, EpochInc, DataEsttime,
	 (EpochEnd - (int)DataEsttime) );
  printf("\n");
#endif

  /*  enter the loop pointing at data...  */

  ReadLine2(Line,MPstatFileDes,&Status);

  while ( !EndOfFile ) {

    DataEsttime = DataEsttime + EpochInc;

    while ( (Line[0] != 'C') & ( Line[1] != 'C' ) & ( strlen(Line) != 0 ) ) {

#ifdef DUMPDATA
      printf("LoadMPstat-CSV-TC:    read collected data:  -->%s<--\n",Line);
#endif

      sscanf(Line," %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
	     &Datacpu, &Dataminf, &Datamjf, &Dataxcal, &Dataintr, &Dataithr, 
	     &Datacsw, &Dataicsw, &Datamigr, &Datasmtx, &Datasrw, &Datasyscl, 
	     &Datausr, &Datasys, &Datawt, &Dataidl);

      fprintf(OutputCSV,"%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	      Key1, DataServerID, DataColDate, (int)DataEsttime,
	      Datacpu, Dataminf, Datamjf, Dataxcal, Dataintr, Dataithr, 
	      Datacsw, Dataicsw, Datamigr, Datasmtx, Datasrw, Datasyscl, 
	      Datausr, Datasys, Datawt, Dataidl);
      Key1++;

#ifdef DUMPDATA
      printf("LoadMPstat-CSV-TC:     the collected data:  --> %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d<--\n",
	     Datacpu, Dataminf, Datamjf, Dataxcal, Dataintr, Dataithr, 
	     Datacsw, Dataicsw, Datamigr, Datasmtx, Datasrw, Datasyscl, 
	     Datausr, Datasys, Datawt, Dataidl);
#endif

        ReadLine2(Line,MPstatFileDes,&Status);
    }  /*  while reading a collection  */

#ifdef DEBUG
    printf("LoadMPstat-CSV-TC:  bottom of EOF loop - out of collection loop...\n");
    printf("LoadMPstat-CSV-TC:  what is this line?\n");
    printf("LoadMPstat-CSV-TC:    -->%s<--\n",Line);
#endif

    ReadLine2(Line,MPstatFileDes,&Status);
  }  /*  while not EOF  */

  /*  clean up and go home  */

#ifdef DEBUG
  printf("\n");
  printf("\n");
  printf("LoadMPstat-CSV-TC:  Ending stats:\n");
  printf("LoadMPstat-CSV-TC:  DataEsttime - EpochStart:  %f\n",
	 ((int)DataEsttime - EpochStart) );
  printf("LoadMPstat-CSV-TC:   StartEpoch    EndEpoch  EpochSpan Collections   EpochInc CalcEpochEnd   MissedBy\n");
  printf("LoadMPstat-CSV-TC:  %11d %11d %12d %11d %8.4f %10.2lf %10d\n",
	 EpochStart, EpochEnd, EpochSpan, NumCollections, EpochInc, DataEsttime,
	 (EpochEnd - (int)DataEsttime) );
  printf("\n");
#endif

  fclose(OutputCSV);
  close(MPstatFileDes);

}
