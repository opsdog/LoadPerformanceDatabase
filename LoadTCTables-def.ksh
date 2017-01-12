##
##  function definitions for LoadTCTables.ksh
##

Load_nicstat_xp_CSV()
(
echo
echo "Loading nicstat -xp files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*nicstat-xp-*.csv 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi


  echo "    $Server (${ServerID}) $ColDate"

  ##
  ##  timestamp and interface will come from each nicstat file entry
  ##

  ##  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  #echo "    $EpochBegin --> $EpochEnd"
  #echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  #EpochBase=`${WorkDir}/ToEpoch $ColDate`
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f NICstatLoad.csv

  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'nicstat'"`

  ##  echo "    ./LoadNICstat-CSV-TC $FilePath $ServerID $ColDate $NextKey"

  ./LoadNICstat-CSV-TC $FilePath $ServerID $ColDate $NextKey

  ##
  ##  load the generated CSV into the database
  ##

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/NICStatLoad.csv fsr nicstat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/NICStatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/NICStatLoad.csv fsr nicstat"
	  ssh -q big-mac "rm -f /tmp/NICStatload.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/NICStatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/NICStatLoad.csv fsr nicstat"
	  ssh -q evildb "rm -f /tmp/NICStatload.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f NICStatLoad.csv
  gzip -f $FilePath

done  ##  for each nicstat file


)  ##  end of function Load_nicstat_xp_CSV

Load_procbymem()
{
echo
echo "Loading ProcByMEM files..."

##  echo "  Purging..."
##  purge

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*ProcByMEM.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ##
  ##  process the file
  ##

  rm -f ProcByMemLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'procbymem'"`
  ## NumColls=`grep \^CPU $FilePath | wc -l | awk '{ print $1 }'`

  echo "    $Server (${ServerID}) $ColDate " #$NumColls"

  ./LoadProcMem-CSV-TC $NextKey $ServerID $FilePath

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/ProcByMemLoad.csv fsr procbymem
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/ProcByMemLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/ProcByMemLoad.csv fsr procbymem"
	  ssh -q big-mac "rm -f /tmp/ProcByMemLoad.csv"
          ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/ProcByMemLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/ProcByMemLoad.csv fsr procbymem"
	  ssh -q evildb "rm -f /tmp/ProcByMemLoad.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f ProcByMemLoad.csv
  gzip -f $FilePath

done  ##  ProcbyMEM file loop

if [ "$DBLocation" = "big-mac" ]
then
  echo
  echo "  Purging big-mac..."
  ##  ssh -q big-mac "purge"
fi

}

Load_procbycpu()
{
echo
echo "Loading ProcByCPU files..."

##  echo "  Purging..."
##  purge

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*ProcByCPU.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ##
  ##  process the file
  ##

  rm -f ProcByCpuLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'procbycpu'"`
  ## NumColls=`grep \^CPU $FilePath | wc -l | awk '{ print $1 }'`

  echo "    $Server (${ServerID}) $ColDate " #$NumColls"

  ./LoadProcCpu-CSV-TC $NextKey $ServerID $FilePath

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/ProcByCpuLoad.csv fsr procbycpu
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/ProcByCpuLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/ProcByCpuLoad.csv fsr procbycpu"
	  ssh -q big-mac "rm -f /tmp/ProcByCpuLoad.csv"
          ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/ProcByCpuLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/ProcByCpuLoad.csv fsr procbycpu"
	  ssh -q evildb "rm -f /tmp/ProcByCpuLoad.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f ProcByCpuLoad.csv
  gzip -f $FilePath

done  ##  ProcbyCPU file loop

if [ "$DBLocation" = "big-mac" ]
then
  echo
  echo "  Purging big-mac..."
  ##  ssh -q big-mac "purge"
fi

}

Load_mpstat_P_ALL()
{
echo
echo "Loading mpstat-P-ALL files..."

##  echo "  Purging..."
##  purge

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*mpstat-P-ALL-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  ## echo "    $EpochBegin --> $EpochEnd"
  ## echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f MPstatLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'linux_mpstat'"`
  NumColls=`egrep '^CPU|^ CPU' $FilePath | wc -l | awk '{ print $1 }'`

  echo "    $Server (${ServerID}) $ColDate $NumColls"

  ##  echo "  ./LoadLinuxMP-CSV-TC $FilePath $ServerID $ColDate $NextKey"

  ./LoadLinuxMP-CSV-TC $FilePath $ServerID $ColDate $NextKey

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/MPstatLoad.csv fsr linux_mpstat
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/MPstatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/MPstatLoad.csv fsr linux_mpstat"
	  ssh -q big-mac "rm -f /tmp/MPstatLoad.csv"
          ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/MPstatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/MPstatLoad.csv fsr linux_mpstat"
	  ssh -q evildb "rm -f /tmp/MPstatLoad.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f MPstatLoad.csv
  gzip -f $FilePath

done  ##  MPstat file loop

if [ "$DBLocation" = "big-mac" ]
then
  echo
  echo "  Purging big-mac..."
  ##  ssh -q big-mac "purge"
fi

}


Load_mpstat()
{
echo
echo "Loading mpstat files..."

##  echo "  Purging..."
##  purge

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*mpstat-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  ## echo "    $EpochBegin --> $EpochEnd"
  ## echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f MPstatLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'mpstat'"`
  NumColls=`egrep '^CPU|^ CPU' $FilePath | wc -l | awk '{ print $1 }'`

  echo "    $Server (${ServerID}) $ColDate $NumColls"

  ## echo "    ./LoadMPstat-CSV-TC $FilePath $ServerID $ColDate $NextKey $NumColls $EpochBegin $EpochEnd"
  ./LoadMPstat-CSV-TC $FilePath $ServerID $ColDate $NextKey $NumColls $EpochBegin $EpochEnd

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/MPstatLoad.csv fsr mpstat
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/MPstatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/MPstatLoad.csv fsr mpstat"
	  ssh -q big-mac "rm -f /tmp/MPstatLoad.csv"
          ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/MPstatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/MPstatLoad.csv fsr mpstat"
	  ssh -q evildb "rm -f /tmp/MPstatLoad.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f MPstatLoad.csv
  gzip -f $FilePath

done  ##  MPstat file loop

if [ "$DBLocation" = "big-mac" ]
then
  echo
  echo "  Purging big-mac..."
  ##  ssh -q big-mac "purge"
fi

}

Load_vmemfailures()
{
echo
echo "Loading VMEMFailures files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*VMEMFailures.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ## ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  #echo "    $EpochBegin --> $EpochEnd"
  #echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  #EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate"
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f VMEMLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'vmemfail'"`
  # echo "./LoadIOstat-CSV $FilePath $ServerID $ColDate $EpochSeconds $NextKey"
  ./LoadVMEM-CSV-TC $FilePath $ServerID $ColDate $NextKey

  case $DBLocation in
    localhost )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  $MYSQL <<EOF
          use fsr;
          load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/VMEMLoad.csv' into table vmemfail columns terminated by ',' lines terminated by '\n';
EOF
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/VMEMLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMEMLoad.csv fsr vmemfail"
	  ssh -q big-mac "rm -f /tmp/VMEMLoad.csv"
          ;;
    evildb )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/VMEMLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMEMLoad.csv fsr vmemfail"
	  ssh -q evildb "rm -f /tmp/VMEMLoad.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f VMEMLoad.csv

done  ##  VMEMfail file loop

}


