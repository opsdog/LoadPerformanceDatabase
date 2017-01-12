/*
  prog to create the iocalc table once the raw data is loaded

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>
#include <my_sys.h>

/*
#undef DEBUG
#define DEBUG
*/

/*
#undef DATADEBUG
#define DATADEBUG
*/


main (int argc, char *argv[])
{
  int CommandLength;
  int NumFields;
  int EsttimeNumFields;
  int TableCreate;

  MYSQL QueryDB;
  MYSQL_RES *ServerQueryResult;
  MYSQL_RES *EsttimeQueryResult;
  MYSQL_RES *DataQueryResult;
  MYSQL_ROW ServerQueryRow;
  MYSQL_ROW EsttimeQueryRow;
  MYSQL_ROW DataQueryRow;
  MYSQL_FIELD *FieldDB;

  char Command[2048];
  char DBName[50];
  char ServerID[50];
  char Esttime[13];

  FILE *DBvip;
  char DBLocation[20];
  char DBHost[30];
  char DBUser[30];
  char DBPassword[30];

  /*  the data in string and numeric form  */

  /*
  char STRvnumactivedevices[256];
  char STRvrs_sum[256];
  char STRvws_sum[256];
  char STRvkrs_sum[256];
  char STRvkws_sum[256];
  char STRvwait_sum[256];
  char STRvactv_sum[256];
  char STRvwsvct_avg[256];
  char STRvasvct_avg[256];
  char STRvpctw_avg[256];
  char STRvpctb_avg[256];
  char STRvavgread_avg[256];
  char STRvavgwrit_avg[256];
  char STRvasvct_sum[256];
  char STRvwsvct_sum[256];
  */

  int vnumactivedevices;
  float vrs_sum;
  float vws_sum;
  float vkrs_sum;
  float vkws_sum;
  float vwait_sum;
  float vactv_sum;
  float vwsvct_avg;
  float vasvct_avg;
  float vpctw_avg;
  float vpctb_avg;
  float vavgread_avg;
  float vavgwrit_avg;
  float vasvct_sum;
  float vwsvct_sum;

  /*  calculated data  */

  double vasvct_avg_a;
  double vwsvct_avg_a;
  double vwkperop;
  double vrkperop;

#ifdef DEBUG
  printf("CreateIOCalc:  entry\n");
  printf("CreateIOCalc:  %d args\n", argc);
#endif

  /*  one potential arg to process  */

  if ( argc == 1 )
    TableCreate=FALSE;
  else if ( argc == 2 )
    if ( strcmp(argv[1],"-c") == 0 ) {
      TableCreate=TRUE;
    }
    else {
      printf("usage:  CreateIOCalc [ -c ]\n");
      exit(1);
    }

#ifdef DEBUG
  printf("CreateIOCalc:  arg processes\n");
#endif

  /*  set up database stuff  */

  strcpy(DBName,"fsr");

  DBvip=fopen("/tmp/DBvip","r");
  fscanf(DBvip,"%s",DBLocation);

#ifdef DEBUG
  printf("CreateIOCalc:  Database is running on %s\n",DBLocation);
#endif

  if ( strcmp(DBLocation,"localhost")==0 ) {
#ifdef DEBUG
    printf("CreateIOCalc:  setting localhost variables...\n");
#endif
    strcpy(DBHost,"localhost");
    strcpy(DBUser,"doug");
    strcpy(DBPassword,"ILikeSex");
  }
  else 
    if ( strcmp(DBLocation,"big-mac")==0 ) {
#ifdef DEBUG
      printf("CreateIOCalc:  setting big-mac variables...\n");
#endif
      strcpy(DBHost,"big-mac");
      strcpy(DBUser,"doug");
      strcpy(DBPassword,"ILikeSex");
    } else
      if ( strcmp(DBLocation,"evildb")==0 ) {
#ifdef DEBUG
	printf("CreateIOCalc:  setting evildb variables...\n");
#endif
	strcpy(DBHost,"evildb");
	strcpy(DBUser,"doug");
	strcpy(DBPassword,"ILikeSex");
      } else {
	printf("Unknown server:  %s\n",DBLocation);
	exit(1);
      }

  fclose(DBvip);

#ifdef DEBUG
  printf("CreateIOCalc:  Using database %s\n",DBName);
#endif

  /* open the database */

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

#ifdef DEBUG
  printf("CreateIOCalc:  Database open\n");
#endif

  /* *********************************************************

                    drop the iocalc table ?

     ********************************************************* */

  if ( TableCreate ) {
    strcpy(Command,"drop table if exists iocalc");
    CommandLength=strlen(Command);

#ifdef DEBUG
    printf("CreateIOCalc:  dropping iocalc table (%d)\n", CommandLength);
#endif

    if (mysql_query(&QueryDB, Command)) {
      fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
	      mysql_error(&QueryDB));
      mysql_close(&QueryDB);
      exit(-1);
    }
  }  /*  if TableCreate  */

  /* *********************************************************

                    create the iocalc table ?

     ********************************************************* */

  if ( TableCreate ) {
    strcpy(Command,"create table iocalc ( pkey     int not null auto_increment, serverid int not null, esttime  int not null default 0, rs_sum      float not null default 0.0, ws_sum      float not null default 0.0, krs_sum     float not null default 0.0, kws_sum     float not null default 0.0, wait_sum    float not null default 0.0, actv_sum    float not null default 0.0, wsvct_avg   float not null default 0.0, asvct_avg   float not null default 0.0, pctw_avg    float not null default 0.0, pctb_avg    float not null default 0.0, avgread_avg float not null default 0.0, avgwrit_avg float not null default 0.0, asvct_avg_a float not null default 0.0, wsvct_avg_a float not null default 0.0, adevices    int   not null default 0, wkperop     float not null default 0.0, rkperop     float not null default 0.0, primary key (pkey), index i_serverid (serverid), index i_esttime (esttime), index i_rssum (rs_sum), index i_wssum (ws_sum), index i_krssum (krs_sum), index i_kwssum (kws_sum), index i_actvsum (actv_sum), index i_asvct_avg (asvct_avg), index i_adevices (adevices), index i_wkperop (wkperop), index i_rkperop (rkperop), index i_servest (serverid, esttime) ) engine = MyISAM");
    CommandLength=strlen(Command);

#ifdef DEBUG
    printf("CreateIOCalc:  creating iocalc table (%d)\n", CommandLength);
#endif

    if (mysql_query(&QueryDB, Command)) {
      fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
	      mysql_error(&QueryDB));
      mysql_close(&QueryDB);
      exit(-1);
    }
  }  /*  if TableCreate  */

  /* *********************************************************

                   set up the serverid loop

     ********************************************************* */

  /*  build the query to get the server list  */

  strcpy(Command,"select distinct(serverid) from iostat order by serverid");
  CommandLength=strlen(Command);

