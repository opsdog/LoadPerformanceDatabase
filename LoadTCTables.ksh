#!/bin/ksh
##
##  read the various data files and load the tables
##
##  time corrected - we only know the exact collection time span from the
##  vxstat files.  in particular, iostat will not collect every 15 seconds
##  depending on the number of disks on a server.  
##
##  determine the actual start and end times from the vxstat file and then
##  apply the known begin and end time to the number of collections for the
##  rest of the files.
##
##  updated to include actual timestamp files - not every server has veritas
##

##
##  turn on/off DBLoad profiling
##

DBProfile="Prof"  ##  turn on

##  DBProfile=""      ##  turn off

##
##  define functions
##

. ./LoadTCTables-def.ksh

##
##  initialize stuff
##

. ./DateFuncs.ksh

MyEMail="doug.greenwald@gmail.com"

PATH="${PATH}:."

IFSorig="${IFS}"

ProgPrefix=/home/douggreenwald/bin

unset MYSQL
. ./SetDBCase.ksh

WorkDir=`pwd`
InsertFile="${WorkDir}/Inserts.sql"
UpdateFile="${WorkDir}/Updates.sql"

InputDir=${WorkDir}/Inputs

echo
echo "LoadTCTables Working from:"
echo "  $DBLocation"
echo

##
##  clear old runs
##

echo "Clearing old runs..."

rm -f $InsertFile $UpdateFile
touch $InsertFile $UpdateFile

##
##  build any required programs
##

echo "Building any required programs..."

(cd $ProgPrefix ; make ) >/dev/null 2>&1
make >/dev/null 2>&1

echo "Removing empty files"
(
  cd $InputDir
  for File in `ls -l *AIOsyscalls.txt 2>/dev/null | awk ' $5 == "147" { print $NF }'`
  do
    echo "  $File"
    gzip -f $File
  done
)


##
##  load the colldate table - holds the collection start and end times
##

echo
echo "Loading collection date table..."

rm -f tmp_servers

##  determine servers we're loading