Load_AIOstat() {
echo
echo "Loading AIOstat files..."
for FilePath in `ls -l ${InputDir}/*AIOstat.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ##
  ##  process the file
  ##

  rm -f AIOstatLoad.csv
  touch AIOstatLoad.csv

  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'aiodetail'"`

  echo "    $Server (${ServerID}) $ColDate $NextKey"

  ##  remove the blank line(s) at the end

  cat $FilePath | egrep -v \^\$ > ${FilePath}-fixed

  IFS=","
  exec 4<${FilePath}-fixed
  while read -u4 EpochLong Reads Writes Asyncs
  do
    ## echo "      $EpochLong $Reads $Writes $Asyncs"
    Epoch=`echo ${EpochLong}/1000000000 | bc`
    ## echo "        $Epoch --> `./FromEpoch $Epoch`"

    echo "${NextKey},${ServerID},${ColDate},${Epoch},${Reads},${Writes},${Asyncs},0,0,0,0" >> AIOstatLoad.csv
    NextKey=`echo $NextKey + 1 | bc`

  done  ##  while reading the AIOstats file
  exec 4>&-
  unset IFS

  rm -f ${FilePath}-fixed

  case $DBLocation in
    localhost )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  $MYSQL <<EOF
          use fsr;
          load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/AIOstatLoad.csv' into table aiodetail columns terminated by ',' lines terminated by '\n';
EOF
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOstatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOstatLoad.csv fsr aiodetail"
	  ssh -q big-mac "rm -f /tmp/AIOstatload.csv"
          ;;
    evildb )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOstatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOstatLoad.csv fsr aiodetail"
	  ssh -q evildb "rm -f /tmp/AIOstatload.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f AIOstatLoad.csv

done  ##  for each AIOstat file
}  ##  end of Load_aiostat function


Load_AIOdetails() {
echo
echo "Loading AIOdetails files..."
for FilePath in `ls -l ${InputDir}/*AIOdetails.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ##
  ##  process the file
  ##

  rm -f AIOdetLoad.csv
  touch AIOdetLoad.csv

  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'aiodetail'"`

  echo "    $Server (${ServerID}) $ColDate $NextKey"

  ##  remove the blank line(s) at the end

  cat $FilePath | egrep -v \^\$ > ${FilePath}-fixed

  IFS=","
  exec 4<${FilePath}-fixed
  while read -u4 EpochLong Reads Writes Asyncs Aerror Await Asusp Acanc
  do
    ## echo "      $EpochLong $Reads $Writes $Asyncs"
    Epoch=`echo ${EpochLong}/1000000000 | bc`
    ## echo "        $Epoch --> `./FromEpoch $Epoch`"

    echo "${NextKey},${ServerID},${ColDate},${Epoch},${Reads},${Writes},${Asyncs},${Aerror},${Await},${Asusp},${Acanc}" >> AIOdetLoad.csv
    NextKey=`echo $NextKey + 1 | bc`

  done  ##  while reading the AIOdetails file
  exec 4>&-
  unset IFS

  rm -f ${FilePath}-fixed

  case $DBLocation in
    localhost )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  $MYSQL <<EOF
          use fsr;
          load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/AIOdetLoad.csv' into table aiodetail columns terminated by ',' lines terminated by '\n';
EOF
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOdetLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOdetLoad.csv fsr aiodetail"
	  ssh -q big-mac "rm -f /tmp/AIOdetload.csv"
          ;;
    evildb )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOdetLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOdetLoad.csv fsr aiodetail"
	  ssh -q evildb "rm -f /tmp/AIOdetload.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f AIOdetLoad.csv

done  ##  for each AIOdetails file
}  ##  end of Load_aiodetails function


Load_AIOsyscalls() {
echo
echo "Loading AIOsyscalls files..."
for FilePath in `ls -l ${InputDir}/*AIOsyscalls.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ##
  ##  process the file
  ##

  rm -f AIOsysLoad.csv
  touch AIOsysLoad.csv

  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'aiosyscall'"`

  echo "    $Server (${ServerID}) $ColDate $NextKey"

  ##  remove the blank line(s) at the end

  cat $FilePath | egrep -v \^\$ > ${FilePath}-fixed

  IFS=","
  exec 4<${FilePath}-fixed
  while read -u4 EpochLong daioread daiowrite adone await acanc aerror asusp
  do
    ## echo "      $EpochLong $daioread $daiowrite $adone $await $acanc $aerror $asusp"
    Epoch=`echo ${EpochLong}/1000000000 | bc`
    ## echo "        $Epoch --> `./FromEpoch $Epoch`"

    echo "${NextKey},${ServerID},${ColDate},${Epoch},${daioread},${daiowrite},${adone},${await},${acanc},${aerror},${asusp}">> AIOsysLoad.csv
    NextKey=`echo $NextKey + 1 | bc`

  done  ##  while reading the AIOsyscalls file
  exec 4>&-
  unset IFS

  rm -f ${FilePath}-fixed

  case $DBLocation in
    localhost )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  $MYSQL <<EOF
          use fsr;
          load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/AIOsysLoad.csv' into table aiosyscall columns terminated by ',' lines terminated by '\n';
EOF
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOsysLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOsysLoad.csv fsr aiosyscall"
	  ssh -q big-mac "rm -f /tmp/AIOsysload.csv"
          ;;
    evildb )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOsysLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOsysLoad.csv fsr aiosyscall"
	  ssh -q evildb "rm -f /tmp/AIOsysload.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f AIOsysLoad.csv

done  ##  for each AIOsyscalls file
}  ##  end of Load_aiosyscalls function


Load_AIOveritas() {
echo
echo "Loading AIOveritas files..."
for FilePath in `ls -l ${InputDir}/*AIOveritas.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ##
  ##  process the file
  ##

  rm -f AIOvxLoad.csv
  touch AIOvxLoad.csv

  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'aioveritas'"`

  echo "    $Server (${ServerID}) $ColDate $NextKey"

  ##  remove the blank line(s) at the end

  cat $FilePath | egrep -v \^\$ > ${FilePath}-fixed

  IFS=","
  exec 4<${FilePath}-fixed
  while read -u4 EpochLong dmp_aread dmp_awrite volaread volawrite vx_aioq_init vx_aioctl vx_aioq_drain vx_aio_iodone vx_aio_done
  do
    ## echo "      $EpochLong ${dmp_aread} ${dmp_awrite} ${volaread} ${volawrite} ${vx_aioq_init} ${vx_aioctl} ${vx_aioq_drain} ${vx_aio_iodone} ${vx_aio_done}"
    Epoch=`echo ${EpochLong}/1000000000 | bc`
    ## echo "        $Epoch --> `./FromEpoch $Epoch`"

    echo "${NextKey},${ServerID},${ColDate},${Epoch},${dmp_aread},${dmp_awrite},${volaread},${volawrite},${vx_aioq_init},${vx_aioctl},${vx_aioq_drain},${vx_aio_iodone},${vx_aio_done}" >> AIOvxLoad.csv
    NextKey=`echo $NextKey + 1 | bc`

  done  ##  while reading the AIOveritas file
  exec 4>&-
  unset IFS

  rm -f ${FilePath}-fixed

  case $DBLocation in
    localhost )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  $MYSQL <<EOF
          use fsr;
          load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/AIOvxLoad.csv' into table aioveritas columns terminated by ',' lines terminated by '\n';
EOF
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOvxLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOvxLoad.csv fsr aioveritas"
	  ssh -q big-mac "rm -f /tmp/AIOvxload.csv"
          ;;
    evildb )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/AIOvxLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/AIOvxLoad.csv fsr aioveritas"
	  ssh -q evildb "rm -f /tmp/AIOvxload.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f AIOvxLoad.csv

done  ##  for each AIOveritas file
}  ##  end of Load_aioveritas function





