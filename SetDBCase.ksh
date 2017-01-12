DBLocation=`${ProgPrefix}/FindDB.ksh`

case $DBLocation in
  quarter-pounder )  ##  echo "DB on localhost"
        MYSQL="mysql -h quater-pounder -u doug -pILikeSex -A"
        DBdatadir=/Volumes/External300/mysqldata/
        DBindexdir=/Volumes/QPBackup/mysqldata/
        VXCALCdatadir=/Volumes/External300/mysqldata/
        VXCALCindexdir=/Volumes/QPBackup/mysqldata/
        IOCALCdatadir=/Volumes/External300/mysqldata/
        IOCALCindexdir=/Volumes/QPBackup/mysqldata/
        DBdirClause="data directory '${DBdatadir}' index directory '${DBindexdir}'"
        ;;
  big-mac )    ##  echo "DB on big-mac"
        MYSQL="mysql -h big-mac -u doug -pILikeSex -A"
        DBdatadir=/Volumes/DBdata/
        DBindexdir=/Volumes/DBindex/
        VXCALCdatadir=/Volumes/DBdata
        VXCALCindexdir=/Volumes/DBindex
        IOCALCdatadir=/Volumes/DBdata
        IOCALCindexdir=/Volumes/DBindex
        DBdirClause="data directory '${DBdatadir}' index directory '${DBindexdir}'"
        ;;
  evildb )     ##  echo "DB on evildb"
        MYSQL="mysql --login-path=local -A"
        DBdatadir=""
        DBindexdir=""
        VXCALCdatadir=""
        VXCALCindexdir=""
        IOCALCdatadir=""
        IOCALCindexdir=""
        DBdirClause=""
        ;;
  * )
        echo
        echo "UNKNOWN DB at $DBLocation !!"
        echo
        exit 1
        ;;
esac
