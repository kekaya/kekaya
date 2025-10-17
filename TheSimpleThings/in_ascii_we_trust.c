#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
/*
  this is a ASCII log integrity check 
  usage 
  
  FILE=in_ascii_we_trust.c
  diff  <(g++ in_ascii_we_trust.c -o /tmp/in_ascii_we_trust; /tmp/in_ascii_we_trust -i $FILE) $FILE
  IS_NO_ASCII=$?
  echo $IS_NO_ASCII
*/


// code generation
// to be added
// man isprint | awk '{gsub(/[\(\)]/," ");}$1 == "int" && $2 !~ /_l$/{list[$2]=1}END{s="</td><td>";title="<html><title>ASCII on C print functions</title><body>\\n<table border=1>\\n<tr><td>" s;printstr="printf(\"<tr><td>dec"s"%3d"s"hex%02x"s; for(i in list){title = title i s s;printstr= printstr " "i s"%d" s;val=val "," i"(c)"} ; print "\nprintf(\""title "</td></tr>\");\n\n" printstr "</td></tr>\\n\",c,c"val");"}'
// 

#define MIN_VAL_OF_UINT8 0
#define MAX_VAL_OF_UINT8 255
#define MAX_STRLEN_FOR_PRINTABLE_INFO 100

#define RET_WRONG_USAGE -1
#define RET_OK 0
#define RET_ERROR 1
#define RET_NON_ASCII_INCIDENT 2

#define SILENT_REPLACE_CHAR " "

typedef enum EPrintMode {
  EMode_NormalDebug,
  EMode_PutPlain_ASCII
} EPrintMode_t;

typedef struct replace_chars {
  uint8_t fix_char;
  const char *format_str;
} replace_chars_t;

replace_chars_t rep_chars[] = {
  {0xb, "[[Vertical tab 0x%02x]]"},
  {0xc, "[[Form feed 0x%02x]]"},
  {0xd, "[[Windows carriage return 0x%02x]]"}
};

uint32_t counter[MAX_VAL_OF_UINT8+1];

const uint8_t* linebreakcheck(uint8_t,int* ret,EPrintMode_t mode);

void help(char *prgname){
  fprintf(stderr,"Usage \n  %s -t <generic output table file[html]> -i <input file for printing> [-s suppress verbose printing(force ASCII version as output)]\n",prgname);
}

int html_output(char *html_file){
  uint8_t c;
  FILE *h=fopen(html_file,"w");
  if(h == NULL){
    fprintf(stderr,"HTML file %s could not be opened\n",html_file);
    return RET_ERROR;
  }else{
    fprintf(h,"<html><title>ASCII on C print functions</title><body>\n<table border=1>\n<tr><td></td><td></td><td></td><td></td><td>isspace</td><td></td><td>ispunct</td><td></td><td>isascii</td><td></td><td>isprint</td><td></td><td>iscntrl</td><td></td><td>isdigit</td><td></td><td>isalnum</td><td></td><td>isalpha</td><td></td><td>isblank</td><td></td><td>islower</td><td></td><td>isupper</td><td></td><td>isxdigit</td><td></td><td>isgraph</td><td></td></tr>");
    for (c=MIN_VAL_OF_UINT8;c<MAX_VAL_OF_UINT8;c++){
          fprintf(h,"<tr><td>dec</td><td>%3d</td><td>hex%02x</td><td> isspace</td><td>%d</td><td> ispunct</td><td>%d</td><td> isascii</td><td>%d</td><td> isprint</td><td>%d</td><td> iscntrl</td><td>%d</td><td> isdigit</td><td>%d</td><td> isalnum</td><td>%d</td><td> isalpha</td><td>%d</td><td> isblank</td><td>%d</td><td> islower</td><td>%d</td><td> isupper</td><td>%d</td><td> isxdigit</td><td>%d</td><td> isgraph</td><td>%d</td><td></td></tr>\n",c,c,isspace(c),ispunct(c),isascii(c),isprint(c),iscntrl(c),isdigit(c),isalnum(c),isalpha(c),isblank(c),islower(c),isupper(c),isxdigit(c),isgraph(c));
    }
    fprintf(h,"</table></body></html>");
    fclose(h);
  }
  return RET_OK;

}

