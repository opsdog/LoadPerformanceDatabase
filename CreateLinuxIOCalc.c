/*
  prog to create the linux_iocalc table once the raw data is loaded

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
  float vrrqms_sum;
  float vwrqms_sum;
  float vrs_sum;
  float vws_sum;
  float vkrs_sum;
  float vkws_sum;
  float vavgrqsz_avg;
  float vavgqusz_avg;
  float vawait_avg;
  float vsvctm_avg;
  float vawait_sum;
  float vsvctm_sum;
  float vutil_avg;
  float vavgread_avg;
  float vavgwrit_avg;

  /*
    float vwait_sum;
    float vactv_sum;
    float vwsvct_avg;
    float vasvct_avg;
    float vpctw_avg;
    float vpctb_avg;
    float vasvct_sum;
    float vwsvct_sum;
  */

  /*  calculated data  */

  double vsvctm_avg_a;
  double vawait_avg_a;
  double vwkperop;
  double vrkperop;

#ifdef DEBUG
  printf("CreateLinuxIOCalc.c:  entry\n");
  printf("CreateLinuxIOCalc.c:  %d args\n", argc);
#endif

  /*  one potential arg to process  */

  if ( argc == 1 )
    TableCreate=FALSE;
  else if ( argc == 2 )
    if ( strcmp(argv[1],"-c") == 0 ) {
      TableCreate=TRUE;
    }
    else {
      printf("usage:  CreateLinuxIOCalc.c [ -c ]\n");
      printf("\n        -c  Drop and recreate the linux_iocalc table\n");
      exit(1);
    }

#ifdef DEBUG
  printf("CreateLinuxIOCalc.c:  arg processed\n");
#endif
  /*  set up database stuff  */

  strcpy(DBName,"fsr");

  DBvip=fopen("/tmp/DBvip","r");
  fscanf(DBvip,"%s",DBLocation);

#ifdef DEBUG
  printf("CreateLinuxIOCalc.c:  Database is running on %s\n",DBLocation);
