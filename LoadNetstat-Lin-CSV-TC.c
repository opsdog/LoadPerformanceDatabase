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
  int EpochStart;
  /*
  int EpochEnd,
    EpochSpan,
    EpochCount;
  */

  char DataColDate[20];
  char NSInputFileName[512];
  char Token[256];
  char Line[1024];

  /*  the data from netstat  */

  long int DataIpack;
  long int DataIerrs;
  long int DataIdrop;
  long int DataIovr;
  long int DataOpack;
  long int DataOerrs;
  long int DataOdrop;
  long int DataOovr;

  long int PrevIpack;
  long int PrevIerrs;
  long int PrevIdrop;
  long int PrevIovr;
  long int PrevOpack;
  long int PrevOerrs;
  long int PrevOdrop;
  long int PrevOovr;

  long int ReadIpack;
  long int ReadIerrs;
  long int ReadIdrop;
  long int ReadIovr;
  long int ReadOpack;
  long int ReadOerrs;
  long int ReadOdrop;
  long int ReadOovr;

  /*  
  long int DataTIpack;
  long int DataTIerrs;
  long int DataTOpack;
  long int DataTOerrs;
  long int DataTOcoll;
  */

  /*
  float DataPCTipack;
  float DataPCTierrs;
  float DataPCTopack;
  float DataPCToerrs;
  float DataPCTocoll;
  */

  char DataIntf[15];
  char datestr[20];

  char strJunk1[25];
  char strJunk2[25];
  char strJunk3[25];
  char strJunk4[25];

  FILE *OutputFILEstats;


  /* arg checking */

