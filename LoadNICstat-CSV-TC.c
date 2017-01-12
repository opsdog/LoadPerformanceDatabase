/*

  reads an nicstat -xp file (argv[1]), parses it, and loads
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

#include <TokenParse.h>

int main(int argc, char *argv[]) {

  int  DataEpoch;

  long int Key1;

  int IOstatFileDes;
  int Status;
  int DataServerID;
  int i;

  char DataColDate[20];
  char IOInputFileName[512];
  char Token[256];
  char Line[1024];

  /*  the nicstat data  */

  float Datansrkb;
  float Datanswkb;
  float Datansrpk;
  float Datanswpk;
  float Datansutil;
  float Datanssat;
  float Datansierr;
  float Datansoerr;
  float Datanscoll;
  float Datansnocp;
  float Datansdefer;
  char Datansintf[1024];

  FILE *OutputFILEstats;

  /*  arg checking  */

#ifdef DEBUG
  printf("LoadNICstat-CSV-TC:  entry\n");
  printf("LoadNICstat-CSV-TC:  %d args\n", argc);
#endif

  if ( argc != 5) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate NextKey\n", argv[0]);
    return(1);
  }

  strcpy(IOInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%ld",&Key1);

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                  process the nicstat file                        */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

#ifdef DEBUG
  printf("LoadNICstat-CSV-TC:  args processed\n\n");
  printf("\tServerID   = %d\n",DataServerID);
  printf("\tCol Date   = %s\n",DataColDate);
  printf("\n\n");
#endif

  EndOfFile = 0;

  if (( IOstatFileDes = open (IOInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",IOInputFileName, errno);
    return(errno);
  }

  if (( OutputFILEstats = fopen("NICStatLoad.csv","w") ) == (FILE *)NULL ) {
    printf("Open of NICStatLoad.csv failed:  %d\n", errno);
    return(errno);
  }

#ifdef DEBUG
  printf("LoadNICstat-CSV-TC:  input and output files open\n");
#endif

  /*  discard the cumulative line  */

  ReadLine2(Line,IOstatFileDes,&Status);

#ifdef DEBUG
  printf("LoadNICstat-CSV-TC.c:  Discarding %d first line --> %s\n",strlen(Line), Line);
#endif

  /*  read the first line of real data  */

  ReadLine2(Line,IOstatFileDes,&Status);

  while ( !EndOfFile ) {
#ifdef DEBUG
    printf("LoadNICstat-CSV-TC:    Top of while --> (%d) %s\n",strlen(Line),Line);
#endif

    /*
    sscanf(Line,"%d:%s:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
	   &DataEpoch, Datansintf, &Datansrkb, &Datanswkb, &Datansrpk, &Datanswpk, &Datansutil,
	   &Datanssat, &Datansierr, &Datansoerr, &Datanscoll, &Datansnocp, &Datansdefer);
    */


    /*
      token read :-(
    */

    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%d",&DataEpoch);
    strcpy(Datansintf,GetNextToken(':',Line));
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansrkb);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datanswkb);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansrpk);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datanswpk);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansutil);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datanssat);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansierr);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansoerr);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datanscoll);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansnocp);
    strcpy(Token,GetNextToken(':',Line));
    sscanf(Token,"%f", &Datansdefer);



#ifdef DEBUG
    printf("LoadNICstat-CSV-TC:      %d:%s:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f\n",
	   DataEpoch, Datansintf, Datansrkb, Datanswkb, Datansrpk, Datanswpk, Datansutil,
	   Datanssat, Datansierr, Datansoerr, Datanscoll, Datansnocp, Datansdefer);
    printf("LoadNICstat-CSV-TC:      interface: %s\n", Datansintf);
    printf("LoadNICstat-CSV-TC:      rkb      : %f\n", Datansrkb);
#endif

    fprintf(OutputFILEstats,"%d,%d,%s,%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
	    Key1, DataServerID, DataColDate,
	    DataEpoch, Datansintf, Datansrkb, Datanswkb, Datansrpk, Datanswpk, Datansutil,
	    Datanssat, Datansierr, Datansoerr, Datanscoll, Datansnocp, Datansdefer);
    Key1++;

    

    ReadLine2(Line,IOstatFileDes,&Status);
  }  /*  while not EOF  */


  /*  clean up and go home  */

  close(IOstatFileDes);
  fclose(OutputFILEstats);

}