int main (int argc,char **argv){
  uint8_t c;
  int readin_c,returnval = RET_OK,check,i;
  char *filename = (char *)"";
  FILE *infile;
  EPrintMode_t printmode=EMode_NormalDebug;
  
  for(i=0;i<=MAX_VAL_OF_UINT8;i++){
    counter[i] = 0;
  }
  

  // no arg : print the table of byte/uint8 values and the printable characteristics, Hauska joulua ja hyyvää utta vuotta
  if(argc<2){
  
    help(argv[0]);
    exit(RET_OK);

  // take argument 1, open it binary and check if ASCII
  }else{
    for(i=1;i<argc;i++){
      //printf("Arg %d: %s\n",i,argv[i]);
      check = RET_ERROR;
      /*
        -t <HTML file for table written by program>
      */
      if(!strcmp("-t",argv[i])){
         if(i<argc-1){
           html_output(argv[i+1]);
           fprintf(stderr,"HTML table written to %s\n",argv[i+1]);
           check = RET_OK;
           i++;
           continue;
         }else{
           fprintf(stderr,"error not enough arguments for option -t\n");
           exit(RET_ERROR);
         }
      /*
        -i <input file for analysis>
      */
      }else if(!strcmp("-i",argv[i])) {
         if(i<argc-1){
           filename = argv[i+1];
           fprintf(stderr,"input file %s\n",filename);
           check = RET_OK;
           i++;
           continue;
         }else{
           fprintf(stderr,"error not enough arguments for option -i\n");
           exit(RET_ERROR);
         }
      /*
        -s: suppress mode: replace syspicious characters in file -i with SILENT_REPLACE_CHAR
      */
      }else if(!strcmp("-s",argv[i])){
        printmode = EMode_PutPlain_ASCII;
        check = RET_OK;
        continue;
      }
      
      fprintf(stderr,"ERROR unknown arg %s \n",argv[i]);
      exit(RET_ERROR);
      
    }
    infile = fopen(filename,"r");
    if(infile == NULL){
      fprintf(stderr,"Opening file %s failed\n",argv[1]);
      exit(RET_WRONG_USAGE);
    }else{
      /* read all chars from unknown file*/
      while( (readin_c = fgetc(infile)) != EOF){
        c=(uint8_t)readin_c;
        if(isprint(c) || isspace(c)){
        //if(isprint(c) || isspace(c) ||isascii(c)  ){
          printf("%s",linebreakcheck(c,&returnval,printmode));
        }else{
          returnval = RET_NON_ASCII_INCIDENT;
          if(printmode == EMode_NormalDebug){
            printf("[[%02x]]",c);
          }else{
            printf("%s",SILENT_REPLACE_CHAR);
          }
          counter[c]++;
        }
      }
      fclose(infile);
      
    }
  }
  if(returnval == RET_NON_ASCII_INCIDENT && printmode == EMode_NormalDebug){
    for(i=0;i<=MAX_VAL_OF_UINT8;i++){
      if(counter[i]>0){
        fprintf(stderr,"CODE 0x%02x : count %d\n",i,counter[i]);
      }
    }
  }
  return returnval;
}

const uint8_t* linebreakcheck_org(uint8_t cin,int* ret,EPrintMode_t printmode){
  static uint8_t cback[MAX_STRLEN_FOR_PRINTABLE_INFO];
  if(cin == 0xb){
    if(printmode == EMode_NormalDebug){
      sprintf((char*)cback,"[[Vertical tab 0x%02x]]",cin);
    }else{
      sprintf((char*)cback,"%s",SILENT_REPLACE_CHAR);
    }
    *ret=RET_NON_ASCII_INCIDENT;
    return cback;
  }
  if(cin == 0xc){
    sprintf((char*)cback,"[[Form feed 0x%02x]]",cin);
    *ret=RET_NON_ASCII_INCIDENT;
    return cback;
  }
  if(cin == 0xd){
    sprintf((char*)cback,"[[Windows carriage return 0x%02x]]",cin);
    *ret=RET_NON_ASCII_INCIDENT;
    return cback;
  }

  sprintf((char*)cback,"%c",cin);
  return cback;    
}

const uint8_t* linebreakcheck(uint8_t cin,int* ret,EPrintMode_t printmode){
  static uint8_t cback[MAX_STRLEN_FOR_PRINTABLE_INFO];
  int i;
  for (i=0;i<sizeof(rep_chars)/sizeof(replace_chars_t);i++){
      if(cin == rep_chars[i].fix_char){
        if(printmode == EMode_NormalDebug){
          sprintf((char*)cback,rep_chars[i].format_str,cin);
        }else{
          sprintf((char*)cback,"%s",SILENT_REPLACE_CHAR);
        }
        *ret=RET_NON_ASCII_INCIDENT;
        return cback;
      }
  }

  sprintf((char*)cback,"%c",cin);
  return cback;    
}

