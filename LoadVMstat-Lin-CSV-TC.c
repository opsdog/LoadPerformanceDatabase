/*

  reads a vmstat -S file (argv[1]), parses it, and loads it into
  a CSV to be loaded into the DB

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

#include <TokenParse.h>

int main(int argc, char *argv[]) {

  double DataEpoch;
  double EpochInc;

  long int Key1;

  int InputFileDes;
  int Status;
  int DataServerID;
  int i;
  int EpochStart,
    EpochEnd,
    EpochSpan,
    EpochCount;

  char DataColDate[20];
  char InputFileName[512];
  char Token[256];
  char Line[1024];

  /*  the data from vmstat anSK  */

  long int Datar;
  long int Datab;
  long int Dataswpd;
  long int Datafree;
  long int Datainact;
  long int Dataactive;
  long int Datasi;
  long int Dataso;
  long int Databi;
  long int Databo;
  long int Dataiin;
  long int Datacs;
  long int Dataus;
  long int Datasy;
  long int Dataid;
  long int Datawa;
  long int Datast;

  FILE *OutputFILEstats;

#ifdef DEBUG
  printf("LoadVMstat-Lin-CSV-TC:  entry\n");
  printf("LoadVMstat-Lin-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 8) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate EpochStart EpochEnd EpochCount NextKey1\n", argv[0]);
    return(1);
  }
  strcpy(InputFileName,argv[1]);
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
  printf("LoadVMstat-Lin-CSV-TC:  Variables:\n");
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
  printf("LoadVMstat-Lin-CSV-TC:  args processed\n");
#endif

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                  process the vmstat file                         */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  EndOfFile = 0;

  if (( InputFileDes = open (InputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",InputFileName, errno);
    return(errno);
  }

  if (( OutputFILEstats = fopen("VMstat-SLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of VMstat-SLoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadVMstat-Lin-CSV-TC:  input and output files open\n");
#endif

  /*  burn the first 2 header lines  */

  ReadLine2(Line,InputFileDes,&Status);
  ReadLine2(Line,InputFileDes,&Status);

  /*  burn the cumulative line  */

  ReadLine2(Line,InputFileDes,&Status);

#ifdef DEBUG
  printf("LoadVMstat-Lin-CSV-TC:  headers and cumulative lines discarded\n");
#endif  

  /*  read the first line of data and process the file  */

  ReadLine2(Line,InputFileDes,&Status);

#ifdef DEBUG
  printf("LoadVMstat-Lin-CSV-TC:  processing real data with --> %s\n",Line);
#endif

  /*  while ( !EndOfFile && ( Line[1] != 'k' || Line[1] != 'r' ) ) {  */
  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadVMstat-Lin-CSV-TC:  Top of while --> (%d) %s\n",strlen(Line),Line);
    printf("LoadVMstat-Lin-CSV-TC:    EpochInc:  %lf\n",EpochInc);
#endif

    if ( ( Line[0] != 'p' && Line[1] != 'r' ) ) {

      sscanf(Line,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	     &Datar, &Datab, &Dataswpd, &Datafree, &Datainact, &Dataactive, 
	     &Datasi, &Dataso, &Databi, &Databo, &Dataiin, &Datacs, &Dataus, 
	     &Datasy, &Dataid, &Datawa, &Datast);

#ifdef DEBUG
      printf("LoadVMstat-Lin-CSV-TC:    Read values:  %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
	     Datar, Datab, Dataswpd, Datafree, Datainact, Dataactive, 
	     Datasi, Dataso, Databi, Databo, Dataiin, Datacs, Dataus, 
	     Datasy, Dataid, Datawa, Datast);

      printf("LoadVMstat-Lin-CSV-TC:    DataEpoch:  %lf\n", DataEpoch);
#endif

      fprintf(OutputFILEstats,"%ld,%d,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
	      Key1, DataServerID, DataColDate, (long int)DataEpoch,
	      Datar, Datab, Dataswpd, Datafree, Datainact, Dataactive, 
	      Datasi, Dataso, Databi, Databo, Dataiin, Datacs, Dataus, 
	      Datasy, Dataid, Datawa, Datast);

#ifdef DEBUG
      printf("LoadVMstat-Lin-CSV-TC:    Wrote CSV\n");
      printf("LoadVMstat-Lin-CSV-TC:    About to add %lf to %lf --> ",DataEpoch, EpochInc);
#endif
      DataEpoch = DataEpoch + EpochInc;
#ifdef DEBUG
      printf("%lf\n",DataEpoch);
#endif
      Key1++;

    }  /*  if not a header line  */


    ReadLine2(Line,InputFileDes,&Status);
  }  /*  while reading each line of the file  */

  /*  clean up and go home  */

  close(InputFileDes);
  fclose(OutputFILEstats);

}