Load_iostat_xn()
{
echo
echo "Loading iostat -xn (full) files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*iostat-xn-full.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  exec 4<$FilePath
  
  ##  discard the header lines

  read -u4 Junk
  read -u4 Junk

  ##  discard the cumulative lines

  read -u4 Line
  Res=`echo $Line | egrep 'extended|asvc_t'`
  while [   -z "$Res"   -a   ! -z "$Line"   ]
  do
    read -u4 Line
    Res=`echo $Line | egrep 'extended|asvc_t'`
  done

  #echo "first real line to process --> $Line"

  ##  process the rest of the file

  while read -u4 Line
  do
    #echo "    Line --> $Line"
    Res=`echo $Line | egrep 'extended|asvc_t'`
    if [ -z "$Res" ]
    then
      #echo "      data line - entering processing loop..."
      while [   -z "$Res"   -a   ! -z "$Line"   ]
      do
	#echo "        processing $EpochSeconds $Line"

	echo $Line | read rs ws krs kws wait actv wsvct asvct pctw pctb device
	#echo "                              $rs $ws $krs $kws $wait $actv $wsvct $asvct $pctw $pctb $device"

	AVGRead=0.0
	AVGWrit=0.0
	if [ "$rs" != "0.0" ]
	then
	  AVGRead=`echo "scale=6; $krs / $rs" | bc`
	fi
	if [ "$ws" != "0.0" ]
	then
	  AVGWrit=`echo "scale=6; $kws / $ws" | bc`
	fi

	echo "insert into iostat (serverid, datestr, esttime, rs, ws, krs, kws, wait, actv, wsvct, asvct, pctw, pctb, device, avgread, avgwrit) values ('${ServerID}', '${ColDate}', '${EpochSeconds}', ${rs}, ${ws}, ${krs}, ${kws}, ${wait}, ${actv}, ${wsvct}, ${asvct}, ${pctw}, ${pctb}, '${device}', ${AVGRead}, ${AVGWrit});" >> ${InsertFile}

	read -u4 Line
	Res=`echo $Line | egrep 'extended|asvc_t'`
      done  ##  processing a block of data
      EpochSeconds=`expr $EpochSeconds + 15`
    #else
      #echo "      discarding $Line"
    fi  ##  if not a header line

  done  ##  while reading the file

  ##
  ##  update tables
  ##

  echo
  echo "    Updating tables..."

$MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

  ##
  ##  clear files
  ##

  rm -f $InsertFile $UpdateFile
  touch $InsertFile $UpdateFile

done  ##  iostat file loop

}

Load_iostat_xn2()
{
echo
echo "Loading iostat -xn (full) files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*iostat-xn-full.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

 ./LoadIOstat $FilePath $ServerID $ColDate $EpochSeconds

done  ##  iostat file loop

}

Load_iostat_xn3()
{
echo
echo "Loading iostat -xn files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*iostat-xn-full.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  sudo purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  #echo "    $EpochBegin --> $EpochEnd"
  #echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  #EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBegin"
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f IOStatLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostat'"`
  # echo "./LoadIOstat-CSV $FilePath $ServerID $ColDate $EpochSeconds $NextKey"
  ./LoadIOstat-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $NextKey

  if [ "$DBLocation" = "evildb" ]
  then
    echo "    Loading tables (`date +%Y%m%d%H%M`)..."
    /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/IOStatLoad.csv fsr iostat
  else
    echo "    Copying csv..."
    scp -q ${WorkDir}/IOStatLoad.csv big-mac:/tmp/
    echo "    Loading tables (`date +%Y%m%d%H%M`)..."
    ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr iostat"
    ssh -q big-mac "rm -f /tmp/IOStatload.csv"
  fi

  rm -f IOStatLoad.csv
  gzip -f $FilePath

done  ##  iostat file loop

echo
echo "Loading iostat -xne files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*iostat-xne-full.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  #echo "    $EpochBegin --> $EpochEnd"
  #echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  #EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBegin"
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f IOStatLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostat'"`
  NextKey2=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostaterrs'"`
  ## echo "./LoadIOerrs-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $NextKey1 $NextKey2"
  ./LoadIOerrs-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $NextKey1 $NextKey2

  if [ "$DBLocation" = "localhost" ]
  then
    echo "    Loading tables (`date +%Y%m%d%H%M`)..."
$MYSQL <<EOF
use fsr;
load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/IOStatLoad.csv' into table iostat columns terminated by ',' lines terminated by '\n';
##load data infile '/Volumes/External300/DBProgs/fsrServers/DougPerfData/IOErrsLoad.csv' into table iostaterrs columns terminated by ',' lines terminated by '\n';
EOF
  else
    echo "    Copying csvs..."
    scp -q ${WorkDir}/IOStatLoad.csv big-mac:/tmp/
    echo "    Loading tables (`date +%Y%m%d%H%M`)..."
    ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr iostat"
    ssh -q big-mac "rm -f /tmp/IOStatload.csv"
    ##  ssh -q big-mac "purge"
  fi
  rm -f IOStatLoad.csv

done  ##  iostat file loop

}

Load_iostat_xdk()
{
echo
echo "Loading iostat -xdk files..."

for FilePath in `ls -l ${InputDir}/*iostat-xdk-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  echo "    $Server (${ServerID}) $ColDate $EpochBegin"

  ##
  ##  process the file
  ##

  rm -f IOStatLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'linux_iostat'"`
  EpochCount=`grep svctm $FilePath | wc -l | awk '{ print $1 }'`

  ##  print out some debug stuff
  ##  echo "    Server    : $Server"
  ##  echo "    ColDate   : $ColDate"
  ##  echo "    NextKey   : $NextKey1"
  ##  echo "    EpochCount: $EpochCount"
  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  ##  echo "  ./LoadIOerrs-Lin-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1"

  ./LoadIOerrs-Lin-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/IOStatLoad.csv fsr linux_iostat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr linux_iostat"
	  ssh -q big-mac "rm -f /tmp/IOStatload.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr linux_iostat"
	  ssh -q evildb "rm -f /tmp/IOStatload.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f IOStatLoad.csv
  gzip -f $FilePath

done  ##  for each iostat file
}


Load_iostat_xdk58()
{
echo
echo "Loading iostat 58 -xdk files..."

for FilePath in `ls -l ${InputDir}/*iostat-xdk-58.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  echo "    $Server (${ServerID}) $ColDate $EpochBegin"

  ##
  ##  process the file
  ##

  rm -f IOStatLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'linux_iostat58'"`
  EpochCount=`grep svctm $FilePath | wc -l | awk '{ print $1 }'`

  ##  print out some debug stuff
  ##  echo "    Server    : $Server"
  ##  echo "    ColDate   : $ColDate"
  ##  echo "    NextKey   : $NextKey1"
  ##  echo "    EpochCount: $EpochCount"
  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  ##  echo "  ./LoadIOerrs-Lin-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1"

  ./LoadIOerrs-Lin-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/IOStatLoad.csv fsr linux_iostat58
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr linux_iostat58"
	  ssh -q big-mac "rm -f /tmp/IOStatload.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr linux_iostat58"
	  ssh -q evildb "rm -f /tmp/IOStatload.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f IOStatLoad.csv
  gzip -f $FilePath

done  ##  for each iostat file
}


Load_iostat_Cxne()
{
echo
echo "Loading iostat -Cxne files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*iostat-Cxne-full.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  #echo "    $EpochBegin --> $EpochEnd"
  #echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  #EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBegin"
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f IOStatLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostat'"`
  NextKey2=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostaterrs'"`
  EpochCount=`grep extended $FilePath | wc -l | awk '{ print $1 }'`

  ## echo "./LoadIOerrs-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1 $NextKey2"

  ./LoadIOerrs-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1 $NextKey2

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/IOStatLoad.csv fsr iostat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr iostat"
	  ssh -q big-mac "rm -f /tmp/IOStatload.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr iostat"
	  ssh -q evildb "rm -f /tmp/IOStatload.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f IOStatLoad.csv
  gzip -f $FilePath

done  ##  iostat file loop

}

Load_iostat_xne58()
{
echo
echo "Loading iostat 58 -xne files..."

#for FilePath in ${InputDir}/*iostat-xn-full.txt
for FilePath in `ls -l ${InputDir}/*iostat-xne58-full.txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  echo "  Purging..."
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  #echo "    $EpochBegin --> $EpochEnd"
  #echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  #EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBegin"
  #EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f IOStatLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostat58'"`
  NextKey2=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'iostat58errs'"`
  EpochCount=`grep extended $FilePath | wc -l | awk '{ print $1 }'`

  ## echo "./LoadIOerrs-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1 $NextKey2"

  ./LoadIOerrs-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1 $NextKey2


  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/IOStatLoad.csv fsr iostat58
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr iostat58"
	  ssh -q big-mac "rm -f /tmp/IOStatload.csv"
          ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/IOStatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/IOStatLoad.csv fsr iostat58"
	  ssh -q evildb "rm -f /tmp/IOStatload.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f IOStatLoad.csv
  gzip -f $FilePath

done  ##  iostat58 file loop

}

