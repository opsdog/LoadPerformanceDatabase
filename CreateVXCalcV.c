/*
  prog to create the vxcalc table once the raw data is loaded

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

  long int vreadblk_sum;
  long int vwritblk_sum;
  int vnumactivedevices;
  int vreadops_sum;
  int vwritops_sum;
  float vreadms_avg;
  float vwritms_avg;
  float vreadms_max;
  float vwritms_max;


  /*  calculated data  */

  float vservtime;

#ifdef DEBUG
  printf("CreateVXCalcD:  entry\n");
  printf("CreateVXCalcD:  %d args\n", argc);
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
  printf("CreateVXCalcD:  arg processes\n");
#endif

  /*  set up database stuff  */

  strcpy(DBName,"fsr");

  DBvip=fopen("/tmp/DBvip","r");
  fscanf(DBvip,"%s",DBLocation);

#ifdef DEBUG
  printf("CreateVXCalcD:  Database is running on %s\n",DBLocation);
#endif

  if ( strcmp(DBLocation,"localhost")==0 ) {
#ifdef DEBUG
    printf("CreateVXCalcD:  setting localhost variables...\n");
#endif
    strcpy(DBHost,"localhost");
    strcpy(DBUser,"doug");
    strcpy(DBPassword,"ILikeSex");
  }
  else 
    if ( strcmp(DBLocation,"big-mac")==0 ) {
#ifdef DEBUG
      printf("CreateVXCalcD:  setting big-mac variables...\n");
#endif
      strcpy(DBHost,"big-mac");
      strcpy(DBUser,"doug");
      strcpy(DBPassword,"ILikeSex");
    } else
      if ( strcmp(DBLocation,"evildb")==0 ) {
#ifdef DEBUG
	printf("CreateVXCalcD:  setting evildb variables...\n");
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
  printf("CreateVXCalcD:  Using database %s\n",DBName);
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
  printf("CreateVXCalcD:  Database open\n");
#endif

  /* *********************************************************

                    drop the vxcalc_dm table ?

     ********************************************************* */

  if ( TableCreate ) {
    strcpy(Command,"drop table if exists vxcalc");
    CommandLength=strlen(Command);

#ifdef DEBUG
    printf("CreateVXCalcD:  dropping vxcalc_dm table (%d)\n", CommandLength);
#endif

    if (mysql_query(&QueryDB, Command)) {
      fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
	      mysql_error(&QueryDB));
      mysql_close(&QueryDB);
      exit(-1);
    }
  }  /*  if TableCreate  */

  /* *********************************************************

                    create the vxcalc table ?

     ********************************************************* */

  if ( TableCreate ) {
    strcpy(Command,"create table vxcalc ( pkey int not null auto_increment, serverid int not null, esttime int not null default 0, readops_sum int unsigned not null default 0, writops_sum int unsigned not null default 0, readblk_sum bigint not null default 0, writblk_sum bigint not null default 0, readms_avg float not null default 0.0, writms_avg float not null default 0.0, readms_max float not null default 0.0, writms_max float not null default 0.0, servtime float not null default 0.0, adevices int not null default 0, primary key (pkey), index i_serverid (serverid), index i_readops_sum (readops_sum), index i_writops_sum (writops_sum), index i_readblk_sum (readblk_sum), index i_writblk_sum (writblk_sum), index i_readms_avg (readms_avg), index i_writms_avg (writms_avg), index i_readms_max (readms_max), index i_writms_max (writms_max), index i_servtime (servtime), index i_adevices (adevices), index i_servest (serverid, esttime) ) engine = MyISAM");
    CommandLength=strlen(Command);

#ifdef DEBUG
    printf("CreateVXCalcD:  creating vxcalc table (%d)\n", CommandLength);
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

  strcpy(Command,"select distinct(serverid) from vxstat where vxtype = 'vol' order by serverid");
  CommandLength=strlen(Command);

#ifdef DEBUG
  printf("CreateVXCalcD:  sending server list select (%d)\n", CommandLength);
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
      printf("CreateVXCalcD:    TOP OF SERVER LOOP\n");
#endif
      strcpy(ServerID,ServerQueryRow[0]);
      printf("    Processing server %s\n",ServerID);

      /* *********************************************************

                     set up the esttime loop

	 ********************************************************* */

      /*  build the query to get the esttime list  */

      /*
	strcpy(Command,"select distinct(esttime) from vxstat where serverid = ");
	strcat(Command,ServerID);
	strcat(Command," and vxtype = 'vol' order by esttime");
	CommandLength=strlen(Command);
      */

      sprintf(Command,"select distinct(esttime) from vxstat where serverid = %s and vxtype = 'vol' and esttime not in (select distinct esttime from vxcalc where serverid = %s)",
	      ServerID, ServerID);
      CommandLength=strlen(Command);

#ifdef DEBUG
      printf("CreateVXCalcD:    sending esttime list select (%d)\n", CommandLength);
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
      printf("CreateVXCalcD:    Query stored (esttime) - processing...\n");
#endif

      if ( (EsttimeQueryRow=mysql_fetch_row(EsttimeQueryResult)) != NULL ) {
	while (EsttimeQueryRow != NULL) {
#ifdef DEBUG
	  printf("CreateVXCalcD:    TOP OF ESTTIME LOOP\n");
#endif
	  strcpy(Esttime,EsttimeQueryRow[0]);
	  /*  printf("  Processing Esttime %s\n",Esttime);  */

	  /* *********************************************************

                        set up the data pull query

	     ********************************************************* */

	  sprintf(Command, "select sum(readops), sum(writops), sum(readblk), sum(writblk), avg(readms), avg(writms), max(readms), max(writms) from vxstat where vxtype = 'vol' and serverid = %s and esttime = %s",
		 ServerID, Esttime);
	  CommandLength=strlen(Command);

#ifdef DEBUG
	  printf("CreateVXCalcD:      About to send %s (%d)\n", Command, CommandLength);  
	  printf("CreateVXCalcD:      About to send data query (%d)\n", CommandLength);
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
	  printf("CreateVXCalcD:      Query stored (data 1) - processing...\n");
#endif

	  if ( (DataQueryRow=mysql_fetch_row(DataQueryResult)) != NULL ) {
	    sscanf(DataQueryRow[0], "%d", &vreadops_sum);
	    sscanf(DataQueryRow[1], "%d", &vwritops_sum);
	    sscanf(DataQueryRow[2], "%d", &vreadblk_sum);
	    sscanf(DataQueryRow[3], "%d", &vwritblk_sum);
	    sscanf(DataQueryRow[4], "%f", &vreadms_avg);
	    sscanf(DataQueryRow[5], "%f", &vwritms_avg);
	    sscanf(DataQueryRow[6], "%f", &vreadms_max);
	    sscanf(DataQueryRow[7], "%f", &vwritms_max);
	  }  /*  if query was not null  */

#ifdef DATADEBUG
	  printf("CreateVXCalcD:          Data Dump:\n");
	  printf("CreateVXCalcD:            vreadops_sum: %d\n", vreadops_sum);
	  printf("CreateVXCalcD:            vwritops_sum: %d\n", vwritops_sum);
	  printf("CreateVXCalcD:            vreadblk_sum: %d\n", vreadblk_sum);
	  printf("CreateVXCalcD:            vwritblk_sum: %d\n", vwritblk_sum);
	  printf("CreateVXCalcD:            vreadms_avg: %f\n", vreadms_avg);
	  printf("CreateVXCalcD:            vwritms_avg: %f\n", vwritms_avg);
	  printf("CreateVXCalcD:            vreadms_max: %f\n", vreadms_max);
	  printf("CreateVXCalcD:            vwritms_max: %f\n", vwritms_max);
#endif

	  /* *********************************************************

                         get number of active paths

	     ********************************************************* */

	  sprintf(Command, "select count(esttime) from vxstat where vxtype = 'vol' and serverid = %s and esttime = %s and readops != 0 and writops != 0",
		 ServerID, Esttime);
	  CommandLength=strlen(Command);

#ifdef DEBUG
	  printf("CreateVXCalcD:      About to send %s (%d)\n", Command, CommandLength);  
	  printf("CreateVXCalcD:      About to get active device count (%d)\n", CommandLength);
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
	  printf("CreateVXCalcD:      Query stored (data 2) - processing...\n");
#endif

	  if ( (DataQueryRow=mysql_fetch_row(DataQueryResult)) != NULL ) {
	    /*  strcpy(STRvnumactivedevices,DataQueryRow[0]);  */
	    sscanf(DataQueryRow[0],"%d",&vnumactivedevices);
	  }  /*  if not null num active devices  */

#ifdef DATADEBUG
	  printf("CreateVXCalcD:        Data Dump:\n");
	  /*  printf("CreateVXCalcD:          STRvnumactivedevices: %s\n", STRvnumactivedevices);  */
	  printf("CreateVXCalcD:          vnumactivedevices   : %d\n", vnumactivedevices);
#endif

	  vservtime=vreadms_avg + vwritms_avg;

#ifdef DATADEBUG
	  printf("CreateVXCalcD:        Calculated Data Dump:\n");
	  printf("CreateVXCalcD:          vservtime: %f\n", vservtime);
#endif

	  /* *********************************************************

                              generate the insert

	     ********************************************************* */

	  sprintf(Command,"insert into vxcalc ( serverid, esttime, readops_sum, writops_sum, readblk_sum, writblk_sum, readms_avg, writms_avg, readms_max, adevices, writms_max, servtime ) values ( %s, %s, %d, %d, %d, %d, %f, %f, %f, %d, %f, %f)",
		  ServerID, Esttime, 
		  vreadops_sum, vwritops_sum, vreadblk_sum, vwritblk_sum, 
		  vreadms_avg, vwritms_avg, vreadms_max, vnumactivedevices, 
		  vwritms_max, vservtime);
	  CommandLength=strlen(Command);

#ifdef DEBUG
	  printf("CreateVXCalcD:      About to insert: \"%s\" (%d)\n", Command, CommandLength);  
	  printf("CreateVXCalcD:      About to insert (%d)\n", CommandLength);
#endif

	  if (mysql_query(&QueryDB, Command)) {
	    fprintf(stderr,"\n\nCommand \"%s\" failed:\n  %s\n\n",Command,
		    mysql_error(&QueryDB));
	    fprintf(stderr,"  ==  BAILING ==\n");
	    mysql_free_result(ServerQueryResult);
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


