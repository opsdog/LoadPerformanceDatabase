/*

  reads a file of

    kstat -n 'p[c,x]*' -s '*fail' -T u XX  

  and parses it into CSV to be loaded into a mysql table

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

  int VMEMFileDes;
  int Status;
  int EpochCount;
  long int Key1;

  char VMEMInputFileName[512];
  char Token[256];
  char Line[1024];
  char FailType[25];
  /*  char FailCount[10];*/

  char Junk[1024];

  int DataServerID;
  char DataColDate[20];
  int DataEpoch;

  char DataModule[15];
  char DataInstance[6];
  char DataName[30];
  char DataClass[30];
  char DataCommitFail[12];
  char DataReserveFail[12];
  char DataAllocFail[12];
  char DataFail[12];
  char DataPopulateFail[12];

  FILE *OutputFILE;

#ifdef DEBUG
  printf("LoadVMEM-CSV-TC:  entry\n");
  printf("LoadVMEM-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 5) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate NextKey\n", argv[0]);
    return(1);
  }
  strcpy(VMEMInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%ld",&Key1);

#ifdef DEBUG
  printf("LoadVMEM-CSV-TC:  zeroing fail counts\n");
#endif
  DataCommitFail[0] = '0'; DataCommitFail[1] = '\0';
  DataReserveFail[0] = '0'; DataReserveFail[1] = '\0';
  DataAllocFail[0] = '0'; DataAllocFail[1] = '\0';
  DataFail[0] = '0'; DataFail[1] = '\0';
  DataPopulateFail[0] = '0'; DataPopulateFail[1] = '\0';

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                    process the VMEM file                         */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( VMEMFileDes = open (VMEMInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",VMEMInputFileName, errno);
    return(errno);
  }

  if (( OutputFILE = fopen("VMEMLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of VMEMLoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadVMEM-CSV-TC:  args processed\n");
  printf("LoadVMEM-CSV-TC:  input file open\n");
  printf("LoadVMEM-CSV-TC:  output file open\n");
#endif

  EpochCount = 0;
  EndOfFile = 0;

  ReadLine2(Line,VMEMFileDes,&Status);
  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("\n\nLoadVMEM-CSV-TC:  Top of read while\n");
    printf("LoadVMEM-CSV-TC:  Line -->%s<--\n",Line);
#endif

    /*  first line should be epoch timestamp  */

    sscanf(Line,"%d",&DataEpoch);

    ReadLine2(Line,VMEMFileDes,&Status);
    while ( ( Line[0] != '1' ) & ( !EndOfFile ) ) {

      while ( strlen(Line) != 0 ) {

#ifdef DEBUG
	printf("LoadVMEM-CSV-TC:  Epoch:  %d\n",DataEpoch);
#endif

	/*  next line is always labeled module and instance  */

#ifdef DUMPDATA
	printf("LoadVMEM-CSV-TC:  Line          -->%s<--\n",Line);
#endif
	StripDelim(' ',Line);
#ifdef DUMPDATA
	printf("LoadVMEM-CSV-TC:  Stripped Line -->%s<--\n",Line);
#endif

	strcpy(Junk,GetNextToken(' ',Line));
	strcpy(DataModule,GetNextToken(' ',Line));

#ifdef DUMPDATA
	printf("LoadVMEM-CSV-TC:  Line          -->%s<--\n",Line);
#endif

	strcpy(Junk,GetNextToken(' ',Line));
	strcpy(DataInstance,GetNextToken(' ',Line));

    /*  next line is always labeled name and class  */

	ReadLine2(Line,VMEMFileDes,&Status);
#ifdef DUMPDATA
	printf("LoadVMEM-CSV-TC:  Line          -->%s<--\n",Line);
#endif
	StripDelim(' ',Line);
#ifdef DUMPDATA
	printf("LoadVMEM-CSV-TC:  Stripped Line -->%s<--\n",Line);
#endif
	strcpy(Junk,GetNextToken(' ',Line));
	strcpy(DataName,GetNextToken(' ',Line));

#ifdef DUMPDATA
	printf("LoadVMEM-CSV-TC:  Line          -->%s<--\n",Line);
#endif
	strcpy(Junk,GetNextToken(' ',Line));
	strcpy(DataClass,GetNextToken(' ',Line));

#ifdef DEBUG
	printf("LoadVMEM-CSV-TC: ----------------------------------------------------------------\n");
	printf("LoadVMEM-CSV-TC:  DataEpoch\tDataModule\tDataInstance\tDataName\tDataClass\n");
	printf("LoadVMEM-CSV-TC:  %10d\t%10s\t%10s\t%10s\t%10s\n",
	       DataEpoch, DataModule, DataInstance, DataName, DataClass);
	printf("LoadVMEM-CSV-TC: ----------------------------------------------------------------\n");
#endif    

	/*  now we get to the fail data that is inconsistent for each entry  */

	ReadLine2(Line,VMEMFileDes,&Status);
	while ( strlen(Line) != 0 ) {

#ifdef DEBUG
	  printf("LoadVMEM-CSV-TC:  entering loop to read a data block:\n");
	  printf("LoadVMEM-CSV-TC:  Line:  -->%s<--\n",Line);
#endif

	  /*  next token will be type of fail  */

	  StripDelim(' ',Line);
#ifdef DEBUG
	  printf("LoadVMEM-CSV-TC:  Stripped Line:  -->%s<--\n",Line);
#endif
	  strcpy(FailType,GetNextToken(' ',Line));

#ifdef DEBUG
	  printf("LoadVMEM-CSV-TC:  fail type -->%s<--\n", FailType);
#endif

	  /*  determine type of fail and generate SQL here  */

	  if ( strcmp("commit_fail",FailType) == 0 )
	    strcpy(DataCommitFail,GetNextToken(' ',Line));
	  else if ( strcmp("reserve_fail",FailType) == 0 )
	    strcpy(DataReserveFail,GetNextToken(' ',Line));
	  else if ( strcmp("alloc_fail",FailType) == 0 )
	    strcpy(DataAllocFail,GetNextToken(' ',Line));
	  else if ( strcmp("fail",FailType) == 0 )
	    strcpy(DataFail,GetNextToken(' ',Line));
	  else if ( strcmp("populate_fail",FailType) == 0 )
	    strcpy(DataPopulateFail,GetNextToken(' ',Line));
	  else
	    { printf("\n\nUNKNOWN FAIL TYPE:  %s\n\n",FailType); exit(1); }



	  ReadLine2(Line,VMEMFileDes,&Status);

	}  /*  while reading a fail data block  */


	/*
	fprintf(OutputFILE,"insert into VMEMfail (pkey, serverid, datestr, esttime, module, instance, name, class, commit_fail, reserve_fail, alloc_fail, fail, populate_fail) values (%d, %d, '%s', %d, '%s', '%s', '%s', '%s', %s, %s, %s, %s, %s);\n",
		Key1, DataServerID, DataColDate, DataEpoch, DataModule, 
		DataInstance, DataName, DataClass, DataCommitFail, 
		DataReserveFail, DataAllocFail, DataFail, DataPopulateFail);
	*/

	fprintf(OutputFILE,"%d,%d,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
		Key1, DataServerID, DataColDate, DataEpoch, DataModule, 
		DataInstance, DataName, DataClass, DataCommitFail, 
		DataReserveFail, DataAllocFail, DataFail, DataPopulateFail);

	  Key1 ++;

#ifdef DEBUG
	  printf("LoadVMEM-CSV-TC:  zeroing fail counts\n");
#endif
	  DataCommitFail[0] = '0'; DataCommitFail[1] = '\0';
	  DataReserveFail[0] = '0'; DataReserveFail[1] = '\0';
	  DataAllocFail[0] = '0'; DataAllocFail[1] = '\0';
	  DataFail[0] = '0'; DataFail[1] = '\0';
	  DataPopulateFail[0] = '0'; DataPopulateFail[1] = '\0';



	ReadLine2(Line,VMEMFileDes,&Status);
      }  



      ReadLine2(Line,VMEMFileDes,&Status);
    }

#ifdef DEBUG
    printf("LoadVMEM-CSV-TC:  Ready to read next collection:\n");
    printf("LoadVMEM-CSV-TC:  Line:  -->%s<--\n",Line);
#endif


    if ( strlen(Line) == 0 )
      ReadLine2(Line,VMEMFileDes,&Status);
  }  /*  while reading VMEM input file */

#ifdef DEBUG
  printf("LoadVMEM-CSV-TC:  reached EOF\n");
#endif


  /*  clean up and go home */

  fclose(OutputFILE);
  close(VMEMFileDes);

}