Load_netstat_I_CSV()
{
echo
echo "Loading netstat -I files from CSV..."

##
##  fuckin' linux uses block i/o for netstat
##  we won't have the full collection time
##  chances are really, really good the last line will be truncated
##
##  it also appears that each line is cumulative, not the interval count
##    yup.  bloody hell.  useless as-is for performance
##

for FilePath in `ls -l ${InputDir}/*netstat-I-*.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  turn the file into something useful
  ##

  ##  strip the headers

  cat $FilePath | egrep -v 'Kernel|RX-DRP' > /var/tmp/$INFile

  ##  strip the last line

  LinesTotal=`wc -l /var/tmp/$INFile | awk '{ print $1 }'`
  LinesKeep=`echo "$LinesTotal - 1" | bc`
  head -$LinesKeep /var/tmp/$INFile > /var/tmp/${INFile}.useme

  ##  get the interface for this file

  Interface=`head -1 /var/tmp/${INFile}.useme | awk '{ print $1 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  ##  print out some debug stuff
  ##  echo "    Server   : $Server"
  ##  echo "    ColDate  : $ColDate"
  ##  echo "    Interface: $Interface"
  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  ##
  ##  process the file
  ##

  rm -f Netstat-ILoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'linux_netstat'"`

  ##  echo "    NextKey1 : $NextKey1"
  ##  echo
  ##  echo "  ./LoadNetstat-Lin-CSV-TC /var/tmp/${INFile}.useme $ServerID $ColDate $Interface $EpochBegin $NextKey1"

  ./LoadNetstat-Lin-CSV-TC /var/tmp/${INFile}.useme $ServerID $ColDate $Interface $EpochBegin $NextKey1


  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/Netstat-ILoad.csv fsr linux_netstat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/Netstat-ILoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/Netstat-ILoad.csv fsr linux_netstat"
	  ssh -q big-mac "rm -f /tmp/Netstat-ILoad.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/Netstat-ILoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/Netstat-ILoad.csv fsr linux_netstat"
	  ssh -q evildb "rm -f /tmp/Netstat-ILoad.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f Netstat-iLoad.csv
  /bin/rm -f /var/tmp/$INFile /var/tmp/${INFile}.useme

done  ##  for each netstat file

}

Load_netstat_i_CSV()
{
echo
echo "Loading netstat -i files from CSV..."

for FilePath in `ls -l ${InputDir}/*netstat-i-*.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  echo "    $Server (${ServerID}) $ColDate $EpochBegin"

  ##
  ##  process the file
  ##

  rm -f Netstat-iLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'netstat'"`
  InfileLines=`wc -l $FilePath | awk '{ print $1 }'`
  EpochCount=`echo $InfileLines - 2 | bc`

  ##  echo "./LoadNetstatI-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1"

  ./LoadNetstat-i-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1


  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/Netstat-iLoad.csv fsr netstat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/Netstat-iLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/Netstat-iLoad.csv fsr netstat"
	  ssh -q big-mac "rm -f /tmp/Netstat-iLoad.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/Netstat-iLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/Netstat-iLoad.csv fsr netstat"
	  ssh -q evildb "rm -f /tmp/Netstat-iLoad.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f Netstat-iLoad.csv
  gzip $FilePath

done  ##  netstat file loop

}


Load_netstat_i()
{
echo
echo "Loading netstat -i files..."

#for FilePath in ${InputDir}/cppsd09a0100_201212121558_netstat-i-*.txt
#for FilePath in ${InputDir}/*netstat-i-*.txt
for FilePath in `ls -l ${InputDir}/*netstat-i-*.txt 2>/dev/null | awk '{ print $NF }'`
do
  ## purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  exec 4<$FilePath
  
  ##  get the interface from the first header line

  read -u4 Junk1 intf Junk2

  ##  discard the second header line and cumulative line

  read -u4 Junk
  read -u4 Junk

  ##  process the rest of the file

  while read -u4 ipack ierrs opack oerrs ocoll tipack tierrs topack toerrs tocoll
  do
    #echo "    $ipack $ierrs $opack $oerrs $ocoll $tipack $tierrs $topack $toerrs $tocoll"

    if [ "$ipack" != "netstat:" ]
    then
      PCTipack=0.0
      PCTierr=0.0
      PCTopack=0.0
      PCToerr=0.0
      PCTocoll=0.0

      if [ "$tipack" != "0" ]
      then
	PCTipack=`echo "scale=12;  $ipack / $tipack * 100.0" | bc`
      fi
      if [ "$tierrs" != "0" ]
      then
	PCTierr=`echo "scale=12;  $ierrs / $tierrs * 100.0" | bc`
      fi
      if [ "$topack" != "0" ]
      then
	PCTopack=`echo "scale=12;  $opack / $topack * 100.0" | bc`
      fi
      if [ "$toerrs" != "0" ]
      then
	PCToerr=`echo "scale=12;  $oerrs / $toerrs * 100.0" | bc`
      fi
      if [ "$tocoll" != "0" ]
      then
	PCTocoll=`echo "scale=12;  $ocoll / $tocoll * 100.0" | bc`
      fi

      #echo "    $PCTipack $PCTierr / $PCTopack $PCToerr $PCTocoll"
      echo "insert into netstat (serverid, datestr, esttime, intf, ipack, ierrs, opack, oerrs, ocoll, pctipack, pctierr, pctopack, pctoerr, pctocoll) values ('${ServerID}', '${ColDate}', '${EpochSeconds}', '${intf}', ${ipack}, ${ierrs}, ${opack}, ${oerrs}, ${ocoll}, ${PCTipack}, ${PCTierr}, ${PCTopack}, ${PCToerr}, ${PCTocoll});" >> ${InsertFile}

      EpochSeconds=`expr $EpochSeconds + 15`
    fi
  done  ##  while reading the netstat -i file

  exec 4<&-  ##  close the netstat -i file
done  ##  for each netstat -i file

##
##  update tables
##

echo
echo "  Updating tables..."

$MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile

##  echo
##  echo "  Purge..."
##  purge
}