ls -l ${InputDir}/*.txt 2>/dev/null | awk '{ print $NF }' | awk -F \_ '{ print $1 }' | awk -F \/ '{ print $NF }' | sort -u > tmp_servers

##  determine collection dates for each server

for server in `cat tmp_servers`
do
  ls -l ${InputDir}/${server}*.txt | awk '{ print $NF }' | awk -F \_ '{ print $2 }' | sort -u > tmp_${server}_colldates
done

##  pick the (arbitrary) last vxstat-v file for the server/colldate
##  determine begin and end time for the colldate

for server in `cat tmp_servers`
do
  echo "  $server"
  for colldate in `cat tmp_${server}_colldates`
  do
    echo "    $colldate"

    TS2OKAY=0
    ##  make sure dtrace didn't error out in timestamp2 file...
    ##  make sure there are actual timestamps present...
    if [ -f ${InputDir}/${server}_${colldate}_timestamp2.txt ]
    then
      ##  echo "TS2 file exists"
      Res=`egrep -i abort ${InputDir}/${server}_${colldate}_timestamp2.txt`
      if [ -z "$Res" ]
      then
	TS2rows=`cat ${InputDir}/${server}_${colldate}_timestamp2.txt | egrep -v '^$' | wc -l | awk '{ print $1 }'`
	##  echo "no aborts - $TS2rows"
	if [ $TS2rows -gt 0 ]
	then
	  TS2OKAY=1
	  ##  echo "TS2 file okay"
	fi
      fi
    fi

    ##  if [ -f ${InputDir}/${server}_${colldate}_timestamp2.txt ]
    if [ $TS2OKAY -eq 1 ]
    then
      head -2 ${InputDir}/${server}_${colldate}_timestamp2.txt | tail -1 | read SecondEpoch
      BeginDate=`echo ${SecondEpoch}/1000000000 | bc`
      cat ${InputDir}/${server}_${colldate}_timestamp2.txt | egrep -v '^$' | tail -1 | read LastEpoch
      EndDate=`echo ${LastEpoch}/1000000000 | bc`

      echo "      we have a timestamp2 file:  $BeginDate --> $EndDate"

    else
      if [ -f ${InputDir}/${server}_${colldate}_timestamp.txt ]
      then
	Global_vxstat_date=`head -2 ${InputDir}/${server}_${colldate}_timestamp.txt | tail -1`
	vxstat_to_epoch
	BeginDate=$Global_vxstat_epoch

	Global_vxstat_date=`tail -1 ${InputDir}/${server}_${colldate}_timestamp.txt`
	vxstat_to_epoch
	EndDate=$Global_vxstat_epoch

	echo "      we have a timestamp file:  $BeginDate --> $EndDate"

      fi  ##  if timestamp file

    fi  ##  if timestamp2 file

    ##  find the server id number

    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${server}'"`

  if [ -z "${ServerID}" ]
  then
    ##  server not in database - insert it
    echo "    Inserting $Server"
    IFS=$IFSorig
$MYSQL <<EOF
    use fsr;
    insert into server (name) values ('${server}');
EOF
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${server}'"`
  fi

    ##  insert row into database

    $MYSQL <<EOF
use fsr;
insert into colldate (serverid, datestr, ebegin, eend) values ($ServerID, '${colldate}', $BeginDate, $EndDate);
EOF

  done  ##  for each colldate
done  ##  for each server

##
##  pause so I can stop it if i don't like the results of adding
##  the timestamp entries
##

echo
echo "Stop me now if you need to..."
sleep 10
echo "Too late - off we go..."
echo

##
##  read the files and load the tables
##

##  retired  ##  Load_netstat_i
##  retired  ##  Load_vmstat_S
##  retired  ##  Load_vmstat_p
##  retired  ##  Load_AIOstat
##  retired  ##  Load_AIOdetails
##  retired  ##  Load_AIOsyscalls
##  retired  ##  Load_AIOveritas
##  retired  ##  Load_vmemfailures

Load_netstat_i_CSV
Load_netstat_I_CSV         ##  Linux netstat
Load_vmstat_S_CSV
Load_vmstat_p_CSV
Load_vmstat_anSK           ##  Linux vmstat
Load_Throttle
Load_vxstat3
Load_iostat_xn3
Load_iostat_Cxne
Load_iostat_xne58
Load_iostat_xdk            ##  Linux iostat
Load_iostat_xdk58          ##  Linux iostat SAN SLA
Load_mpstat
Load_mpstat_P_ALL          ##  Linux mpstat
Load_procbymem
Load_procbycpu
Load_VXcache
Load_nicstat_xp_CSV

##
##  update the SAN type on virtual servers...
##

./VServUpdateSAN.ksh

##
##  create the disk "calculated" tables
##

echo
echo "Creating calculated tables..."

echo
echo "  dm  vxcalc..."

./CreateVXCalcD

echo
echo "  vol vxcalc..."

./CreateVXCalcV

echo
echo "  SAN  iocalc..."

./CreateIOCalc

echo
echo "  NFS  iocalc..."

./CreateIOCalc-NFS

echo
echo "  ACFS iocalc..."

./CreateIOCalc-ACFS

echo
echo "  linux iocalc..."

./CreateLinuxIOCalc

##
##  send meself an email since i don't watch the console of this server
##

mailx -s "LoadTCTables completed at `date +%Y%m%d%H%M`" doug.greenwald@gmail.com <<EOF
Load completed.
EOF


###########################################################################
###########################################################################
##
##  these are not used
##
###########################################################################
###########################################################################

## echo "  iocalc_tape..."
## $MYSQL <<EOF
## use fsr;
## call createiocalc_tape('${IOCALCindexdir}');
## EOF

## echo "  ioerrcalc..."
## $MYSQL <<EOF
## use fsr;
## call createioerrcalc('${IOERRCALCdatadir}', '${IOERRCALCindexdir}');
## EOF

##echo "  mpcalc..."
##Load_mpstat_calc
