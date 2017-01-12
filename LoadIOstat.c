/*

  reads an iostat -xn file (argv[1]), parses it, and loads
  it into the iostat table of the fsr database.

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

#include <my_global.h>
#include <mysql.h>
#include <my_sys.h>

#include <TokenParse.h>

int main(int argc, char *argv[]) {

  int IOstatFileDes;
  int Status;

  char IOInputFileName[512];
  char Token[256];
  char Line[1024];

  int  DataServerID;
  char DataColDate[16];
  time_t  DataEpoch;

  float Datars;
  float Dataws;
  float Datakrs;
  float Datakws;
  float Datawait;
  float Dataactv;
  float Datawsvct;
  float Dataasvct;
  int Datapctw;
  int Datapctb;
  char Datadevice[31];
  float Dataavgread;
  float Dataavgwrit;

  char strDay[5];
  char strMonth[5];
  char strDate[5];
  char strHour[5];
  char strMin[5];
  char strSec[5];
  char strYear[5];

  char datestr[15];

  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  struct tm t;
  time_t t_of_day;

  int EpochStart;

  /*  DB vars  */

  FILE *DBvip;
  char DBLocation[20];
  char DBHost[30];
  char DBUser[30];
  char DBPassword[30];
  char DBName[50];
  char DBCommand[2048];
  int DBCommandLength;

  MYSQL QueryDB;
  MYSQL_RES *QueryResult;
  MYSQL_ROW QueryRow;

  /* arg checking */

  if ( argc != 5) {
    printf("Usage:  %s CSVInputFileName ServerID ColDate EpochStart\n", argv[0]);
    return(1);
  }
  strcpy(IOInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%d",&EpochStart);
  DataEpoch = EpochStart;

  /*  printf("%s %d %d\n",IOInputFileName, DataServerID, EpochStart);*/

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                      prepare the database                        */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  DBvip=fopen("/tmp/DBvip","r");
  fscanf(DBvip,"%s",DBLocation);

#ifdef DBDEBUG
  printf("\n\nDatabase is running on %s\n",DBLocation);
#endif

  if ( strcmp(DBLocation,"localhost")==0 ) {
#ifdef DBDEBUG
    printf("setting localhost variables...\n");
#endif
    strcpy(DBHost,"localhost");
    strcpy(DBUser,"doug");
    strcpy(DBPassword,"ILikeSex");
  }
  else
    if ( strcmp(DBLocation,"big-mac")==0 ) {
#ifdef DBDEBUG
      printf("setting big-mac variables...\n");
#endif
      strcpy(DBHost,"big-mac");
      strcpy(DBUser,"doug");
      strcpy(DBPassword,"ILikeSex");
    } else {
      printf("Unknown database:  %s\n",DBLocation);
      exit(1);
    }

  fclose(DBvip);

  strcpy(DBName,"FSR");

  /*
    open the database
  */

#ifdef DBDEBUG
  printf("Opening database:\n  %s\n  %s\n  %s\n  %s\n",
         DBHost, DBUser, DBPassword, DBName
         );
#endif

  if (mysql_init(&QueryDB) == NULL) {
    fprintf(stderr,"Database not initialized\n");
    exit(-1);
  }

  if (!mysql_real_connect(&QueryDB,DBHost,DBUser,DBPassword,DBName,
                          3306,NULL,0))
    {
      fprintf(stderr, "Connect failed: %s\n",mysql_error(&QueryDB));
      exit(-1);
    }

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*                  process the iostat file                         */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( IOstatFileDes = open (IOInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",IOInputFileName, errno);
    return(errno);
  }

  /*  discard header lines  */

  /*
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);


  /*  discard the cumulative lines  */

  /*  strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
  ReadLine2(Line,IOstatFileDes,&Status);

  /*  printf("Should be %d real data --> %s\n",strlen(Line), Line);*/
  
  while ( strlen(Line) != 60 ) {
    /*    printf("burning --> (%d) %s\n",strlen(Line),Line);*/
    /*    strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
    ReadLine2(Line,IOstatFileDes,&Status);
  }

  /*  printf("out of burn loop --> %s\n", Line);*/

  /*  burn the next 2 header lines */

  /*
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  strcpy(Line,ReadLine(IOstatFileDes,&Status));
  */

  ReadLine2(Line,IOstatFileDes,&Status);
  ReadLine2(Line,IOstatFileDes,&Status);

  /*  printf("processing real data with --> %s\n",Line);*/

  while ( !EndOfFile ) {
    /*    printf("Top of while --> (%d) %s\n",strlen(Line),Line);*/

    
    while ( ( strlen(Line) != 60 ) && ( strlen(Line) != 0 ) ) {
      /*
      printf("inner while --> processing line --> (%d) %s\n",
	     strlen(Line),Line);
      */

      /*  process each data entry  */

      sscanf(Line," %f %f %f %f %f %f %f %f %d %d %s",
	     &Datars, &Dataws, &Datakrs, &Datakws, &Datawait, &Dataactv,
	     &Datawsvct, &Dataasvct, &Datapctw, &Datapctb, Datadevice);

      /*
      printf("%f %f %f %f %f %f %f %f %d %d %s\n",
	     Datars, Dataws, Datakrs, Datakws, Datawait, Dataactv,
	     Datawsvct, Dataasvct, Datapctw, Datapctb, Datadevice);
      */

      if ( Datars != 0.0 )
	Dataavgread = Datakrs / Datars;
      else
	Dataavgread = 0.0;

      if ( Dataws != 0.0 )
	Dataavgwrit = Datakws / Dataws;
      else
	Dataavgwrit = 0.0;

      sprintf(DBCommand,"insert into iostat (serverid, datestr, esttime, rs, ws, krs, kws, wait, actv, wsvct, asvct, pctw, pctb, device, avgread, avgwrit) values (%d, '%s', %d, %f, %f, %f, %f, %f, %f, %f, %f, %d, %d, '%s', %f, %f)",
	      DataServerID, DataColDate, DataEpoch,
	      Datars, Dataws, Datakrs, Datakws, Datawait, Dataactv,
	      Datawsvct, Dataasvct, Datapctw, Datapctb, Datadevice,
	      Dataavgread, Dataavgwrit);
      DBCommandLength=strlen(DBCommand);

#ifdef DBDEBUG
      printf("DB command (%d):  %s\n",DBCommandLength,DBCommand);
#endif

      /*  send the command to the DB  */

      if (mysql_real_query(&QueryDB, DBCommand, DBCommandLength)) {

	/*  we did lazy (no) list management - ignore duplicate entries  */

	/*      if ( mysql_errno(&QueryDB) != 1062 ) {*/
	fprintf(stderr,"Command %s failed: %s\n", DBCommand,
		mysql_error(&QueryDB));
	printf("---->  mysql_errno:  %d\n",mysql_errno(&QueryDB));
	exit(-1);
	/*      }*/  /*  if not ER_DUP_ENTRY  */

      }  /*  if error on DB Command  */

#ifdef DBDEBUG
      printf("consuming DB result and freeing QueryResult\n");
#endif

      QueryResult=mysql_use_result(&QueryDB);
      mysql_free_result(QueryResult);



      /*      strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
      ReadLine2(Line,IOstatFileDes,&Status);
    }  /*  end while processing chunk of data  */



    /*  done processing a chunk of data - update epoch seconds  */

    DataEpoch+=15;

    /*  and burn the second header line  */

    /*
    strcpy(Line,ReadLine(IOstatFileDes,&Status));
    strcpy(Line,ReadLine(IOstatFileDes,&Status));
    */

    ReadLine2(Line,IOstatFileDes,&Status);
    ReadLine2(Line,IOstatFileDes,&Status);


    /*    printf("Bot of while --> %s\n",Line);*/


    /*    strcpy(Line,ReadLine(IOstatFileDes,&Status));*/
  }  /*  while not EOF  */



  /*
    clean up and go home
  */

  mysql_free_result(QueryResult);
  mysql_close(&QueryDB);

  close(IOstatFileDes);


}
