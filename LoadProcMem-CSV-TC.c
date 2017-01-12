/*

  reads a file of ProcByMEM output and parses it into CSV to be loaded
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

  long int Key1;

  int InputFileDes;
  int Status;

  int DataServerID;

  char DataDateStr[20];
  char LongEsttime[50];
  char DataEsttime[20];
  char DataOS[10];
  char DataOSVer[20];
  char DataPID[10];
  char DataMemory[25];
  char DataPmem[10];
  char DataPcpu[10];
  char DataNlwp[10];
  char DataProject[25];
  char DataCommand[1024];

  char Line[2048];
  char Junk[256];
  char InputFileName[1024];

  FILE *OutputCSV;

#ifdef DEBUG
  printf("LoadProcMem-CSV-TC:  entry\n");
  printf("LoadProcMem-CSV-TC:  %d args\n", argc);
#endif

  /*  arg checking  */

  if ( argc != 4 ) {
    printf("usage:  %s Key ServerID InputFileName\n", argv[0]);
    return(1);
  }

  sscanf(argv[1],"%ld",&Key1);
  sscanf(argv[2],"%d",&DataServerID);
  strcpy(InputFileName,argv[3]);

#ifdef DEBUG
  printf("LoadProcMem-CSV-TC:  Key: %ld\n", Key1);
  printf("LoadProcMem-CSV-TC:  ServerID: %d\n",DataServerID);
  printf("LoadProcMem-CSV-TC:  File: %s\n",InputFileName);
#endif


  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                  process the ProcByMEM file                      */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( InputFileDes = open (InputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",InputFileName, errno);
    return(errno);
  }

  if (( OutputCSV = fopen("ProcByMemLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of MPstatLoad.csv failed:  %d\n", errno);
    return(errno);
  }

  /*  no headers to burn here - first line is real data  */

  ReadLine2(Line,InputFileDes,&Status);

  while ( !EndOfFile ) {
#ifdef DUMPDATA
    printf("LoadProcMem-CSV-TC:    top of while - line is -->%s<--\n",Line);
#endif

    strcpy(Junk,GetNextToken(',',Line));
    strcpy(DataDateStr,GetNextToken(',',Line));
    strcpy(LongEsttime,GetNextToken(',',Line));
    strcpy(DataOS,GetNextToken(',',Line));
    strcpy(DataOSVer,GetNextToken(',',Line));
    strcpy(DataPID,GetNextToken(',',Line));
    strcpy(DataMemory,GetNextToken(',',Line));
    strcpy(DataPmem,GetNextToken(',',Line));
    strcpy(DataPcpu,GetNextToken(',',Line));
    strcpy(DataNlwp,GetNextToken(',',Line));
    strcpy(DataProject,GetNextToken(',',Line));
    strcpy(DataCommand,GetNextToken(',',Line));

    /*  convert the nanosecond esttime to second esttime  */

    memcpy(DataEsttime,LongEsttime,10);
    DataEsttime[10]='\0';

#ifdef DUMPDATA
    printf("\n");
    printf("LoadProcMem-CSV-TC:      datstr: %s\n",DataDateStr);
    printf("LoadProcMem-CSV-TC:      longesttime: %s\n",LongEsttime);
    printf("LoadProcMem-CSV-TC:      esttime: %s\n",DataEsttime);
    printf("LoadProcMem-CSV-TC:      os: %s\n",DataOS);
    printf("LoadProcMem-CSV-TC:      osver: %s\n",DataOSVer);
    printf("LoadProcMem-CSV-TC:      pid: %s\n",DataPID);
    printf("LoadProcMem-CSV-TC:      memory: %s\n",DataMemory);
    printf("LoadProcMem-CSV-TC:      pmem: %s\n",DataPmem);
    printf("LoadProcMem-CSV-TC:      pcpu: %s\n",DataPcpu);
    printf("LoadProcMem-CSV-TC:      nlwp: %s\n",DataNlwp);
    printf("LoadProcMem-CSV-TC:      project: %s\n",DataProject);
    printf("LoadProcMem-CSV-TC:      command: %s\n",DataCommand);
    printf("\n");
#endif


    if ( strcmp(DataCommand,"COMMAND") != 0 ) {
      fprintf(OutputCSV,"%ld,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
	      Key1, DataServerID, DataDateStr, DataEsttime,
	      DataOS, DataOSVer, DataPID, DataMemory, DataPmem, DataPcpu,
	      DataNlwp, DataProject, DataCommand);
      Key1++;
    }  /*  if not header  */

    ReadLine2(Line,InputFileDes,&Status);
  }  /*  while reading input file  */



  /*  clean up and go home  */

  close(InputFileDes);
  fclose(OutputCSV);

}
