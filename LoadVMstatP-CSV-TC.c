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

  /*  the data from vmstat S  */

  long int Datapswap;
  long int Datapfree;
  long int Datapre;
  long int Datapmf;
  long int Datapfr;
  long int Datapde;
  long int Datapsr;
  long int Datapepi;
  long int Datapepo;
  long int Datapepf;
  long int Datapapi;
  long int Datapapo;
  long int Datapapf;
  long int Datapfpi;
  long int Datapfpo;
  long int Datapfpf;

  FILE *OutputFILEstats;

#ifdef DEBUG
  printf("LoadVMstatP-CSV-TC:  entry\n");
  printf("LoadVMstatP-CSV-TC:  %d args\n", argc);
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
  printf("LoadVMstatP-CSV-TC:  Variables:\n");
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
  printf("LoadVMstatP-CSV-TC:  args processed\n");
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

  if (( OutputFILEstats = fopen("VMstat-PLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of VMstat-PLoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadVMstatP-CSV-TC:  input and output files open\n");
#endif

  /*  burn the first 2 header lines  */

  ReadLine2(Line,InputFileDes,&Status);
  ReadLine2(Line,InputFileDes,&Status);

  /*  burn the cumulative line  */

  ReadLine2(Line,InputFileDes,&Status);

#ifdef DEBUG
  printf("LoadVMstatP-CSV-TC:  headers and cumulative lines discarded\n");
#endif  

  /*  read the first line of data and process the file  */

  ReadLine2(Line,InputFileDes,&Status);

#ifdef DEBUG
  printf("LoadVMstatP-CSV-TC:  processing real data with --> %s\n",Line);
#endif

  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadVMstatP-CSV-TC:  Top of while --> (%d) %s\n",strlen(Line),Line);
    printf("LoadVMstatP-CSV-TC:    EpochInc:  %lf\n",EpochInc);
#endif

    if ( Line[1] != ' ' ) {

      sscanf(Line,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",

	     &Datapswap, &Datapfree, &Datapre, &Datapmf, &Datapfr, &Datapde,
	     &Datapsr, &Datapepi, &Datapepo, &Datapepf, &Datapapi, &Datapapo,
	     &Datapapf, &Datapfpi, &Datapfpo, &Datapfpf);


#ifdef DEBUG
      printf("LoadVMstatP-CSV-TC:    Read values:  %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
	     Datapswap, Datapfree, Datapre, Datapmf, Datapfr, Datapde,
	     Datapsr, Datapepi, Datapepo, Datapepf, Datapapi, Datapapo,
	     Datapapf, Datapfpi, Datapfpo, Datapfpf);
      printf("LoadVMstatP-CSV-TC:    DataEpoch:  %lf\n", DataEpoch);
#endif

      fprintf(OutputFILEstats,"%ld,%d,%s,p,%ld,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
	      Key1, DataServerID, DataColDate, (long int)DataEpoch,
	     Datapswap, Datapfree, Datapre, Datapmf, Datapfr, Datapde,
	     Datapsr, Datapepi, Datapepo, Datapepf, Datapapi, Datapapo,
	     Datapapf, Datapfpi, Datapfpo, Datapfpf);

#ifdef DEBUG
      printf("LoadVMstatP-CSV-TC:    Wrote insert\n");
      printf("LoadVMstatP-CSV-TC:    About to add %lf to %lf --> ",DataEpoch, EpochInc);
#endif
      DataEpoch = DataEpoch + EpochInc;
#ifdef DEBUG
      printf("LoadVMstatP-CSV-TC:    Next Epoch:  %lf\n",DataEpoch);
#endif
      Key1++;

    }  /*  if not a header line  */


    ReadLine2(Line,InputFileDes,&Status);
  }  /*  while reading each line of the file  */

  /*  clean up and go home  */

  close(InputFileDes);
  fclose(OutputFILEstats);

}