Load_vxstat()
{
echo
echo "Loading vxstat files..."

#for FilePath in ${InputDir}/cppsd09a0100_201212121558_vxstat-*arch*.txt ## ${InputDir}/cppsd09a0100_201212121558_vxstat-*redo*.txt
#for FilePath in ${InputDir}/cppsd09a0100_20121213*_vxstat-p1ogl99_datadg_asm-d.txt ${InputDir}/cppsd09a0100_20121213*_vxstat-p1ogl99_datadg_asm-v.txt
#for FilePath in ${InputDir}/*vxstat-*-[d,v].txt
for FilePath in `ls -l ${InputDir}/*vxstat-*-[d,v].txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  exec 4<$FilePath

  ##  discard header lines, blank line, and date of cumulative entry

  read -u4 Junk
  read -u4 Junk
  read -u4 Junk
  read -u4 Junk

  ##  discard the cumulative lines

  read -u4 Line
  while [[ ! -z "$Line" ]]
  do
    read -u4 Line
    #echo "discarding $Line"
  done  ##  while discarding cumulative lines

  #echo

  ##  start processing the real data

  while read -u4 Line
  do
    #echo $Line
    
    if [ ! -z "$Line" ]
    then
      Res=`echo $Line | egrep '^dm\ |^vol\ '`
      #echo "(${Res})"
      if [ ! -z "$Res" ]
      then
        #echo "  dm/vol line"
	echo $Line | read vxtype objname readops writops readblk writblk readms writms
	#echo "    $vxtype $objname $readops $writops $readblk $writblk $readms $writms"
	echo "insert into vxstat (serverid, datestr, esttime, vxtype, objname, readops, writops, readblk, writblk, readms, writms) values ('${ServerID}', '${ColDate}', '${EpochSeconds}', '${vxtype}', '${objname}', '${readops}', '${writops}', '${readblk}', '${writblk}', '${readms}', '${writms}');" >> ${InsertFile}
      else
	#echo "  date line"
	Global_vxstat_date=$Line
	vxstat_to_epoch
	EpochSeconds=$Global_vxstat_epoch
	#echo "  EpochSeconds set to $EpochSeconds"
      fi  ##  if Res is not blank
    fi  ##  if Line is blank
  done  ##  while reading the vxstat file

  exec 4<&-

done  ##  for each vxstat -d file

##
##  update tables
##

echo
echo "  Updating tables..."

$MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile
}

Load_vxstat2()
{
echo
echo "Loading vxstat files..."

#for FilePath in ${InputDir}/cppsd09a0100_201212121558_vxstat-*arch*.txt ## ${InputDir}/cppsd09a0100_201212121558_vxstat-*redo*.txt
#for FilePath in ${InputDir}/cppsd09a0100_20121213*_vxstat-p1ogl99_datadg_asm-d.txt ${InputDir}/cppsd09a0100_20121213*_vxstat-p1ogl99_datadg_asm-v.txt
#for FilePath in ${InputDir}/*vxstat-*-[d,v].txt
for FilePath in `ls -l ${InputDir}/*vxstat-*-[d,v].txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  ./LoadVXstat $FilePath $ServerID $ColDate $EpochSeconds

done  ##  for each vxstat -d file

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile
}

Load_vxstat3()
{
echo
echo "Loading vxstat files..."

#for FilePath in ${InputDir}/*vxstat-*-[d,v].txt
for FilePath in `ls -l ${InputDir}/*vxstat-*-[d,v].txt 2>/dev/null | awk '{ print $NF }'`
do
  ##  purge
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`
  vxstat3DG=`echo $INFile | awk -F\- '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  rm -f VXStatLoad.csv
  NextKey=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'vxstat'"`
  ##  echo "  ./LoadVXstat-CSV $FilePath $ServerID $ColDate $EpochSeconds $NextKey $vxstat3DG"
  ./LoadVXstat-CSV $FilePath $ServerID $ColDate $EpochSeconds $NextKey $vxstat3DG

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/VXStatLoad.csv fsr vxstat
          ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/VXStatLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VXStatLoad.csv fsr vxstat"
	  ssh -q big-mac "rm -f /tmp/VXStatload.csv"
          ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csv..."
	  scp -q ${WorkDir}/VXStatLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VXStatLoad.csv fsr vxstat"
	  ssh -q evildb "rm -f /tmp/VXStatload.csv"
          ;;
    * )          echo "DB on UNKNOWN - $DBLocation"
          exit 1
          ;;
  esac

  rm -f VXStatLoad.csv
  gzip -f $FilePath

done  ##  for each vxstat -d file

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile
}

Load_vmstat_p_CSV()
{
echo
echo "Loading vmstat -p files from CSV..."
for FilePath in `ls -l ${InputDir}/*vmstat-p-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  echo "    $Server (${ServerID}) $ColDate $EpochBegin"

  ##
  ##  process the file
  ##

  rm -f VMstat-PLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'vmstat'"`
  EpochCount=`cat $FilePath | egrep -v 'memory|swap' | wc -l | awk '{ print $1 }'`

  ##  echo "      EpochCount:  $EpochCount"

  ##  echo "./LoadVMstatP-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1"

  ./LoadVMstatP-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/VMstat-PLoad.csv fsr vmstat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/VMstat-PLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMstat-PLoad.csv fsr vmstat"
	  ssh -q big-mac "rm -f /tmp/VMstat-PLoad.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/VMstat-PLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMstat-PLoad.csv fsr vmstat"
	  ssh -q evildb "rm -f /tmp/VMstat-PLoad.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f VMstat-PLoad.csv
  gzip -f $FilePath

done  ##  for each vmstat file

}


Load_vmstat_p()
{
echo
echo "Loading vmstat -p files..."

for FilePath in `ls -l ${InputDir}/*vmstat-p-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  exec 4<$FilePath
  
  ##  discard the header lines

  read -u4 Junk
  read -u4 Junk

  ##  discard the cumulative line

  read -u 4 Junk

  ##  process the rest of the file

  while read -u4 Line
  do
    #echo $Line
    Res=`echo $Line | egrep 'memory|swap'`
    if [ -z "$Res" ]
    then
      #echo "  process this $EpochSeconds"
      echo $Line | read pswap pfree pre pmf pfr pde psr pepi pepo pepf papi papo papf pfpi pfpo pfpf
      echo "insert into vmstat (vmtype, serverid, datestr, esttime, pswap, pfree, pre, pmf, pfr, pde, psr, pepi, pepo, pepf, papi, papo, papf, pfpi, pfpo, pfpf) values ('p', '${ServerID}', '${ColDate}', '${EpochSeconds}', '${pswap}', '${pfree}', '${pre}', '${pmf}', '${pfr}', '${pde}', '${psr}', '${pepi}', '${pepo}', '${pepf}', '${papi}', '${papo}', '${papf}', '${pfpi}', '${pfpo}', '${pfpf}');" >> ${InsertFile}

      EpochSeconds=`expr $EpochSeconds + 15`
    fi  ##  if not a header line
  done  ##  while reading the vmstat -p file
  exec 4>&-

done  ##  for vmstat -p files

##
##  update tables
##

echo
echo "  Updating tables..."

$MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile

##  echo
##  echo "  Purge..."
##  purge
}

Load_vmstat_anSK()
{
echo
echo "Loading vmstat -anSK files..."

for FilePath in `ls -l ${InputDir}/*vmstat-anSK-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  echo "    $Server (${ServerID}) $ColDate $EpochBegin"

  ##
  ##  process the file
  ##

  rm -f VMstat-SLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'linux_vmstat'"`
  EpochCount=`cat $FilePath | egrep -v 'memory|swap' | wc -l | awk '{ print $1 }'`

  ##  print out some debug stuff
  ##  echo "    Server    : $Server"
  ##  echo "    ColDate   : $ColDate"
  ##  echo "    NextKey   : $NextKey1"
  ##  echo "    EpochCount: $EpochCount"
  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  ##  echo "  ./LoadVMstat-Lin-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1"

  ./LoadVMstat-Lin-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/VMstat-SLoad.csv fsr linux_vmstat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/VMstat-SLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMstat-SLoad.csv fsr linux_vmstat"
	  ssh -q big-mac "rm -f /tmp/VMstat-SLoad.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/VMstat-SLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMstat-SLoad.csv fsr linux_vmstat"
	  ssh -q evildb "rm -f /tmp/VMstat-SLoad.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f VMstat-SLoad.csv
done  ##  for each vmstat file
}


Load_vmstat_S_CSV()
{
echo
echo "Loading vmstat -S files from CSV..."

for FilePath in `ls -l ${InputDir}/*vmstat-S-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  ${ProgPrefix}/NewQuery fsr "select ebegin, eend from colldate where serverid = $ServerID and datestr = '${ColDate}'" | read EpochBegin EpochEnd

  ##  echo "    $EpochBegin --> $EpochEnd"
  ##  echo "    `./FromEpoch $EpochBegin` --> `./FromEpoch $EpochEnd`"

  echo "    $Server (${ServerID}) $ColDate $EpochBegin"

  ##
  ##  process the file
  ##

  rm -f VMstat-SLoad.csv
  NextKey1=`${ProgPrefix}/NewQuery fsr "SELECT AUTO_INCREMENT FROM information_schema.TABLES WHERE TABLE_SCHEMA = 'fsr' and table_name = 'vmstat'"`
  EpochCount=`cat $FilePath | egrep -v 'memory|swap' | wc -l | awk '{ print $1 }'`

  ##  echo "      EpochCount:  $EpochCount"

  ##  echo "./LoadVMstatS-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1"

  ./LoadVMstatS-CSV-TC $FilePath $ServerID $ColDate $EpochBegin $EpochEnd $EpochCount $NextKey1

  case $DBLocation in
    evildb )  ##  echo "DB on localhost"
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  /home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh ${WorkDir}/VMstat-SLoad.csv fsr vmstat
	  ;;
    big-mac )    ##  echo "DB on big-mac"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/VMstat-SLoad.csv big-mac:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh big-mac "/Users/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMstat-SLoad.csv fsr vmstat"
	  ssh -q big-mac "rm -f /tmp/VMstat-SLoad.csv"
	  ;;
    unused )     ##  echo "DB on evildb"
	  echo "    Copying csvs..."
	  scp -q ${WorkDir}/VMstat-SLoad.csv evildb:/tmp/
	  echo "    Loading tables (`date +%Y%m%d%H%M`)..."
	  ssh evildb "/home/douggreenwald/bin/DBLoadTable${DBProfile}.ksh /tmp/VMstat-SLoad.csv fsr vmstat"
	  ssh -q evildb "rm -f /tmp/VMstat-SLoad.csv"
	  ;;
    * )          echo "DB is UNKNOWN - $DBLocation"
	  exit 1
	  ;;
  esac

  rm -f VMstat-SLoad.csv
  gzip -f $FilePath

done  ##  for each vmstat file

}

Load_vmstat_S()
{
echo
echo "Loading vmstat -S files..."

for FilePath in `ls -l ${InputDir}/*vmstat-S-15.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  EpochBase=`${WorkDir}/ToEpoch $ColDate`
  echo "    $Server (${ServerID}) $ColDate $EpochBase"
  EpochSeconds=$EpochBase

  ##
  ##  process the file
  ##

  exec 4<$FilePath
  
  ##  discard the header lines

  read -u4 Junk
  read -u4 Junk

  ##  discard the cumulative line

  read -u 4 Junk

  ##  process the rest of the file

  while read -u4 Line
  do
    #echo $Line
    Res=`echo $Line | egrep 'kthr|swap'`
    if [ -z "$Res" ]
    then
      #echo "  process this $EpochSeconds"
      echo $Line | read r b w swap free si so pi po fr de sr s0 s1 s2 sd iin sy cs us sys id
      #echo "$r $b $w $iin $sy $cs $us $sys $id"
      if [ $sd -lt 0 ]
      then
	sd=0
      fi
      echo "insert into vmstat (vmtype, serverid, datestr, esttime, rq, bq, wq, swap, free, si, so, pi, po, fr, de, sr, s0, s1, s2, sd, iin, sy, cs, us, sys, id) values ('S', '${ServerID}', '${ColDate}', '${EpochSeconds}', '${r}', '${b}', '${w}', '${swap}', '${free}', '${si}', '${so}', '${pi}', '${po}', '${fr}', '${de}', '${sr}', '${s0}', '${s1}', '${s2}', '${sd}', '${iin}', '${sy}', '${cs}', '${us}', '${sys}', '${id}');" >> ${InsertFile}

      EpochSeconds=`expr $EpochSeconds + 15`
    fi  ##  if not a header line
  done  ##  while reading the vmstat -S file
  exec 4>&-


done  ##  for vmstat -S files

##
##  update tables
##

echo
echo "  Updating tables..."

$MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile

##  echo
##  echo "  Purge..."
##  purge
}


Load_mpstat_calc()
{

rm -f $UpdateFile
touch $UpdateFile

${MYSQL} <<EOF
use fsr;

drop table if exists mpcalc;

create table MPcalc (
pkey         int not null auto_increment,
serverid     int not null,
datestr      char(15) default 'NULL',
esttime      int not null default 0,

minf         int unsigned,
mjf          int unsigned,
xcal         int unsigned,
intr         int unsigned,
ithr         int unsigned,
csw          int unsigned,
icsw         int unsigned,
migr         int unsigned,
smtx         int unsigned,
srw          int unsigned,
syscl        int unsigned,
usr          float,
sys          float,
wt           float,
idl          float,

primary key (pkey),

index i_minf (minf),
index i_mjf (mjf),
index i_xcal (xcal),
index i_intr (intr),
index i_ithr (ithr),
index i_csw (csw),
index i_icsw (icsw),
index i_migr (migr),
index i_smtx (smtx),
index i_srw (srw),
index i_syscl (syscl),
index i_usr (usr),
index i_sys (sys),
index i_wt (wt),
index i_idl (idl)
)
engine = MyISAM
#data directory = '${MPCALCdatadir}'
#index directory = '${MPCALCindexdir}'
;
EOF

##  loop on servers in mpstat

for ServerID in `${ProgPrefix}/NewQuery fsr "select distinct serverid from mpstat order by serverid"`
do
  ## echo "    ServerID = $ServerID"

  for CollDate in `${ProgPrefix}/NewQuery fsr "select distinct datestr from mpstat where serverid = ${ServerID} order by datestr"`
  do
    ## echo "      $CollDate"

    for EpochTime in `${ProgPrefix}/NewQuery fsr "select distinct esttime from mpstat where serverid = ${ServerID} and datestr = '${CollDate}' order by esttime"`
    do
      ## echo "        $EpochTime"

      ${ProgPrefix}/NewQuery fsr "select  sum(minf), sum(mjf), sum(xcal), sum(intr), sum(ithr), sum(csw), sum(icsw), sum(migr), sum(smtx), sum(srw), sum(syscl), avg(usr), avg(sys), avg(wt), avg(idl) from mpstat where datestr = '${CollDate}' and serverid = ${ServerID} and esttime = ${EpochTime}" | read minf mjf xcal intr ithr csw icsw migr smtx srw syscl usr sys wt idl

      ## echo "          $minf $mjf $xcal $intr $ithr $csw $icsw $migr $smtx $srw $syscl $usr $sys $wt $idl"
      echo "insert into mpcalc (serverid, datestr, esttime, minf, mjf, xcal, intr, ithr, csw, icsw, migr, smtx, srw, syscl, usr, sys, wt, idl) values (${ServerID}, ${CollDate}, ${EpochTime},$minf, $mjf, $xcal, $intr, $ithr, $csw, $icsw, $migr, $smtx, $srw, $syscl, $usr, $sys, $wt, $idl);" >> $UpdateFile

    done  ##  for each EpochTime
  done  ##  for each CollDate
done  ##  for each ServerID

$MYSQL <<EOF
use fsr;
source ${UpdateFile};
EOF


}  ##  Load_mpstat_calc

Load_Throttle()
{
echo
echo "Loading Throttle files..."

for FilePath in `ls -l ${InputDir}/*Throttle.psv 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  echo "    $Server ($ServerID) $ColDate"

  ##  we currently only care about the (s)sd_reduce_throttle and
  ## (s)sd_restore_throttle entries - grep them out to be processed

  rm -f tmp_lt_lines 2>/dev/null
  egrep 'sd_reduce_throttle|sd_restore_throttle' $FilePath | egrep -v 'failed to compile' > tmp_lt_lines

  exec 4<tmp_lt_lines
  while read -u4 Line
  do

    ##  reset all values

    unset FunctionName Column8 DeviceName Provider Module Function Name
    unset Adaptthrottle Cmdsdriver Cmdsxport Currthrottle Savethrottle 
    unset Busythrottle Minthrottle Throttletype EsttimeLong

    ##  determine if this is a reduce_throttle or a restore_throttle

    FunctionName=`echo $Line | awk -F\| '{ print $4 }'`
    ## echo "      $FunctionName"

    if [ "$FunctionName" = "ssd_reduce_throttle" ]
    then
      ## echo $Line
      ## echo "      Processing ssd_reduce_throttle..."

      ##  determine the format of the Throttle file - original or extra crispy

      Column8=`echo $Line | awk -F\| '{ print $8 }' | awk -F\= '{ print $1 }'`
      ## echo "        $Column8"

      if [ "$Column8" != "un_f_use_adaptive_throttle" ]
      then
	## echo "        Original"

	EsttimeLong=`echo $Line | awk -F\| '{ print $1 }'`
	Provider=`echo $Line | awk -F\| '{ print $2 }'`
	Module=`echo $Line | awk -F\| '{ print $3 }'`
	Name=`echo $Line | awk -F\| '{ print $5 }'`
	DeviceName=`echo $Line | awk -F\| '{ print $7 }' | awk -F\= '{ print $2 }'`
	Adaptthrottle="0"
	Cmdsdriver=`echo $Line | awk -F\| '{ print $17 }' | awk -F\= '{ print $2 }'`
	Cmdsxport=`echo $Line | awk -F\| '{ print $18 }' | awk -F\= '{ print $2 }'`
	Currthrottle=`echo $Line | awk -F\| '{ print $19 }' | awk -F\= '{ print $2 }'`
	Savethrottle=`echo $Line | awk -F\| '{ print $20 }' | awk -F\= '{ print $2 }'`
	Busythrottle=`echo $Line | awk -F\| '{ print $21 }' | awk -F\= '{ print $2 }'`
	Minthrottle=`echo $Line | awk -F\| '{ print $22 }' | awk -F\= '{ print $2 }'`
	ThrottleType=`echo $Line | awk -F\| '{ print $23 }' | awk -F\= '{ print $2 }'`

	## echo "        Original"
	## echo -n "        EsttimeLong=$EsttimeLong "
	## echo -n "Provider=$Provider "
	## echo -n "Module=$Module "
	## echo "Name=$Name "
	## echo -n "        DeviceName=$DeviceName "
	## echo -n "Adaptthrottle=$Adaptthrottle "
	## echo -n "Cmdsdriver=$Cmdsdriver "
	## echo "Cmdsxport=$Cmdsxport "
	## echo -n "        Currthrottle=$Currthrottle "
	## echo -n "Savethrottle=$Savethrottle "
	## echo -n "Busythrottle=$Busythrottle "
	## echo -n "Minthrottle=$Minthrottle "
	## echo -n "ThrottleType=$ThrottleType "
	## echo

      else
	## echo "        Extra Crispy"

	EsttimeLong=`echo $Line | awk -F\| '{ print $1 }'`
	Provider=`echo $Line | awk -F\| '{ print $2 }'`
	Module=`echo $Line | awk -F\| '{ print $3 }'`
	Name=`echo $Line | awk -F\| '{ print $5 }'`
	DeviceName=`echo $Line | awk -F\| '{ print $7 }' | awk -F\= '{ print $2 }'`
	Adaptthrottle=`echo $Line | awk -F\| '{ print $8 }' | awk -F\= '{ print $2 }'`
	Cmdsdriver=`echo $Line | awk -F\| '{ print $9 }' | awk -F\= '{ print $2 }'`
	Cmdsxport=`echo $Line | awk -F\| '{ print $10 }' | awk -F\= '{ print $2 }'`
	Currthrottle=`echo $Line | awk -F\| '{ print $11 }' | awk -F\= '{ print $2 }'`
	Savethrottle=`echo $Line | awk -F\| '{ print $12 }' | awk -F\= '{ print $2 }'`
	Busythrottle=`echo $Line | awk -F\| '{ print $13 }' | awk -F\= '{ print $2 }'`
	Minthrottle=`echo $Line | awk -F\| '{ print $14 }' | awk -F\= '{ print $2 }'`
	ThrottleType=`echo $Line | awk -F\| '{ print $15 }' | awk -F\= '{ print $2 }'`

	## echo "        Original"
	## echo -n "        EsttimeLong=$EsttimeLong "
	## echo -n "Provider=$Provider "
	## echo -n "Module=$Module "
	## echo "Name=$Name "
	## echo -n "        DeviceName=$DeviceName "
	## echo -n "Adaptthrottle=$Adaptthrottle "
	## echo -n "Cmdsdriver=$Cmdsdriver "
	## echo "Cmdsxport=$Cmdsxport "
	## echo -n "        Currthrottle=$Currthrottle "
	## echo -n "Savethrottle=$Savethrottle "
	## echo -n "Busythrottle=$Busythrottle "
	## echo -n "Minthrottle=$Minthrottle "
	## echo -n "ThrottleType=$ThrottleType "
	## echo


      fi  ##  value of Column8

    fi  ##  if ssd_reduce_throttle

    if [ "$FunctionName" = "ssd_restore_throttle" ]
    then
      ## echo $Line
      ## echo "      Processing ssd_restore_throttle..."

      ##  determine the format of the Throttle file - original or extra crispy

      Column8=`echo $Line | awk -F\| '{ print $8 }' | awk -F\= '{ print $1 }'`
      ## echo "        $Column8"

      if [ "$Column8" != "un_f_use_adaptive_throttle" ]
      then
	## echo "        Original"

	EsttimeLong=`echo $Line | awk -F\| '{ print $1 }'`
	Provider=`echo $Line | awk -F\| '{ print $2 }'`
	Module=`echo $Line | awk -F\| '{ print $3 }'`
	Name=`echo $Line | awk -F\| '{ print $5 }'`
	DeviceName=`echo $Line | awk -F\| '{ print $7 }' | awk -F\= '{ print $2 }'`
	Adaptthrottle="0"
	Cmdsdriver="0"
	Cmdsxport="0"
	Currthrottle="0"
	Savethrottle="0"
	Busythrottle="0"
	Minthrottle="0"
	ThrottleType="0"

	## echo "        Original"
	## echo -n "        EsttimeLong=$EsttimeLong "
	## echo -n "Provider=$Provider "
	## echo -n "Module=$Module "
	## echo "Name=$Name "
	## echo -n "        DeviceName=$DeviceName "
	## echo -n "Adaptthrottle=$Adaptthrottle "
	## echo -n "Cmdsdriver=$Cmdsdriver "
	## echo "Cmdsxport=$Cmdsxport "
	## echo -n "        Currthrottle=$Currthrottle "
	## echo -n "Savethrottle=$Savethrottle "
	## echo -n "Busythrottle=$Busythrottle "
	## echo -n "Minthrottle=$Minthrottle "
	## echo -n "ThrottleType=$ThrottleType "
	## echo

      else
	## echo "        Extra Crispy"

	EsttimeLong=`echo $Line | awk -F\| '{ print $1 }'`
	Provider=`echo $Line | awk -F\| '{ print $2 }'`
	Module=`echo $Line | awk -F\| '{ print $3 }'`
	Name=`echo $Line | awk -F\| '{ print $5 }'`
	DeviceName=`echo $Line | awk -F\| '{ print $7 }' | awk -F\= '{ print $2 }'`
	Adaptthrottle=`echo $Line | awk -F\| '{ print $8 }' | awk -F\= '{ print $2 }'`
	Cmdsdriver=`echo $Line | awk -F\| '{ print $9 }' | awk -F\= '{ print $2 }'`
	Cmdsxport=`echo $Line | awk -F\| '{ print $10 }' | awk -F\= '{ print $2 }'`
	Currthrottle=`echo $Line | awk -F\| '{ print $11 }' | awk -F\= '{ print $2 }'`
	Savethrottle=`echo $Line | awk -F\| '{ print $12 }' | awk -F\= '{ print $2 }'`
	Busythrottle=`echo $Line | awk -F\| '{ print $13 }' | awk -F\= '{ print $2 }'`
	Minthrottle=`echo $Line | awk -F\| '{ print $14 }' | awk -F\= '{ print $2 }'`
	ThrottleType="0"

	## echo "        Original"
	## echo -n "        EsttimeLong=$EsttimeLong "
	## echo -n "Provider=$Provider "
	## echo -n "Module=$Module "
	## echo "Name=$Name "
	## echo -n "        DeviceName=$DeviceName "
	## echo -n "Adaptthrottle=$Adaptthrottle "
	## echo -n "Cmdsdriver=$Cmdsdriver "
	## echo "Cmdsxport=$Cmdsxport "
	## echo -n "        Currthrottle=$Currthrottle "
	## echo -n "Savethrottle=$Savethrottle "
	## echo -n "Busythrottle=$Busythrottle "
	## echo -n "Minthrottle=$Minthrottle "
	## echo -n "ThrottleType=$ThrottleType "
	## echo

      fi  ##  value of Column8

    fi  ##  if ssd_restore_throttle

    ##  generate SQL insert

    Esttime=`echo ${EsttimeLong}/1000000000 | bc`

    echo "insert into Throttle (serverid, datestr, esttime, provider, module, function, name, devicename, adaptthrottle, cmdsdriver, cmdsxport, currthrottle, savethrottle, busythrottle, minthrottle, throttletype) values (${ServerID}, '${ColDate}', ${Esttime}, '${Provider}', '${Module}', '${FunctionName}', '${Name}', '${DeviceName}', ${Adaptthrottle}, ${Cmdsdriver}, ${Cmdsxport}, ${Currthrottle}, ${Savethrottle}, ${Busythrottle}, ${Minthrottle}, ${ThrottleType});" >> $InsertFile


  done  ##  for each line in tmp_lt_lines
  exec 4<&-

  gzip -f $FilePath

done  ##  for each input file

##
##  update tables
##

echo
echo "  Updating tables..."

$MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

##
##  clear files
##

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile


}  ##  Load_Throttle

Load_VXcache()
{
echo
echo "Loading VXcache files..."

rm -f $InsertFile
touch $InsertFile

for FilePath in `ls -l ${InputDir}/*vxfsstat*.txt 2>/dev/null | awk '{ print $NF }'`
do
  INFile=`echo $FilePath | awk -F\/ '{ print $NF }'`
  echo "  Processing $INFile"
  Server=`echo $INFile | awk -F\_ '{ print $1 }'`
  ColDate=`echo $INFile | awk -F\_ '{ print $2 }'`

  VXFilesystem=`echo $INFile | awk -F\. '{ print $1 }' | awk -F\_ '{ print "/"$(NF-1)"/"$NF }'`

  ##
  ##  add server to server table if needed
  ##

  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${Server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'"`
  fi

  echo "    $Server ($ServerID) $ColDate $VXFilesystem"

  ##
  ##  roll through the file - lots of brutally simplistic code ahead...
  ##

  exec 4<${FilePath}

  ##  the file begins with a blank line and each block has a leading blank
  ##  line.  set up the top of the loop for this recurring blank line.

  ##  and consume the first entire block - it's cumulative

  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine
  read -u4 JunkLine

  ## echo "      last line consumed is:  -->${JunkLine}<--"

  while read -u4 Line
  do
    ## echo
    ## echo "      should be blank: -->${Line}<--"
    read -u4 LongTime Day Month Date Time Year Sample
    ## echo "      should be date:  -->${Day}, ${Month} ${Date}, ${Year} - ${Time}<--"

    ##  convert the text date to epoch seconds

    Global_vxstat_date="${Day} ${Month} ${Date} ${Time} ${Year}"
    vxstat_to_epoch
    EpochSeconds=$Global_vxstat_epoch
    ## echo "      ${Month} ${Date}, ${Year} - ${Time} -->  ${EpochSeconds}"

    ##  consume the blank line and DNLC header

    read -u4 JunkLine
    read -u4 JunkLine

    ##  read the DNLC data block

    read -u4 Dmaximum junk
    ## echo "      Dmaximum:  $Dmaximum"
    read -u4 Dtotlookups junk1 junk2 junk3 junk
    echo "$junk3" | tr \% \ | read Dfastlookup
    ## echo "      Dtotlookups, Dfastlookup:  $Dtotlookups $Dfastlookup"
    read -u4 Ddnlclookup junk1 junk2 junk3 junk4 junk
    echo "$junk4" | tr \% \ | read Ddnlchitrate
    ## echo "      Ddnlclookup, Ddnlchitrate:  $Ddnlclookup $Ddnlchitrate"
    read -u4 Denter junk1 junk2 Dhitperenter junk
    ## echo "      Denter, Dhitperenter:  $Denter $Dhitperenter"
    read -u4 Ddircachesetup junk1 junk2 junk3 Dcallspersetup junk
    ## echo "      Ddircachesetup, Dcallspersetup:  $Ddircachesetup $Dcallspersetup"
    read -u4 Dtotdirscan junk1 junk2 junk3 junk4 junk
    echo "$junk4" | tr \% \ | read Dfastdirscan
    ## echo "      Dtotdirscan, Dfastdirscan:  $Dtotdirscan $Dfastdirscan"

    ##  consume the blank line and inode header

    read -u4 JunkLine
    read -u4 JunkLine

    ##  read the inode data block

    read -u4 Icurrent junk1 junk2 Ipeak junk3 Imaximum junk
    ## echo "      Icurrent, Ipeak, Imaximum:  $Icurrent $Ipeak $Imaximum"
    read -u4 Ilookups junk1 junk2 junk
    echo "$junk2" | tr \% \ | read Ihitrate
    ## echo "      Ilookups, Ihitrate:  $Ilookups $Ihitrate"
    read -u4 Ialloced junk1 junk2 Ifreed junk
    ## echo "      Ialloced, Ifreed:  $Ialloced $Ifreed"
    read -u4 Isecrecycle junk
    read -u4 Isecfree junk
    ## echo "      Isecrecycle, Isecfree:  $Isecrecycle $Isecfree"

    ##  consume the blank line and buffer cache header

    read -u4 JunkLine
    read -u4 JunkLine

    ##  read the buffer cache data block

    read -u4 Bkcurrent junk1 junk2 Bmaximum junk
    read -u4 Blookups junk1 junk2 junk
    echo "$junk2" | tr \% \ | read Bhitrate
    read -u4 Bsecrecycle junk
    ## echo "      Bkcurrent, Bmaximum:  $Bkcurrent $Bmaximum"
    ## echo "      Blookups, Bhitrate:  $Blookups $Bhitrate"
    ## echo "      Bsecrecycle:  $Bsecrecycle"

    echo "insert into vxcache (serverid, datestr, esttime, VXfilesystem, Dmaxentries, Dtotlookups, Dfstlookups, Dtotdlookup, Ddnlchitrat, Dtotenter, Dhitperenter, Dtotdirsetup, Dcallssetup, Dtotdirscan, Dfastdirscan, Inodescurr, Ipeak, Imaximum, Ilookups, Ihitrate, Ialloced, Ifreed, Irecage, Ifreeage, Bkcurrent, Bmaximum, Blookups, Bhitrate, Brecage) values (${ServerID}, '${ColDate}', ${EpochSeconds}, '${VXFilesystem}', ${Dmaximum}, ${Dtotlookups}, ${Dfastlookup}, ${Ddnlclookup}, ${Ddnlchitrate}, ${Denter}, ${Dhitperenter}, ${Ddircachesetup}, ${Dcallspersetup}, ${Dtotdirscan}, ${Dfastdirscan}, ${Icurrent}, ${Ipeak}, ${Imaximum}, ${Ilookups}, ${Ihitrate}, ${Ialloced}, ${Ifreed}, ${Isecrecycle}, ${Isecfree}, ${Bkcurrent}, ${Bmaximum}, ${Blookups}, ${Bhitrate}, ${Bsecrecycle});" >> $InsertFile

  done  ##  while reading the file
  
  exec 4<&-

  ##
  ##  update tables
  ##

  echo "    Updating tables (`wc -l $InsertFile | awk '{ print $1 }'`)..."

  $MYSQL <<EOF
use fsr;
source ${InsertFile};
EOF

  ##
  ##  clear files
  ##

  ##  rm -f $InsertFile $UpdateFile
  touch $InsertFile $UpdateFile

  gzip -f $FilePath

done  ##  for each file


}  ##  Load_VXcache