#ifdef DEBUG
  printf("LoadNetstat-Lin-CSV-TC:  entry\n");
  printf("LoadNetstat-Lin-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 7) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate Interface EpochStart NextKey1\n", argv[0]);
    return(1);
  }
  strcpy(NSInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%s",&DataIntf);
  sscanf(argv[5],"%d",&EpochStart);
  /*
  sscanf(argv[5],"%d",&EpochEnd);
  sscanf(argv[6],"%d",&EpochCount);
  */
  sscanf(argv[6],"%ld",&Key1);
  DataEpoch = (double)EpochStart;

  /*
  EpochSpan = EpochEnd - EpochStart;
  EpochCount = EpochCount--;
  */

  /*
  EpochInc = (double)EpochSpan / (double)EpochCount;
  */

#ifdef DEBUG
  printf("\n\n");
  printf("\tServerID   = %d\n",DataServerID);
  printf("\tCol Date   = %s\n",DataColDate);
  printf("\tEpochStart = %d\n",EpochStart);
  /*
  printf("\tEpochEnd   = %d\n",EpochEnd);
  printf("\tEpochSpan  = %d\n",EpochSpan);
  printf("\tEpochCount = %d\n",EpochCount);
  printf("\tEpochInc   = %lf\n",EpochInc);
  */
  printf("\tDataIntf   = %s\n",DataIntf);
  printf("\n\tDataEpoch  = %lf\n",DataEpoch);
  printf("\n\n");
  printf("LoadNetstat-Lin-CSV-TC:  args processed\n");
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

  if (( OutputFILEstats = fopen("Netstat-ILoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of Netstat-ILoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadNetstat-Lin-CSV-TC:  input and output files open\n");
#endif

  /*
    the first line is only useful to get the starting values

    since the entries are cumulative, you have to subract the previous
    value from the current to get the interval count.

    from the first line, set the previous values

    also have to increment the Epoch by 15 seconds
  */

  ReadLine2(Line,NetstatFileDes,&Status);
  DataEpoch = DataEpoch + 15.0;

  sscanf(Line,"%s %s %s %ld %ld %ld %ld %ld %ld %ld %ld %s",
	 strJunk1, strJunk2, strJunk3, 
	 &PrevIpack, &PrevIerrs, &PrevIdrop, &PrevIovr, 
	 &PrevOpack, &PrevOerrs, &PrevOdrop, &PrevOovr, 
	 strJunk4);

#ifdef DEBUG
  printf("LoadNetstat-Lin-CSV-TC:  seed previous values read\n");
  printf("LoadNetstat-Lin-CSV-TC:    PrevIpack : %ld\n", PrevIpack);
  printf("LoadNetstat-Lin-CSV-TC:    PrevIerrs : %ld\n", PrevIerrs);
  printf("LoadNetstat-Lin-CSV-TC:    PrevIdrop : %ld\n", PrevIdrop);
  printf("LoadNetstat-Lin-CSV-TC:    PrevIovr  : %ld\n", PrevIovr);
  printf("LoadNetstat-Lin-CSV-TC:    PrevOpack : %ld\n", PrevOpack);
  printf("LoadNetstat-Lin-CSV-TC:    PrevOerrs : %ld\n", PrevOerrs);
  printf("LoadNetstat-Lin-CSV-TC:    PrevOdrop : %ld\n", PrevOdrop);
  printf("LoadNetstat-Lin-CSV-TC:    PrevOovr  : %ld\n", PrevOovr);
#endif  

  /*  read the next line of data and process the file  */

  ReadLine2(Line,NetstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadNetstat-Lin-CSV-TC:  processing real data with --> %s\n",Line);
#endif

  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadNetstat-Lin-CSV-TC:  Top of while --> (%d) %s\n",strlen(Line),Line);
    /*    printf("LoadNetstat-Lin-CSV-TC:    EpochInc:  %lf\n",EpochInc);*/
#endif

    sscanf(Line,"%s %s %s %ld %ld %ld %ld %ld %ld %ld %ld %s",
	   strJunk1, strJunk2, strJunk3, 
	   &ReadIpack, &ReadIerrs, &ReadIdrop, &ReadIovr, 
	   &ReadOpack, &ReadOerrs, &ReadOdrop, &ReadOovr, 
	   strJunk4);

#ifdef DEBUG
    printf("LoadNetstat-Lin-CSV-TC:    Read values:  %ld %ld %ld %ld - %ld %ld %ld %ld\n",
	   ReadIpack, ReadIerrs, ReadIdrop, ReadIovr, 
	   ReadOpack, ReadOerrs, ReadOdrop, ReadOovr);
#endif

    /*  
	because it's all cumulative data, have to subtract the previous
	value from the current across the board
    */

    DataIpack = ReadIpack - PrevIpack;
    DataIerrs = ReadIerrs - PrevIerrs;
    DataIdrop = ReadIdrop - PrevIdrop;
    DataIovr = ReadIovr - PrevIovr;
    DataOpack = ReadOpack - PrevOpack;
    DataOerrs = ReadOerrs - PrevOerrs;
    DataOdrop = ReadOdrop - PrevOdrop;
    DataOovr = ReadOovr - PrevOovr;

#ifdef DEBUG
    printf("LoadNetstat-Lin-CSV-TC:    Calculated values:  %ld %ld %ld %ld - %ld %ld %ld %ld\n",
	   DataIpack, DataIerrs, DataIdrop, DataIovr, 
	   DataOpack, DataOerrs, DataOdrop, DataOovr);
    printf("LoadNetstat-Lin-CSV-TC:    DataEpoch:  %lf\n", DataEpoch);
#endif



    fprintf(OutputFILEstats,
	    "%ld,%d,%s,%ld,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
	    Key1, DataServerID, DataColDate, (long int)DataEpoch,
	    DataIntf,
	    DataIpack, DataIerrs, DataIdrop, DataIovr, 
	    DataOpack, DataOerrs, DataOdrop, DataOovr);


#ifdef DEBUG
    printf("LoadNetstat-Lin-CSV-TC:    Wrote CSV line\n");
    /*    printf("LoadNetstat-Lin-CSV-TC:    About to add %lf to %lf --> ",DataEpoch, EpochInc);*/
#endif
    DataEpoch = DataEpoch + 15;
#ifdef DEBUG
    printf("LoadNetstat-Lin-CSV-TC:    Next Epoch:  %lf\n",DataEpoch);
#endif

    /*  set new previous values  */

    PrevIpack = ReadIpack;
    PrevIerrs = ReadIerrs;
    PrevIdrop = ReadIdrop;
    PrevIovr = ReadIovr;
    PrevOpack = ReadOpack;
    PrevOerrs = ReadOerrs;
    PrevOdrop = ReadOdrop;
    PrevOovr = ReadOovr;



    Key1++;
    ReadLine2(Line,NetstatFileDes,&Status);
  }  /*  while reading each line of the file  */


  close(NetstatFileDes);
  fclose(OutputFILEstats);

}