#endif

  if ( strcmp(DBLocation,"localhost")==0 ) {
#ifdef DEBUG
    printf("CreateLinuxIOCalc.c:  setting localhost variables...\n");
#endif
    strcpy(DBHost,"localhost");
    strcpy(DBUser,"doug");
    strcpy(DBPassword,"ILikeSex");
  }
  else 
    if ( strcmp(DBLocation,"big-mac")==0 ) {
#ifdef DEBUG
      printf("CreateLinuxIOCalc.c:  setting big-mac variables...\n");
#endif
      strcpy(DBHost,"big-mac");
      strcpy(DBUser,"doug");
      strcpy(DBPassword,"ILikeSex");
    } else
      if ( strcmp(DBLocation,"evildb")==0 ) {
#ifdef DEBUG
	printf("CreateLinuxIOCalc.c:  setting evildb variables...\n");
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
  printf("CreateLinuxIOCalc.c:  Using database %s\n",DBName);
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
  printf("CreateLinuxIOCalc.c:  Database open\n");
#endif

  /* *********************************************************

                    drop the linux_iocalc table ?

     ********************************************************* */

  if ( TableCreate ) {
    strcpy(Command,"drop table if exists linux_iocalc");
    CommandLength=strlen(Command);

#ifdef DEBUG
    printf("CreateLinuxIOCalc.c:  dropping linux_iocalc table (%d)\n", CommandLength);
#endif

    if (mysql_query(&QueryDB, Command)) {
      fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
	      mysql_error(&QueryDB));
      mysql_close(&QueryDB);
      exit(-1);
    }
  }  /*  if TableCreate  */

  /* *********************************************************

                    create the linux_iocalc table ?

     ********************************************************* */

  if ( TableCreate ) {
    strcpy(Command,"create table linux_iocalc ( pkey     int not null auto_increment, serverid int not null, esttime  int not null default 0, rrqms_sum   float not null default 0.0, wrqms_sum   float not null default 0.0, rs_sum      float not null default 0.0, ws_sum      float not null default 0.0, krs_sum     float not null default 0.0, kws_sum     float not null default 0.0, avgrqsz_avg float not null default 0.0, avgqusz_avg float not null default 0.0, await_avg   float not null default 0.0, await_avg_a float not null default 0.0, svctm_avg   float not null default 0.0, svctm_avg_a float not null default 0.0, util_avg    float not null default 0.0, avgread_avg float not null default 0.0, avgwrit_avg float not null default 0.0, adevices    int   not null default 0, wkperop     float not null default 0.0, rkperop     float not null default 0.0, primary key (pkey), index i_serverid (serverid), index i_esttime (esttime), index i_rrqmssum (rrqms_sum), index i_wrqmssum (wrqms_sum), index i_rssum (rs_sum), index i_wssum (ws_sum), index i_krssum (krs_sum), index i_kwssum (kws_sum), index i_avgrqszsum (avgrqsz_avg), index i_avgquszavg (avgqusz_avg), index i_awaitavg (await_avg), index i_svctmavg (svctm_avg), index i_utilavg (util_avg), index i_adevices (adevices), index i_wkperop (wkperop), index i_rkperop (rkperop), index i_servest (serverid, esttime) ) engine = MyISAM");
    CommandLength=strlen(Command);

#ifdef DEBUG
    printf("CreateLinuxIOCalc.c:  creating linux_iocalc table (%d)\n", CommandLength);
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

  strcpy(Command,"select distinct(serverid) from linux_iostat order by serverid");
  CommandLength=strlen(Command);

#ifdef DEBUG
  printf("CreateLinuxIOCalc.c:  sending server list select (%d)\n", CommandLength);
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
      printf("CreateLinuxIOCalc.c:    TOP OF SERVER LOOP\n");
#endif
      strcpy(ServerID,ServerQueryRow[0]);
      printf("    Processing server %s\n",ServerID);

      /* *********************************************************

                     set up the esttime loop

	 ********************************************************* */

      /*  build the query to get the esttime list  */

      /*
	strcpy(Command,"select distinct(esttime) from linux_iostat where serverid = ");
	strcat(Command,ServerID);
	strcat(Command," order by esttime");
      */

      /*
	sprintf(Command,"select distinct(esttime) from linux_iostat where serverid = %s order by esttime",
	        ServerID);
      */

      sprintf(Command,"select distinct esttime from linux_iostat where serverid = %s and esttime not in (select distinct esttime from linux_iocalc where serverid = %s)",
	      ServerID, ServerID);

      CommandLength=strlen(Command);

#ifdef DEBUG
      printf("CreateLinuxIOCalc.c:    sending esttime list select (%d)\n", CommandLength);
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
      printf("CreateLinuxIOCalc.c:    Query stored (esttime) - processing...\n");
#endif

      if ( (EsttimeQueryRow=mysql_fetch_row(EsttimeQueryResult)) != NULL ) {
	while (EsttimeQueryRow != NULL) {
#ifdef DEBUG
	  printf("CreateLinuxIOCalc.c:    TOP OF ESTTIME LOOP\n");
#endif
	  strcpy(Esttime,EsttimeQueryRow[0]);
	  /*  printf("  Processing Esttime %s\n",Esttime);  */

	  /* *********************************************************

                        set up the data pull query

	     ********************************************************* */

	  /*
	    strcpy(Command,"select sum(rs), sum(ws), sum(krs), sum(kws), sum(wait), sum(actv), avg(wsvct), avg(asvct), avg(pctw), avg(pctb), avg(avgread), avg(avgwrit), sum(asvct), sum(wsvct) from linux_iostat where esttime = ");
	    strcat(Command,Esttime);
	    strcat(Command," and serverid = ");
	    strcat(Command,ServerID);
	    strcat(Command," and ( devtype = 1 or devtype = 2 )");
	  */

	  /*	  sprintf(Command,"select sum(rs), sum(ws), sum(krs), sum(kws), sum(wait), sum(actv), avg(wsvct), avg(asvct), avg(pctw), avg(pctb), avg(avgread), avg(avgwrit), sum(asvct), sum(wsvct) from linux_iostat where esttime = %s and serverid = %s and ( devtype = 1 or devtype = 2 )", */
	  sprintf(Command,"select sum(rrqms), sum(wrqms), sum(rs), sum(ws), sum(krs), sum(kws), avg(avgrqsz), avg(avgqusz), avg(await), avg(svctm), avg(util), avg(avgread), avg(avgwrit), sum(svctm), sum(await) from linux_iostat where esttime = %s and serverid = %s and ( devtype = 1 or devtype = 2 )",
		  Esttime, ServerID);

	  CommandLength=strlen(Command);

#ifdef DEBUG
	  /*  printf("CreateLinuxIOCalc.c:      About to send %s (%d)\n", Command, CommandLength);  */
	  printf("CreateLinuxIOCalc.c:      About to send data query (%d)\n", CommandLength);
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
	  printf("CreateLinuxIOCalc.c:      Query stored (data 1) - processing...\n");
#endif

	  if ( (DataQueryRow=mysql_fetch_row(DataQueryResult)) != NULL ) {

	    sscanf(DataQueryRow[0],"%f",&vrrqms_sum);
	    sscanf(DataQueryRow[1],"%f",&vwrqms_sum);
	    sscanf(DataQueryRow[2],"%f",&vrs_sum);
	    sscanf(DataQueryRow[3],"%f",&vws_sum);
	    sscanf(DataQueryRow[4],"%f",&vkrs_sum);
	    sscanf(DataQueryRow[5],"%f",&vkws_sum);
	    sscanf(DataQueryRow[6],"%f",&vavgrqsz_avg);
	    sscanf(DataQueryRow[7],"%f",&vavgqusz_avg);
	    sscanf(DataQueryRow[8],"%f",&vawait_avg);
	    sscanf(DataQueryRow[9],"%f",&vsvctm_avg);
	    sscanf(DataQueryRow[10],"%f",&vutil_avg);
	    sscanf(DataQueryRow[11],"%f",&vavgread_avg);
	    sscanf(DataQueryRow[12],"%f",&vavgwrit_avg);
	    sscanf(DataQueryRow[13],"%f",&vsvctm_sum);
	    sscanf(DataQueryRow[14],"%f",&vawait_sum);

	  }  /*  if query was not null  */

#ifdef DATADEBUG
	  printf("CreateLinuxIOCalc.c:          Data Dump:\n");
	  printf("CreateLinuxIOCalc.c:            vrrqms_sum: %f\n", vrrqms_sum);
	  printf("CreateLinuxIOCalc.c:            vwrqms_sum: %f\n", vwrqms_sum);
	  printf("CreateLinuxIOCalc.c:            vrs_sum: %f\n", vrs_sum);
	  printf("CreateLinuxIOCalc.c:            vws_sum: %f\n", vws_sum);
	  printf("CreateLinuxIOCalc.c:            vkrs_sum: %f\n", vkrs_sum);
	  printf("CreateLinuxIOCalc.c:            vkws_sum: %f\n", vkws_sum);
	  printf("CreateLinuxIOCalc.c:            vavgrqsz_avg: %f\n", vavgrqsz_avg);
	  printf("CreateLinuxIOCalc.c:            vavgqusz_avg: %f\n", vavgqusz_avg);
	  printf("CreateLinuxIOCalc.c:            vawait_avg: %f\n", vawait_avg);
	  printf("CreateLinuxIOCalc.c:            vsvctm_avg: %f\n", vsvctm_avg);
	  printf("CreateLinuxIOCalc.c:            vutil_avg: %f\n", vutil_avg);
	  printf("CreateLinuxIOCalc.c:            vavgread_avg: %f\n", vavgread_avg);
	  printf("CreateLinuxIOCalc.c:            vavgwrit_avg: %f\n", vavgwrit_avg);
	  printf("CreateLinuxIOCalc.c:            vsvctm_sum: %f\n", vsvctm_sum);
	  printf("CreateLinuxIOCalc.c:            vawait_sum: %f\n", vawait_sum);
#endif

	  /* *********************************************************

                         get number of active paths

	     ********************************************************* */

	  /*
	    strcpy(Command,"select count(device) from linux_iostat where esttime = ");
	    strcat(Command,Esttime);
	    strcat(Command," and serverid = ");
	    strcat(Command,ServerID);
	    strcat(Command," and ( devtype = 1 or devtype = 2 ) and asvct != 0.0");
	  */

	  sprintf(Command,"select count(device) from linux_iostat where esttime = %s and serverid = %s and ( devtype = 1 or devtype = 2 ) and svctm != 0.0",
		  Esttime, ServerID);

	  CommandLength=strlen(Command);

#ifdef DEBUG
	  /*  printf("CreateLinuxIOCalc.c:      About to send %s (%d)\n", Command, CommandLength);  */
	  printf("CreateLinuxIOCalc.c:      About to get active device count (%d)\n", CommandLength);
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
	  printf("CreateLinuxIOCalc.c:      Query stored (data 2) - processing...\n");
#endif

	  if ( (DataQueryRow=mysql_fetch_row(DataQueryResult)) != NULL ) {
	    /*  strcpy(STRvnumactivedevices,DataQueryRow[0]);  */
	    sscanf(DataQueryRow[0],"%d",&vnumactivedevices);
	  }  /*  if not null num active devices  */

#ifdef DATADEBUG
	  printf("CreateLinuxIOCalc.c:        Data Dump:\n");
	  /*  printf("CreateLinuxIOCalc.c:          STRvnumactivedevices: %s\n", STRvnumactivedevices);  */
	  printf("CreateLinuxIOCalc.c:          vnumactivedevices   : %d\n", vnumactivedevices);
#endif

	  if ( vnumactivedevices != 0 ) {
	    vsvctm_avg_a = (double)vsvctm_sum / (double)vnumactivedevices;
	    vawait_avg_a = (double)vawait_sum / (double)vnumactivedevices;
	  }
	  else {
	    vsvctm_avg_a = 0.0;
	    vawait_avg_a = 0.0;
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
	  printf("CreateLinuxIOCalc.c:        Calculated Data Dump:\n");
	  printf("CreateLinuxIOCalc.c:          vsvctm_avg_a : %f\n", vsvctm_avg_a);
	  printf("CretaelinuxIOCalc:            vawait_avg_a : %f\n", vawait_avg_a);
	  printf("CretaelinuxIOCalc:            vwkperop     : %f\n", vwkperop);
	  printf("CretaelinuxIOCalc:            vrkperop     : %f\n", vrkperop);
#endif

	  /* *********************************************************

                              generate the insert

	     ********************************************************* */

	  sprintf(Command,"insert into linux_iocalc ( serverid, esttime, rrqms_sum, wrqms_sum, rs_sum, ws_sum, krs_sum, kws_sum, avgrqsz_avg, avgqusz_avg, await_avg, await_avg_a, svctm_avg, svctm_avg_a, util_avg, avgread_avg, avgwrit_avg, adevices, wkperop, rkperop ) values ( %s, %s, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f )",
		  ServerID, Esttime, 
		  vrrqms_sum, vwrqms_sum, vrs_sum, vws_sum, vkrs_sum, vkws_sum, vavgrqsz_avg, 
		  vavgqusz_avg, vawait_avg, vawait_avg_a, vsvctm_avg, vsvctm_avg_a, vutil_avg, 
		  vavgread_avg, vavgwrit_avg, vnumactivedevices, vwkperop, vrkperop);
	  CommandLength=strlen(Command);

#ifdef DEBUG
	  printf("CreateLinuxIOCalc.c:      About to insert: \"%s\" (%d)\n", Command, CommandLength);
	  printf("CreateLinuxIOCalc.c:      About to insert (%d)\n", CommandLength);
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


