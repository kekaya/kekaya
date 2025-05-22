#! /bin/bash

#
# Was für ein Spass: UTF-8 characters in C / C++ source code
#

cd $(dirname $0)

SOURCE_CODE="/tmp/source.c"
UTML_CHECK_PROGRAMM="../utf82html.c"
UTML_CHECK_KOMPILAT="/tmp/utf82html.c"
EXECUTABLE="/tmp/source"
ASCII_RAUS_ODER_DOCH_UTF8="/tmp/source.txt"

echo "#include<stdio.h>" >$SOURCE_CODE
echo "int main(int argc, char **argv){" >>$SOURCE_CODE
echo "  fprintf(stdout,\"Wenn du tot bist, ist das nicht schlimm für Dich, nur für die anderen\\n\");" >>$SOURCE_CODE
echo "  fprintf(stdout,\"Genauso ist es, wenn Du blöd bist, es ist echt µ sam\\n\");" >>$SOURCE_CODE
echo "}" >>$SOURCE_CODE

echo "Source code Typ : $(file $SOURCE_CODE)" 
g++ $SOURCE_CODE -o $EXECUTABLE
if [ $? -ne 0 ]
then
  echo Kompilierung von $SOURCE_CODE nicht geklappt. Entweder g++ is nicht installiert oder der compile failed
  exit 1
fi
g++ $UTML_CHECK_PROGRAMM -o $UTML_CHECK_KOMPILAT

$EXECUTABLE >$ASCII_RAUS_ODER_DOCH_UTF8

echo Filetyp von Ausgabedatei : $(file $ASCII_RAUS_ODER_DOCH_UTF8)

echo "+++++++++++++++"
echo -e Und so sieht das Ausgabefile aus: "\n"
cat $ASCII_RAUS_ODER_DOCH_UTF8
echo "+++++++++++++++"
echo -e Und $ASCII_RAUS_ODER_DOCH_UTF8 in HTML "\n"
$UTML_CHECK_KOMPILAT $ASCII_RAUS_ODER_DOCH_UTF8
ret=$? 
echo +++++++++++++++++++++++++++++
if [ $ret -ne 0 ]
then
  echo "$ret Fehler passiert beim UTF-8 check"
else
  echo "UTF 8 check OK"
fi
echo +++++++++++++++++++++++++++++
MANIF_LINUX="uname -a"
MANIF_GPP="g++ --version"
echo -e "Linux version ($MANIF_LINUX) :\n $($MANIF_LINUX) \n\n Compiler version ($MANIF_GPP):\n $($MANIF_GPP) "





