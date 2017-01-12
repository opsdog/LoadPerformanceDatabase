/*

  reads a netstat -i file (argv[1]), parses it, and loads it into
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

  int NetstatFileDes;
  int Status;
  int DataServerID;
  int i;
  int EpochStart,
    EpochEnd,
    EpochSpan,
    EpochCount;

  char DataColDate[20];
  char NSInputFileName[512];
  char Token[256];
  char Line[1024];

  /*  the data from netstat  */

  long int DataIpack;
  long int DataIerrs;
  long int DataOpack;
  long int DataOerrs;
  long int DataOcoll;

  long int DataTIpack;
  long int DataTIerrs;
  long int DataTOpack;
  long int DataTOerrs;
  long int DataTOcoll;

  float DataPCTipack;
  float DataPCTierrs;
  float DataPCTopack;
  float DataPCToerrs;
  float DataPCTocoll;

  char DataIntf[15];
  char datestr[20];

  FILE *OutputFILEstats;


  /* arg checking */

#ifdef DEBUG
  printf("LoadNetstatI-CSV-TC:  entry\n");
  printf("LoadNetstatI-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 8) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate EpochStart EpochEnd EpochCount NextKey1\n", argv[0]);
    return(1);
  }
  strcpy(NSInputFileName,argv[1]);
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
  printf("LoadNetstatI-CSV-TC:  args processed\n");
#endif

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                  process the netstat file                        */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  EndOfFile = 0;

  if (( NetstatFileDes = open (NSInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",NSInputFileName, errno);
    return(errno);
  }

  if (( OutputFILEstats = fopen("Netstat-iLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of Netstat-iLoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadNetstatI-CSV-TC:  input and output files open\n");
#endif

  /*  the first header line contains the interface name  */

  ReadLine2(Line,NetstatFileDes,&Status);
  StripDelim(' ',Line);
  strcpy(DataIntf,GetNextToken(' ',Line));
  strcpy(DataIntf,GetNextToken(' ',Line));

#ifdef DEBUG
  printf("LoadNetstatI-CSV-TC:  Extracted interface name %s\n", DataIntf);
#endif

  /*  burn the second header line  */

  ReadLine2(Line,NetstatFileDes,&Status);

  /*  discard the cumulative line  */

  ReadLine2(Line,NetstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadNetstatI-CSV-TC:  headers and cumulative lines discarded\n");
#endif  

  /*  read the first line of data and process the file  */

  ReadLine2(Line,NetstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadNetstatI-CSV-TC:  processing real data with --> %s\n",Line);
#endif

  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadNetstatI-CSV-TC:  Top of while --> (%d) %s\n",strlen(Line),Line);
    printf("LoadNetstatI-CSV-TC:    EpochInc:  %lf\n",EpochInc);
#endif

    sscanf(Line,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
	   &DataIpack, &DataIerrs, &DataOpack, &DataOerrs, &DataOcoll,
	   &DataTIpack, &DataTIerrs, &DataTOpack, &DataTOerrs, &DataTOcoll);

#ifdef DEBUG
    printf("LoadNetstatI-CSV-TC:    Read values:  %ld %ld %ld %ld %ld - %ld %ld %ld %ld %ld\n",
	   DataIpack, DataIerrs, DataOpack, DataOerrs, DataOcoll,
	   DataTIpack, DataTIerrs, DataTOpack, DataTOerrs, DataTOcoll);
#endif

    if ( strlen(Line) < 80 ) {    /*  discard the stupidly big numbers  */

      /*  calculate the percent to total  */

      if ( DataTIpack != 0.0 )
	DataPCTipack = (float)DataIpack / (float)DataTIpack * 100.00;

      if ( DataTIerrs != 0 )
	DataPCTierrs = (float)DataIerrs / (float)DataTIerrs * 100.00;

      if ( DataTOpack != 0 )
	DataPCTopack = (float)DataOpack / (float)DataTOpack * 100.00;

      if ( DataTOerrs != 0 )
	DataPCToerrs = (float)DataOerrs / (float)DataTOerrs * 100.00;

      if ( DataTOcoll != 0 )
	DataPCTocoll = (float)DataOcoll / (float)DataTOcoll * 100.00;

#ifdef DEBUG
      printf("LoadNetstatI-CSV-TC:    Calc values:  %ld %ld %ld %ld %ld - %ld %ld %ld %ld %ld --> ",
	     DataIpack, DataIerrs, DataOpack, DataOerrs, DataOcoll,
	     DataTIpack, DataTIerrs, DataTOpack, DataTOerrs, DataTOcoll);
      printf("%f %f %f %f %f\n",
	     DataPCTipack, DataPCTierrs,
	     DataPCTopack, DataPCToerrs, DataPCTocoll);
      printf("LoadNetstatI-CSV-TC:    DataEpoch:  %lf\n", DataEpoch);
#endif


      fprintf(OutputFILEstats,
	      "%ld,%d,%s,%ld,%s,%ld,%ld,%ld,%ld,%ld,%f,%f,%f,%f,%f\n",
	      Key1, DataServerID, DataColDate, (long int)DataEpoch,
	      DataIntf,
	      DataIpack, DataIerrs, DataOpack, DataOerrs, DataOcoll,
	      DataPCTipack, DataPCTierrs, 
	      DataPCTopack, DataPCToerrs, DataPCTocoll);

#ifdef DEBUG
      printf("LoadNetstatI-CSV-TC:    Wrote insert\n");
      printf("LoadNetstatI-CSV-TC:    About to add %lf to %lf --> ",DataEpoch, EpochInc);
#endif

    }  /*  if not bogus entry  */



    DataEpoch = DataEpoch + EpochInc;
#ifdef DEBUG
    printf("LoadNetstatI-CSV-TC:    Next Epoch:  %lf\n",DataEpoch);
#endif

    Key1++;
    ReadLine2(Line,NetstatFileDes,&Status);
  }  /*  while reading each line of the file  */


  close(NetstatFileDes);
  fclose(OutputFILEstats);

}
