#!/bin/bash

PUBLICHOLIDAYS_ON_WEEKDAYS="Karfreitag, Ostermontag, Pfingstmontag, Christi Himmelfahrt, Fronleichnam"
HOLIDAYSDAYS=30
HO_DAYS=120
STRECKE_ZUR_ARBEIT_KM=8.8

if [ $# -lt 1 ]
then
  echo "Benutzung: $0 <Jahr>"
  exit 
fi
JAHR=$1

STARTDAY="1 Jan $JAHR"

CNT=0
WEEKDAYS=0
echo "date -d \"$STARTDAY + $CNT days\""
while [ "$(date +%Y -d "$STARTDAY + $CNT days")" == "$JAHR" ]
do
  DSTR="$STARTDAY + $CNT days"
  if [ "$(date +%w -d "$DSTR" | awk '{if($1 == 0 || $1 == 6){print "0"}else{print "1"}}')" -ne 0 ]
  then
   let WEEKDAYS=$WEEKDAYS+1
  #else
  #  echo "Wochenende "
  #  date -d "$DSTR"
  fi
  let CNT=$CNT+1
done

echo "Tage im Jahr " $CNT ", Wochentage im Jahr" $WEEKDAYS
FEIERTAGE_DATEI=/tmp/feiertage
FEIERTAGE_STRING="%w %a %F"
rm -f $FEIERTAGE_DATEI
date +"$FEIERTAGE_STRING" -d "Jan 1 $JAHR" >>$FEIERTAGE_DATEI # Neujahr
date +"$FEIERTAGE_STRING" -d "Jan 6 $JAHR" >>$FEIERTAGE_DATEI # Heilige drei Könige
date +"$FEIERTAGE_STRING" -d "May 1 $JAHR" >>$FEIERTAGE_DATEI # Tag der Arbeit
date +"$FEIERTAGE_STRING" -d "Oct 3 $JAHR" >>$FEIERTAGE_DATEI # Tag der Deutschen Einheit
date +"$FEIERTAGE_STRING" -d "Nov 1 $JAHR" >>$FEIERTAGE_DATEI # Allerheiligen
date +"$FEIERTAGE_STRING" -d "Dec 25 $JAHR" >>$FEIERTAGE_DATEI # 1. Weihnachtsfeiertag
date +"$FEIERTAGE_STRING" -d "Dec 26 $JAHR" >>$FEIERTAGE_DATEI # 2. Weihnachtsfeiertag
date +"$FEIERTAGE_STRING" -d "Dec 31 $JAHR" >>$FEIERTAGE_DATEI # halber Feiertag mit 24.12.
echo feste Feiertage mit variablem Wochentag
cat $FEIERTAGE_DATEI
FEIERTAGE_NICHT_AM_WOCHENDE=$(awk 'BEGIN{cnt=0}{if($1 >0 && $1 < 6) cnt++}END{print cnt}' $FEIERTAGE_DATEI)
echo "Jahr $JAHR:"
echo "Feiertage nicht am Wochenende $FEIERTAGE_NICHT_AM_WOCHENDE" 
let ARBEITSTAGE_OHNE_URLAUB=$WEEKDAYS-$FEIERTAGE_NICHT_AM_WOCHENDE
echo "Arbeitstage  $ARBEITSTAGE_OHNE_URLAUB" 

NUM_PUBLICHOLIDAYS_ON_WEEKDAYS=$(echo $PUBLICHOLIDAYS_ON_WEEKDAYS | awk 'BEGIN{FS="[,]+"}{print NF}')
echo "Anzahl Feiertage auf Wochentagen: $NUM_PUBLICHOLIDAYS_ON_WEEKDAYS : $PUBLICHOLIDAYS_ON_WEEKDAYS "
let ARBEITSTAGE_OHNE_URLAUB__MINUS_FEIERTAGE=$ARBEITSTAGE_OHNE_URLAUB-$NUM_PUBLICHOLIDAYS_ON_WEEKDAYS

let ARBEITSTAGE_MIT_URLAUB=$ARBEITSTAGE_OHNE_URLAUB__MINUS_FEIERTAGE-$HOLIDAYSDAYS
echo "Arbeitstage abzüglich $HOLIDAYSDAYS Tage Urlaub  $ARBEITSTAGE_MIT_URLAUB" 
let ARBEITSTAGE_AM_STANDORT=$ARBEITSTAGE_MIT_URLAUB-$HO_DAYS
echo "Arbeitstage am Standort  $ARBEITSTAGE_AM_STANDORT" 
echo "Arbeitstage im Homeoffice  $HO_DAYS" 

FAHRT_ZUR_ARBEIT_EINFACH_KM=$(echo "scale=2;${ARBEITSTAGE_AM_STANDORT}*${STRECKE_ZUR_ARBEIT_KM}" | bc)
echo " Fahrtstrecke $STRECKE_ZUR_ARBEIT_KM km x $ARBEITSTAGE_AM_STANDORT = $FAHRT_ZUR_ARBEIT_EINFACH_KM km"






