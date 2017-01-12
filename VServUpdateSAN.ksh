#!/bin/ksh
##
##  script to run after the DB load to update the SAN type on virtual servers
##
##  their virtual LUNs have a variety of odd naming conventions
##  this script will just go update all of them to SAN
##

##  initialize stuff

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
echo "`basename $0` Working from:"
echo "  $DBLocation"
echo

##
##  clear old runs
##

##
##  set the list of virtual servers
##  i need to modify the server table to include a type so this
##  can be dynamic
##
##  until then...
##

##  VirtualServerList="cpdsa00a0008 ccdsw00a0003 cpdsw00a0009 cpdsw00a0010 cpdsw00a0011 cpdsw00a0012 cpdsw00a0013 cpdsw00a0014 cpdsw00a0015 cpdsw00a0016 crdsw00a0003 crdsw00a0004 crdsw00a0005 crdsw00a0006 cudsw00a0013 cudsw00a0014"

VirtualServerList="ccdsa97a0021 ccdsa98a0021 ccdsa99a0021 ccdsw00a0001 ccdsw00a0002 ccdsw00a0003 ccdsw00a0004 ccdsw00a0005 cddsa00a0017 cddsa97a0021 cddsa98a0021 cddsa99a0021 cddsw00a0001 cddsw00a0002 cddsw00a0003 cddsw00a0004 cddsw00a0005 cldsw00a0002 cldsw00a0003 cldsw00a0004 cldsw00a0005 cldsw00a0019 cndsa01a0113 cndsa02a0113 cpdsa00a0008 cpdsa00a0009 cpdsa00a0013 cpdsa01a0113 cpdsa02a0113 cpdsa03a0113 cpdsa04a0113 cpdsa97a0021 cpdsa98a0021 cpdsa99a0021 cpdsw00a0009 cpdsw00a0010 cpdsw00a0011 cpdsw00a0012 cpdsw00a0013 cpdsw00a0014 cpdsw00a0015 cpdsw00a0016 cpdsw00a0019 cpdsw00a0020 crdsa00a0001 crdsa01a0113 crdsa02a0113 crdsa03a0113 crdsa04a0113 crdsa97a0021 crdsa98a0021 crdsa99a0021 crdsw00a0003 crdsw00a0004 crdsw00a0005 crdsw00a0006 crdsw00a0007 crdsw00a0008 crdsw00a0009 crdsw00a0010 crdsw00a0015 crdsw00a0016 csdsa01a0113 csdsa02a0113 csdsa03a0113 cudsa00a0004 cudsa01a0113 cudsa02a0113 cudsa03a0113 cudsa04a0113 cudsa97a0021 cudsa98a0021 cudsa99a0021 cudsw00a0005 cudsw00a0006 cudsw00a0007 cudsw00a0009 cudsw00a0011 cudsw00a0012 cudsw00a0013 cudsw00a0014 cudsw00a0015 cudsw00a0016"

##
##  test run
##

if [ "$1" = "-t" ]
then
  for Server in $VirtualServerList
  do
    ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'`
    echo "  $Server - $ServerID"
    for Device in `${ProgPrefix}/NewQuery fsr "select distinct device from iostat where serverid = ${ServerID}`
    do
      echo "    $Device"
    done  ##  for each Device
  done  ##  for each server

  exit

fi  ##  if test run

##
##  for real
##

for Server in $VirtualServerList
do
  ServerID=`${ProgPrefix}/NewQuery fsr "select id from server where name = '${Server}'`
  echo "  $Server - $ServerID"
  for Device in `${ProgPrefix}/NewQuery fsr "select distinct device from iostat where serverid = ${ServerID}`
  do
    ## echo "  $Device"
    Res=`echo $Device | egrep \:`
    if [ -z "$Res" ]
    then
      echo "    $Device is SAN"
${MYSQL} <<EOF
      use fsr;
      update iostat set devtype = 2 where serverid = ${ServerID} and device = '${Device}';
      update iostat58 set devtype = 2 where serverid = ${ServerID} and device = '${Device}';
EOF
    else
      echo "    $Device is NFS"
${MYSQL} <<EOF
      use fsr;
      update iostat set devtype = 4 where serverid = ${ServerID} and device = '${Device}';
      update iostat58 set devtype = 4 where serverid = ${ServerID} and device = '${Device}';
EOF
    fi  ##  if no : in device name

  done  ##  for each Device
done  ##  for each server