#ifdef DEBUG
  printf("CreateIOCalc:  sending server list select (%d)\n", CommandLength);
#endif

  if (mysql_real_query(&QueryDB, Command, CommandLength)) {
    fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
	    mysql_error(&QueryDB));
    mysql_close(&QueryDB);
    exit(-1);
  }
  /*  ServerQueryResult=mysql_use_result(&QueryDB);*/
  ServerQueryResult=mysql_store_result(&QueryDB);

  if ( (ServerQueryRow=mysql_fetch_row(ServerQueryResult)) != NULL ) {
    while (ServerQueryRow != NULL) {
#ifdef DEBUG
      printf("CreateIOCalc:    TOP OF SERVER LOOP\n");
#endif
      strcpy(ServerID,ServerQueryRow[0]);
      printf("    Processing server %s\n",ServerID);

      /* *********************************************************

                     set up the esttime loop

	 ********************************************************* */

      /*  build the query to get the esttime list  */

      /*
	strcpy(Command,"select distinct(esttime) from iostat where serverid = ");
	strcat(Command,ServerID);
	strcat(Command," order by esttime");
      */

      /*
	sprintf(Command,"select distinct(esttime) from iostat where serverid = %s order by esttime",
	        ServerID);
      */

      sprintf(Command,"select distinct esttime from iostat where serverid = %s and ( devtype = 1 or devtype = 2 ) and esttime not in (select distinct esttime from iocalc where serverid = %s)",
	      ServerID, ServerID);

      CommandLength=strlen(Command);

#ifdef DEBUG
      printf("CreateIOCalc:    sending esttime list select (%d)\n", CommandLength);
#endif

      if (mysql_real_query(&QueryDB, Command, CommandLength)) {
      	fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
      		mysql_error(&QueryDB));
      	mysql_free_result(ServerQueryResult);
      	mysql_close(&QueryDB);
      	exit(-1);
      }
      /*  EsttimeQueryResult=mysql_use_result(&QueryDB);  */
      EsttimeQueryResult=mysql_store_result(&QueryDB);

      printf("      Times received - populating...\n");

#ifdef DEBUG
      printf("CreateIOCalc:    Query stored (esttime) - processing...\n");
#endif

      if ( (EsttimeQueryRow=mysql_fetch_row(EsttimeQueryResult)) != NULL ) {
	while (EsttimeQueryRow != NULL) {
#ifdef DEBUG
	  printf("CreateIOCalc:    TOP OF ESTTIME LOOP\n");
#endif
	  strcpy(Esttime,EsttimeQueryRow[0]);
	  /*  printf("  Processing Esttime %s\n",Esttime);  */

	  /* *********************************************************

                        set up the data pull query

	     ********************************************************* */

	  /*
	    strcpy(Command,"select sum(rs), sum(ws), sum(krs), sum(kws), sum(wait), sum(actv), avg(wsvct), avg(asvct), avg(pctw), avg(pctb), avg(avgread), avg(avgwrit), sum(asvct), sum(wsvct) from iostat where esttime = ");
	    strcat(Command,Esttime);
	    strcat(Command," and serverid = ");
	    strcat(Command,ServerID);
	    strcat(Command," and ( devtype = 1 or devtype = 2 )");
	  */

	  sprintf(Command,"select sum(rs), sum(ws), sum(krs), sum(kws), sum(wait), sum(actv), avg(wsvct), avg(asvct), avg(pctw), avg(pctb), avg(avgread), avg(avgwrit), sum(asvct), sum(wsvct) from iostat where esttime = %s and serverid = %s and ( devtype = 1 or devtype = 2 )",
		  Esttime, ServerID);

	  CommandLength=strlen(Command);

#ifdef DEBUG
	  /*  printf("CreateIOCalc:      About to send %s (%d)\n", Command, CommandLength);  */
	  printf("CreateIOCalc:      About to send data query (%d)\n", CommandLength);
#endif

	  if (mysql_real_query(&QueryDB, Command, CommandLength)) {
	    fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
		    mysql_error(&QueryDB));
	    mysql_free_result(EsttimeQueryResult);
	    mysql_free_result(ServerQueryResult);
	    mysql_close(&QueryDB);
	    exit(-1);
	  }
	  /*  EsttimeQueryResult=mysql_use_result(&QueryDB);  */
	  DataQueryResult=mysql_store_result(&QueryDB);

#ifdef DEBUG
	  printf("CreateIOCalc:      Query stored (data 1) - processing...\n");
#endif

	  if ( (DataQueryRow=mysql_fetch_row(DataQueryResult)) != NULL ) {
	    sscanf(DataQueryRow[0],"%f",&vrs_sum);
	    sscanf(DataQueryRow[1],"%f",&vws_sum);
	    sscanf(DataQueryRow[2],"%f",&vkrs_sum);
	    sscanf(DataQueryRow[3],"%f",&vkws_sum);
	    sscanf(DataQueryRow[4],"%f",&vwait_sum);
	    sscanf(DataQueryRow[5],"%f",&vactv_sum);
	    sscanf(DataQueryRow[6],"%f",&vwsvct_avg);
	    sscanf(DataQueryRow[7],"%f",&vasvct_avg);
	    sscanf(DataQueryRow[8],"%f",&vpctw_avg);
	    sscanf(DataQueryRow[9],"%f",&vpctb_avg);
	    sscanf(DataQueryRow[10],"%f",&vavgread_avg);
	    sscanf(DataQueryRow[11],"%f",&vavgwrit_avg);
	    sscanf(DataQueryRow[12],"%f",&vasvct_sum);
	    sscanf(DataQueryRow[13],"%f",&vwsvct_sum);
	  }  /*  if query was not null  */

#ifdef DATADEBUG
	  printf("CreateIOCalc:          Data Dump:\n");
	  printf("CreateIOCalc:            vrs_sum: %f\n", vrs_sum);
	  printf("CreateIOCalc:            vws_sum: %f\n", vws_sum);
	  printf("CreateIOCalc:            vkrs_sum: %f\n", vkrs_sum);
	  printf("CreateIOCalc:            vkws_sum: %f\n", vkws_sum);
	  printf("CreateIOCalc:            vwait_sum: %f\n", vwait_sum);
	  printf("CreateIOCalc:            vactv_sum: %f\n", vactv_sum);
	  printf("CreateIOCalc:            vwsvct_avg: %f\n", vwsvct_avg);
	  printf("CreateIOCalc:            vasvct_avg: %f\n", vasvct_avg);
	  printf("CreateIOCalc:            vpctw_avg: %f\n", vpctw_avg);
	  printf("CreateIOCalc:            vpctb_avg: %f\n", vpctb_avg);
	  printf("CreateIOCalc:            vavgread_avg: %f\n", vavgread_avg);
	  printf("CreateIOCalc:            vavgwrit_avg: %f\n", vavgwrit_avg);
	  printf("CreateIOCalc:            vasvct_sum: %f\n", vasvct_sum);
	  printf("CreateIOCalc:            vwsvct_sum: %f\n", vwsvct_sum);
#endif

	  /* *********************************************************

                         get number of active paths

	     ********************************************************* */

	  /*
	    strcpy(Command,"select count(device) from iostat where esttime = ");
	    strcat(Command,Esttime);
	    strcat(Command," and serverid = ");
	    strcat(Command,ServerID);
	    strcat(Command," and ( devtype = 1 or devtype = 2 ) and asvct != 0.0");
	  */

	  sprintf(Command,"select count(device) from iostat where esttime = %s and serverid = %s and ( devtype = 1 or devtype = 2 ) and asvct != 0.0",
		  Esttime, ServerID);

	  CommandLength=strlen(Command);

#ifdef DEBUG
	  /*  printf("CreateIOCalc:      About to send %s (%d)\n", Command, CommandLength);  */
	  printf("CreateIOCalc:      About to get active device count (%d)\n", CommandLength);
#endif

	  if (mysql_real_query(&QueryDB, Command, CommandLength)) {
	    fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
		    mysql_error(&QueryDB));
	    mysql_free_result(EsttimeQueryResult);
	    mysql_free_result(ServerQueryResult);
	    mysql_close(&QueryDB);
	    exit(-1);
	  }
	  DataQueryResult=mysql_store_result(&QueryDB);

#ifdef DEBUG
	  printf("CreateIOCalc:      Query stored (data 2) - processing...\n");
#endif

	  if ( (DataQueryRow=mysql_fetch_row(DataQueryResult)) != NULL ) {
	    /*  strcpy(STRvnumactivedevices,DataQueryRow[0]);  */
	    sscanf(DataQueryRow[0],"%d",&vnumactivedevices);
	  }  /*  if not null num active devices  */

#ifdef DATADEBUG
	  printf("CreateIOCalc:        Data Dump:\n");
	  /*  printf("CreateIOCalc:          STRvnumactivedevices: %s\n", STRvnumactivedevices);  */
	  printf("CreateIOCalc:          vnumactivedevices   : %d\n", vnumactivedevices);
#endif

	  if ( vnumactivedevices != 0 ) {
	    vasvct_avg_a = (double)vasvct_sum / (double)vnumactivedevices;
	    vwsvct_avg_a = (double)vwsvct_sum / (double)vnumactivedevices;
	  }
	  else {
	    vasvct_avg_a = 0.0;
	    vwsvct_avg_a = 0.0;
	  }

	  if ( vws_sum != 0 )
	    vwkperop = (double)vkws_sum / (double)vws_sum;
	  else
	    vwkperop = 0.0 ;

	  if ( vrs_sum != 0 )
	    vrkperop = (double)vkrs_sum / (double)vrs_sum;
	  else
	    vrkperop = 0.0;

#ifdef DATADEBUG
	  printf("CreateIOCalc:        Calculated Data Dump:\n");
	  printf("CreateIOCalc:          vasvct_avg_a: %f\n", vasvct_avg_a);
	  printf("CretaeIOCalc:          vwsvct_avg_a: %f\n", vwsvct_avg_a);
	  printf("CretaeIOCalc:          vwkperop    : %f\n", vwkperop);
	  printf("CretaeIOCalc:          vrkperop    : %f\n", vrkperop);
#endif

	  /* *********************************************************

                              generate the insert

	     ********************************************************* */

	  sprintf(Command,"insert into iocalc ( serverid, esttime, rs_sum,  ws_sum, krs_sum, kws_sum, wait_sum, actv_sum, wsvct_avg, asvct_avg, pctw_avg, pctb_avg, avgread_avg, avgwrit_avg, asvct_avg_a, wsvct_avg_a, adevices, wkperop, rkperop ) values ( %s, %s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f )",
		  ServerID, Esttime, vrs_sum, vws_sum, vkrs_sum, vkws_sum, 
		  vwait_sum, vactv_sum, vwsvct_avg, vasvct_avg, vpctw_avg, 
		  vpctb_avg, vavgread_avg, vavgwrit_avg, vasvct_avg_a, 
		  vwsvct_avg_a, vnumactivedevices, vwkperop, vrkperop);
	  CommandLength=strlen(Command);

#ifdef DEBUG
	  /*  printf("CreateIOCalc:      About to insert: \"%s\" (%d)\n", Command, CommandLength);  */
	  printf("CreateIOCalc:      About to insert (%d)\n", CommandLength);
#endif

	  if (mysql_query(&QueryDB, Command)) {
	    fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
		    mysql_error(&QueryDB));
	    mysql_close(&QueryDB);
	    exit(-1);
	  }

	  /*  bottom of esttime loop */

	  EsttimeQueryRow=mysql_fetch_row(EsttimeQueryResult);
	}  /*  while esttime  */
      }  /*  if not null esttime result  */
      mysql_free_result(EsttimeQueryResult);


      /*  bottom of serverid loop  */

      ServerQueryRow=mysql_fetch_row(ServerQueryResult);
    }  /* while server  */
  }  /* if not null serverid result  */

  /*  clean up and go home  */

  mysql_free_result(ServerQueryResult);
  mysql_close(&QueryDB);

}


