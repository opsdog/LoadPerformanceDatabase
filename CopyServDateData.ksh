#!/bin/ksh
##
##  script to copy DougPerfData for the specified server and
##  collection date
##
##  use to load the Inputs directory to load the database
##

##  initialize stuff

ArchiveServer=quarter-pounder
ArchiveDir=/Volumes/External300/DBProgs/FSRServers/DougPerfData/Inputs

TargetDir=/home/douggreenwald/DBProgs/LoadDougPerf/Inputs

##  process args

if [ -z $2 ]
then
  echo "usage:  `basename $0` server colldate"
  exit 11
fi

ServerName=$1
CollDate=$2

##
##  let's roll
##

scp ${ArchiveServer}:${ArchiveDir}/${ServerName}/${ServerName}\*${CollDate}\* ${TargetDir}/

echo "gunzipping..."
gunzip -f ${TargetDir}/${ServerName}*${CollDate}*.gz
