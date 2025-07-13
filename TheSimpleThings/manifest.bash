#!/bin/bash

FEHLER_AUSGABE=/dev/stderr
MANIFEST_AUSGABE=/dev/null

function manifest (){
  echo $MANIFEST_AUSGABE
  echo -e "Manifest \nfür die Kategorie U3 des Kommandozeilenaufrufs von $0 \n\n(U steht für User interface (des scripts/tools .etc)\n" >$MANIFEST_AUSGABE
  echo -e "\n  U1 ist die grafische Oberfläche\n" >>$MANIFEST_AUSGABE
  echo -e "  U2 die Automatisierungsschnittstelle" >>$MANIFEST_AUSGABE
  echo -e "  U3 die Kommandozeile, also hier" >>$MANIFEST_AUSGABE
  echo -e "  U4 die Konfigurationsschnittstelle  (Konfigurationsdatei etc)" >>$MANIFEST_AUSGABE
  echo -e "\nDas Manifest beschreibt die SW Umgebungsplatform (P1)\n" >>$MANIFEST_AUSGABE
  echo -e "Davon ausgehend, das die Scriptausführungsleistung nicht ausschlaggebend ist, " >>$MANIFEST_AUSGABE
  echo -e "lassen wir da die HW Platform (P2) erstmal weg. Aber das kann sich ja noch ergeben" >>$MANIFEST_AUSGABE
  echo "+++++++++++++++++++++++++++++++++++++" >>$MANIFEST_AUSGABE
  echo "+++ was ist das für ein script ? ++++" >>$MANIFEST_AUSGABE
  echo "+++ Und noch wichtiger: Was ist die +" >>$MANIFEST_AUSGABE
  echo "+++ Umgebung in der es läuft? +++++++" >>$MANIFEST_AUSGABE
  echo "+++ also auf welcher Platform? ++++++" >>$MANIFEST_AUSGABE
  echo "+++++++++++++++++++++++++++++++++++++" >>$MANIFEST_AUSGABE
  echo -e "Scriptname und Ort\n   "$(readlink -f $0) >>$MANIFEST_AUSGABE
  echo "Daten dieses Aufrufs" >>$MANIFEST_AUSGABE
  echo "   Nutzer $(whoami)" >>$MANIFEST_AUSGABE
  echo "   Datum $(date +%Y-%m-%d)" >>$MANIFEST_AUSGABE
  echo "   Zeit $(date +%H:%M:%SUTC%:::z)" >>$MANIFEST_AUSGABE
  #
  # Da tut man die Kommandos rein, die man braucht
  #
  for welche_version in "uname -a" "bash --version" "git --version" "awk --version" "python3 --version" "file --version"
  do
    cmd=$(echo $welche_version | awk '{print $1}')
    echo -e "++++++++++++\n" Kommando \"$cmd\" >>$MANIFEST_AUSGABE
    echo -e "\n In welcher Version ist das Kommando \"$cmd\" verfügbar?\n "$(welche_version) "\n   Und wo liegt das Kommando? \n   $(ls -l $(which  $cmd))" >>$MANIFEST_AUSGABE
    if [ $? -ne 0 ]
    then
      echo "Fehler : Kommando $cmd nicht gefunden!" >/dev/stderr
      exit 1
    fi
  done

}

#
# Gib mal das Manifest aus!
#

MANIFEST_AUSGABE=/dev/stdout
manifest
