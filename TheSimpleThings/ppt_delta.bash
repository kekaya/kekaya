#!/bin/bash
#
# easy PPT compare 
# you need utilities
#   unzip
#   uname
#   mkdir
#   readlink
#   md5sum
#   file
#   grep 
#   date
#   awk
#   rm
#
#   ...and two versions of a PPTX file to compare
#
# NOTE : if you should have two folders /tmp/f1 and /tmp/f2 , they are simply deleted when you call this script, just search for "rm" in this script
#



RESULT_FILE_TRUNK="/tmp/ppt1_vs_ppt2_$(date +%Y_%m_%d__%H_%M_%S)"

if [ $# -lt 2 ]
then 
  echo "usage : $0 <ppt 1> <ppt 2> [trunk for result path , default: $RESULT_FILE_TRUNK ]"
  exit 1
fi
if [ $# -gt 2 ]
then 
  RESULT_FILE_TRUNK=$3
fi

HERE=$(pwd)
RESULT_FILE="${RESULT_FILE_TRUNK}.log"
RESULT_HTML="${RESULT_FILE_TRUNK}.html"

PPT1=$(readlink -f $1)
PPT2=$(readlink -f $2)

FOLDER_1=/tmp/f1
FOLDER_2=/tmp/f2

echo "User $(whoami)" >$RESULT_FILE
echo "Date $(date)" >>$RESULT_FILE
echo "Hostname $(hostname)" >>$RESULT_FILE
echo "distro  $(uname -a)" >>$RESULT_FILE
echo -e "\nfile 1 : $(file $PPT1)\nfile 2 : $(file $PPT2)" >>$RESULT_FILE
echo -e "\nmd5 checksum file 1 : $(md5sum $PPT1)\nmd5 checksum file 2 : $(md5sum $PPT2)\n----------------------\n" >>$RESULT_FILE

rm -rf $FOLDER_1;mkdir $FOLDER_1
rm -rf $FOLDER_2;mkdir $FOLDER_2

cd $FOLDER_1
unzip "$PPT1"
grep -oP '(?<=<a:t>).*?(?=</a:t>)' ppt/slides/*.xml > slides.txt
cd $FOLDER_2
unzip "$PPT2"
grep -oP '(?<=<a:t>).*?(?=</a:t>)' ppt/slides/*.xml > slides.txt

diff  ${FOLDER_1}/slides.txt ${FOLDER_2}/slides.txt >>$RESULT_FILE

awk  -v html=$RESULT_HTML -v left="$(basename $PPT1)" -v right="$(basename) $PPT2" -v ascii_result=$RESULT_FILE 'BEGIN{
         g_cnt=0;
         state="idle"
         if(!html){
           html = "/mnt/c/Temporary_private/x.html"
         }
         if(!title){
           title = "left " ascii2html(left) ", right " ascii2html(right)
         }
        }
        {
          print "++++" NR "++++["g_cnt"]["state"]" $0 >"/tmp/x"
        }
        is_new_diff($0){
          print " is new diff ---> " $0
          g_cnt++;
          state = "left"
          next
        }
        $0 == "---"{
          state = "right"
          next
        }
        state != "idle"{
          n = split(ascii2html($0),a,/xml:/);
          g_slide = substr(a[1],length(a[1])-1,1)
          print "   register count " g_cnt ", " state ", slide " g_slide ": " a[2] >"/tmp/x"
          if(str[g_cnt,state]){
            str[g_cnt,state] = str[g_cnt,state] "<br>"
          }
          str[g_cnt,state] = str[g_cnt,state] a[2]
          slide[g_cnt] = g_slide;
        }
        function ascii2html(str){
           gsub(/\</,"&lt;",str) ;gsub(/\>/,"&gt;",str);gsub(/\&/,"&amp;",str);
           return str;
        }

        END{
          print "<html><title>"title"</title><body>" >html
          print "<table border=1>" >html
          print "<tr>" >html
          print "  <td>slide NR</td>" >html
          print "  <td>left side <br><b>"ascii2html(left)"</b> </td>" >html
          print "  <td>right side <br><b>"ascii2html(right)"</b></td>" >html
          print "</tr>" >html
          for (i=1;i<=g_cnt;i++){
            print "<tr>" >html
            print "  <td>"slide[i]"</td>" >html
            print "  <td>"str[i,"left"]"</td>" >html
            print "  <td>"str[i,"right"]"</td>" >html
            print "</tr>"
          }
          print "</table>" >html
          print "<hr>ASCII result file:<br><pre>"  >html
          while (getline x < ascii_result >0){
            print ascii2html(x)  >html
          }
          print "</pre>"  >html
          print "</body></html>" >html
          print "open "html
        }

        function is_new_diff(str,_n,_a){
          _n = split(str,_a,/c/)
          #print "   is_new_diff _n = " _n
          if(_n != 2){
            return 0;
          }
          if(isnum(_a[1]) && isnum(_a[2])){
            return 1;
          }
          return 0;
        }

        function isnum(num,_l,_i,_sub,_ver){
          _l = length(num);
          #print "    isnum " num "--> " _l
          for(_i = 1;_i<=_l;_i++){
            _sub = substr(num,_i,1)
            # print "    isnum " num "["_i ","_sub"]"

            if(_sub !~ /[0-9,]/){
              # print _sub " : " _i " ERROR --> " num
              return 0;
            }
          }
          return 1;
        }' $RESULT_FILE

echo see $RESULT_FILE for delta


