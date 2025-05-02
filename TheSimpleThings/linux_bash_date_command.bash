#!/bin/bash

#
# was ist passiert ? 
# Ich wollte als Schriftführer der Tennisabteilung der Sportfreunde Dornstadt (https://sfd-dornstadt.de/)
# im Tennisplatzreservierungssystem GotCourts https://apps.gotcourts.com/
# eine Blockierung für ein zweiwöchentliches training vornehmen. 
# Tja. 
# Jetzt kann das Portal aber keine zweiwöchentliche Reservierung, nur ne wöchentliche Wiederholung. 
# Ich könnt natürlich jetzt rumexceln, aber da hab ich keine Lust. 
# deswegen mach ich mir jetzt einfach eine Liste der Termine , damit ich besser weiß, was ich händisch rauslöschen muss
# und zwar wie folgt:
#
# ==================================================================================
# SEITENSPIEL: Bei der Suche auf google bin ich nur über python Lösungen gestolpert.
#      Und ich mag python nicht besonders.
#      Liegt vielleicht daran, dass ich mir mal einen abgebrochen hab,
#      python in das uimage einer Linux distro auf einem embedded system 
#      mit begrenztem Speicher zu bringen. ein Drama. 
#      awk und sed sind die wahren tools für eine nachhaltige Zukunft
#      --> Hirn statt Masse. Und ein Bier dazu.
# ==================================================================================
# 
# Manifest: 
#   uname -a
#      Linux DESKTOP-27145I2 5.15.167.4-microsoft-standard-WSL2 #1 SMP Tue Nov 5 00:21:55 UTC 2024 x86_64 x86_64 x86_64 GNU/Linux
#   bash --version
#      GNU bash, version 5.2.21(1)-release (x86_64-pc-linux-gnu)
#   date --version
#      date (GNU coreutils) 9.4
#
# Beim nächsten Update/commit tu ich wahrscheinlich die Ticketnummer von Gotcourts mit rein 
#

STARTTAG="7 May 2025"
WOCHEN_IDENTIFIKATION_IM_DATE_COMMAND="week"
WOCHENWIDERHOLUNGSINTERVAL=2
WOCHENOFFSET_STARTWERT=0
ENDEDATUM=20251101

w=$WOCHENOFFSET_STARTWERT

weitermachen=1
while [ $weitermachen -eq 1 ]
do
  CONFIG_STRING="$STARTTAG  + $w $WOCHEN_IDENTIFIKATION_IM_DATE_COMMAND"
  VERGLEICHSDATUM=$(date +%Y%m%d -d "$CONFIG_STRING")
  if [ $VERGLEICHSDATUM -lt $ENDEDATUM ]
  then
    echo "Training am " $(date +%F -d "$CONFIG_STRING")
    let w=$w+$WOCHENWIDERHOLUNGSINTERVAL
  else
   weitermachen=0
  fi
done



