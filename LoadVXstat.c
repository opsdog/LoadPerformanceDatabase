/*

  reads a vxstat -d or vxstat -v file (argv[1]), parses it, and loads
  it into the vxstat table of the fsr database.

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

  int VXstatFileDes;
  int Status;

  char VXInputFileName[512];
  char Token[256];
  char Line[1024];

  int  DataServerID;
  char DataColDate[16];
  time_t  DataEpoch;
  char DataVXtype[6];
  char DataObjName[101];
  int  DataReadops;
  int  DataWritops;
  int  DataReadblk;
  int  DataWritblk;
  float DataReadms;
  float DataWritms;

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
  strcpy(VXInputFileName,argv[1]);
  sscanf(argv[2],"%d",&DataServerID);
  sscanf(argv[3],"%s",&DataColDate);
  sscanf(argv[4],"%d",&EpochStart);

  /*  printf("%s %d %d\n",VXInputFileName, DataServerID, EpochStart);*/

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
  /*                  process the vxstat file                         */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  if (( VXstatFileDes = open (VXInputFileName, O_RDONLY) ) == -1 ) {
    printf("Open of %s failed:  %d\n",VXInputFileName, errno);
    return(errno);
  }

  /*  discard header lines, blank line, and date of cumulative entry  */

  /*
  strcpy(Line,ReadLine(VXstatFileDes,&Status));
  strcpy(Line,ReadLine(VXstatFileDes,&Status));
  strcpy(Line,ReadLine(VXstatFileDes,&Status));
  strcpy(Line,ReadLine(VXstatFileDes,&Status));
  */

  ReadLine2(Line,VXstatFileDes,&Status);
  ReadLine2(Line,VXstatFileDes,&Status);
  ReadLine2(Line,VXstatFileDes,&Status);
  ReadLine2(Line,VXstatFileDes,&Status);


  /*  discard the cumulative lines  */

  /*  strcpy(Line,ReadLine(VXstatFileDes,&Status));*/
  ReadLine2(Line,VXstatFileDes,&Status);
  /*  printf("%d\n",strlen(Line));*/
  while ( strlen(Line) != 0 )
    /*    strcpy(Line,ReadLine(VXstatFileDes,&Status));*/
    ReadLine2(Line,VXstatFileDes,&Status);

  while ( !EndOfFile ) {
    /*  next line will be a data  */
    /*    strcpy(Line,ReadLine(VXstatFileDes,&Status));*/
    ReadLine2(Line,VXstatFileDes,&Status);

    /*    printf("Top of while --> %s\n",Line);*/

    /*  convert date in vxdate format to epoch seconds */
    strcpy(strDay,GetNextToken(' ',Line));
    strcpy(strMonth,GetNextToken(' ',Line));
    strcpy(strDate,GetNextToken(' ',Line));
    strcpy(strHour,GetNextToken(':',Line));
    strcpy(strMin,GetNextToken(':',Line));
    strcpy(strSec,GetNextToken(' ',Line));
    strcpy(strYear,GetNextToken(' ',Line));
    /*
    printf("Should be a date --> %s %s %s %s:%s:%s %s\n",
	   strDay, strMonth, strDate,
	   strHour, strMin, strSec,
	   strYear);
    */

    if ( strcmp(strMonth,"Jan") == 0 ) strcpy(strMonth,"01");
    else
      if ( strcmp(strMonth,"Feb") == 0 ) strcpy(strMonth,"02");
      else
	if ( strcmp(strMonth,"Mar") == 0 ) strcpy(strMonth,"03");
	else
	  if ( strcmp(strMonth,"Apr") == 0 ) strcpy(strMonth,"04");
	  else
	    if ( strcmp(strMonth,"May") == 0 ) strcpy(strMonth,"05");
	    else
	      if ( strcmp(strMonth,"Jun") == 0 ) strcpy(strMonth,"06");
	      else
		if ( strcmp(strMonth,"Jul") == 0 ) strcpy(strMonth,"07");
		else
		  if ( strcmp(strMonth,"Aug") == 0 ) strcpy(strMonth,"08");
		  else
		    if ( strcmp(strMonth,"Sep") == 0 ) strcpy(strMonth,"09");
		    else
		      if ( strcmp(strMonth,"Oct") == 0 ) strcpy(strMonth,"10");
		      else
			if ( strcmp(strMonth,"Nov") == 0 ) strcpy(strMonth,"11");
			else
			  if ( strcmp(strMonth,"Dec") == 0 ) strcpy(strMonth,"12");

    /*  convert the date to epoch seconds  */

    sprintf(datestr,"%s%s%s%s%s%s",strYear,strMonth,strDate,strHour,strMin,strSec);
    /*    printf("datestr --> %s\n",datestr);*/
    sscanf(strYear,"%d",&year);
    sscanf(strMonth,"%d",&month);
    sscanf(strDate,"%d",&day);
    sscanf(strHour,"%d",&hour);
    sscanf(strMin,"%d",&minute);
    sscanf(strSec,"%d",&second);

    t.tm_year = year-1900;
    t.tm_mon = month - 1; // Month, 0 - jan
    t.tm_mday = day; // Day of the month
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown                      
    DataEpoch = mktime(&t);
    /*    printf("%ld\n", (long) DataEpoch);*/

    /*  process the dm/vol data */

    /*    strcpy(Line,ReadLine(VXstatFileDes,&Status));*/
    ReadLine2(Line,VXstatFileDes,&Status);
    /*    printf("should be dm/vol --> (%d) (%d) %s\n",strlen(Line),Status,Line);*/
    while ( strlen(Line) != 0 ) {

      /*  process each data entry  */
      
      sscanf(Line,"%s %s %d %d %d %d %f %f",
	     &DataVXtype, &DataObjName, &DataReadops, &DataWritops,
	     &DataReadblk, &DataWritblk, &DataReadms, &DataWritms);

      /*
      printf("read values --> %s %s %d %d %d %d %f %f\n",
	     DataVXtype, DataObjName, DataReadops, DataWritops,
	     DataReadblk, DataWritblk, DataReadms, DataWritms);
      */

      sprintf(DBCommand,"insert into vxstat (serverid, datestr, esttime, vxtype, objname, readops, writops, readblk, writblk, readms, writms) values (%d, '%s', '%d', '%s', '%s', %d, %d, %d, %d, %f, %f)",
	      DataServerID, DataColDate, DataEpoch, DataVXtype, DataObjName,
	      DataReadops, DataWritops, DataReadblk, DataWritblk, 
	      DataReadms, DataWritms);
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


      /*      strcpy(Line,ReadLine(VXstatFileDes,&Status));*/
      ReadLine2(Line,VXstatFileDes,&Status);
    }



    /*  done processing a chunk of data  */

    /*    strcpy(Line,ReadLine(VXstatFileDes,&Status));*/
  }  /*  while not EOF  */



  /*
    clean up and go home
  */

  mysql_free_result(QueryResult);
  mysql_close(&QueryDB);

  close(VXstatFileDes);


}
