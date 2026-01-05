// Source written in 2004 by Peter Samuelson
// Copyright abandoned by the author

#define ERROR_FILE_OPEN_ISSUE   3
#define ERROR_ARGUMENT_ISSUE   2
#define ERROR_CODE_FILENAME_NOT_SET   1
#define ERROR_NO_ERROR_EXPECTED_USAGE 0

#define STRING_UTF_NOT_FOUND "[[NOT FOUND]]"

#define READ_FROM_FILE_BUFFER_IN_BYTES 4096

#define ASCII_CHAR_MASK 0x80

typedef struct {
    unsigned int utf8_codepoint;
    const char *html_entity;
} UTF8Mapping;

static const UTF8Mapping utf8_html_mapping[] = {
    // from https://www.w3schools.com/charsets/ref_html_ansi.asp
    // The Windows-1252 Character Set
    {0x80,"&euro;"},{0x82,"&sbquo;"},{0x83,"&fnof;"},{0x84,"&bdquo;"},{0x85,"&hellip;"},
    {0x86,"&dagger;"},{0x87,"&Dagger;"},{0x88,"&circ;"},{0x89,"&permil;"},{0x8a,"&Scaron;"},
    {0x8b,"&lsaquo;"},{0x8c,"&OElig;"},{0x8e,"&Zcaron;"},{0x91,"&lsquo;"},{0x92,"&rsquo;"},
    {0x93,"&ldquo;"},{0x94,"&rdquo;"},{0x95,"&bull;"},{0x96,"&ndash;"},{0x97,"&mdash;"},
    {0x98,"&tilde;"},{0x99,"&trade;"},{0x9a,"&scaron;"},{0x9b,"&rsaquo;"},{0x9c,"&oelig;"},
    {0x9e,"&zcaron;"},{0x9f,"&Yuml;"},
    // basis
    {0xA0, "&nbsp;"}, {0xA1, "&iexcl;"}, {0xA2, "&cent;"}, {0xA3, "&pound;"}, {0xA4, "&curren;"},
    {0xA5, "&yen;"}, {0xA6, "&brvbar;"}, {0xA7, "&sect;"}, {0xA8, "&uml;"}, {0xA9, "&copy;"},
    {0xAA, "&ordf;"}, {0xAB, "&laquo;"}, {0xAC, "&not;"}, {0xAD, "&shy;"}, {0xAE, "&reg;"},
    {0xAF, "&macr;"}, {0xB0, "&deg;"}, {0xB1, "&plusmn;"}, {0xB2, "&sup2;"}, {0xB3, "&sup3;"},
    {0xB4, "&acute;"}, {0xB5, "&micro;"}, {0xB6, "&para;"}, {0xB7, "&middot;"}, {0xB8, "&cedil;"},
    {0xB9, "&sup1;"}, {0xBA, "&ordm;"}, {0xBB, "&raquo;"}, {0xBC, "&frac14;"}, {0xBD, "&frac12;"},
    {0xBE, "&frac34;"}, {0xBF, "&iquest;"}, {0xC0, "&Agrave;"}, {0xC1, "&Aacute;"}, {0xC2, "&Acirc;"},
    {0xC3, "&Atilde;"}, {0xC4, "&Auml;"}, {0xC5, "&Aring;"}, {0xC6, "&AElig;"}, {0xC7, "&Ccedil;"},
    {0xC8, "&Egrave;"}, {0xC9, "&Eacute;"}, {0xCA, "&Ecirc;"}, {0xCB, "&Euml;"}, {0xCC, "&Igrave;"},
    {0xCD, "&Iacute;"}, {0xCE, "&Icirc;"}, {0xCF, "&Iuml;"}, {0xD0, "&ETH;"}, {0xD1, "&Ntilde;"},
    {0xD2, "&Ograve;"}, {0xD3, "&Oacute;"}, {0xD4, "&Ocirc;"}, {0xD5, "&Otilde;"}, {0xD6, "&Ouml;"},
    {0xD7, "&times;"}, {0xD8, "&Oslash;"}, {0xD9, "&Ugrave;"}, {0xDA, "&Uacute;"}, {0xDB, "&Ucirc;"},
    {0xDC, "&Uuml;"}, {0xDD, "&Yacute;"}, {0xDE, "&THORN;"}, {0xDF, "&szlig;"}, {0xE0, "&agrave;"},
    {0xE1, "&aacute;"}, {0xE2, "&acirc;"}, {0xE3, "&atilde;"}, {0xE4, "&auml;"}, {0xE5, "&aring;"},
    {0xE6, "&aelig;"}, {0xE7, "&ccedil;"}, {0xE8, "&egrave;"}, {0xE9, "&eacute;"}, {0xEA, "&ecirc;"},
    {0xEB, "&euml;"}, {0xEC, "&igrave;"}, {0xED, "&iacute;"}, {0xEE, "&icirc;"}, {0xEF, "&iuml;"},
    {0xF0, "&eth;"}, {0xF1, "&ntilde;"}, {0xF2, "&ograve;"}, {0xF3, "&oacute;"}, {0xF4, "&ocirc;"},
    {0xF5, "&otilde;"}, {0xF6, "&ouml;"}, {0xF7, "&divide;"}, {0xF8, "&oslash;"}, {0xF9, "&ugrave;"},
    {0xFA, "&uacute;"}, {0xFB, "&ucirc;"}, {0xFC, "&uuml;"}, {0xFD, "&yacute;"}, {0xFE, "&thorn;"},
    {0xFF, "&yuml;"},
    // UTF-8 from Windows-1252 Character Set imported 
    {0x178, "&Yuml;"}
};

#define VALUE_RANGE_U8 512
static const char *hex2html_array[VALUE_RANGE_U8];


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//#define DBG_PRINT

#define CHAT_GPT_WATERMARK 0x202f
#define DEBUG_BUFFER_IN_BYTES 10

int global_utf_return_code = 0;

unsigned anzahl_an_chatgpt_watermarks = 0;

void help(char *prg) {
   fprintf(stderr,"usage %s \n",prg);
   fprintf(stderr,"  arguments\n");
   fprintf(stderr,"  -H : print HTML header trailer\n");
   fprintf(stderr,"  -i <input file>:UTF-8 input file to be converted\n");
   fprintf(stderr,"  -d: print HTML list of all mappings and quit\n");
   fprintf(stderr,"  -v: debug printing\n");
   exit(0);
}


char memory4x[100];

const char * plain_ascii_check(){
  return (const char *)NULL;
}

const char*  hex2html(unsigned int ch){
  int found=0;
  ch &= (VALUE_RANGE_U8-1);
  return hex2html_array[ch];
  /*
  switch(ch){
    case 0xe4:
      return "&auml;";
      break;
    case 0xf6:
      return "&ouml;";
      break;
    case 0xfc:
      return "&uuml;";
      break;
    case 0xc4:
      return "&Auml;";
      break;
    case 0xd6:
      return "&Ouml;";
      break;
    case 0xdc:
      return "&Uuml;";
      break;
    case 0xdf:
      return "&szlig;";
      break;
    case 0xf4:
      return "&ocirc;";
      break;
    case 0xe0:
      return "&aacute;";
      break;
    // U+00B5	µ	c2 b5	MICRO SIGN
    // https://www.utf8-chartable.de/
    case 0xb5:
      return "&micro;";
      break;
    default:
      sprintf(memory4x,"<b>UTF-8 char not found(0x%02x)</b>",ch);
      return (const char *)memory4x;
  }
  */
}

unsigned char debug_buffer[DEBUG_BUFFER_IN_BYTES];
int  debug_buffer_pointer=0;

void initialize_buffers(void){
  int i;
  for(i=0;i<DEBUG_BUFFER_IN_BYTES;i++){
    debug_buffer[i]=0;
  }
}
void debug_buffer_update(char chr){
  debug_buffer[debug_buffer_pointer] = (unsigned char)chr;
  debug_buffer_pointer++;
  if(debug_buffer_pointer == DEBUG_BUFFER_IN_BYTES){
    debug_buffer_pointer=0;
  }
}


void from_utf8 (FILE *in, FILE *out, bool dbg)
{
   char work_buffer[READ_FROM_FILE_BUFFER_IN_BYTES + 17];
   size_t buffer_length = 0;

   while (!feof(in)) {
      size_t usable, i;

      buffer_length += fread(work_buffer+buffer_length, 1, READ_FROM_FILE_BUFFER_IN_BYTES, in);
      if(dbg){
        fprintf(stderr,"DBG : buffer_length %d : \n",(int)buffer_length);
      }
      work_buffer[buffer_length] = 0;
      if (feof(in) || buffer_length < 16)
         usable = buffer_length;
      else
         usable = buffer_length - 16;

      for (i=0; i<usable; i++) {
         int utf_prop_index;
         unsigned ch, usable_len;
         debug_buffer_update(work_buffer[i]);
         /* properties of UTF-8 chars */
         static struct { unsigned minval; int len; char b0, b0mask; }
         uprop[] = {
            { 1<<7,  2, (char)0xc0, (char)0xe0 },
            { 1<<11, 3, (char)0xe0, (char)0xf0 },
            { 1<<16, 4, (char)0xf0, (char)0xf8 },
            { 1<<21, 5, (char)0xf8, (char)0xfc },
            { 1<<26, 6, (char)0xfc, (char)0xfe },
            { 0, 0, 0, 0}
         };
         // CHECK 1: If ASCII , simply print
         if (!(work_buffer[i] & ASCII_CHAR_MASK)) {
            plain_ascii_check();
            #ifdef DBG_PRINT
              printf("DBG : plain ASCII (pos %d) %c \n",i,work_buffer[i]);
            #else
              putc(work_buffer[i], out);
            #endif //DBG_PRINT

            continue;
         }
         // CHECK 2: Bitmask of checked character indicates the properties for the next chars
         for (utf_prop_index = 0; uprop[utf_prop_index].minval; utf_prop_index++){
            if ((work_buffer[i] & uprop[utf_prop_index].b0mask) == uprop[utf_prop_index].b0){
               break;
            }
         }
         if (!uprop[utf_prop_index].minval){
            fprintf(stderr, "UTF-8 minimum value check of 0x%02x failed\n",(unsigned char)work_buffer[i]);
            goto invalid;
         }

         ch = work_buffer[i] & ~uprop[utf_prop_index].b0mask;
         for (usable_len = uprop[utf_prop_index].len - 1; usable_len; usable_len--,i++) {
            debug_buffer_update(work_buffer[i+1]);
            if ((work_buffer[i+1] & 0xc0) != 0x80){
               fprintf(stderr, "UTF-8 wrong bitmap: coding length: %d, char %d of %d \n",uprop[utf_prop_index].len,uprop[utf_prop_index].len-usable_len,uprop[utf_prop_index].len);
               goto invalid;
            }

            ch = (ch << 6) | (work_buffer[i+1] & 0x3f);
         }
         if (ch < uprop[utf_prop_index].minval){
            fprintf(stderr, "UTF-8 : minimum value %d >= %d\n",uprop[utf_prop_index].minval,ch);
            goto invalid;
         }

         if(ch >= VALUE_RANGE_U8){
            #ifdef DBG_PRINT
              printf("DBG : UTF8 ASCII NUM (pos %d) %s \n",i,ch);
            #else
              fprintf(out, "&#x%04x;", ch);
              if(ch == CHAT_GPT_WATERMARK){
                 anzahl_an_chatgpt_watermarks++;
              }
            #endif //DBG_PRINT
         }else{
            #ifdef DBG_PRINT
              printf("DBG : UTF8 ASCII HTML (pos %d) %s \n",i,hex2html(ch));
            #else
              fprintf(out, "%s", hex2html(ch));
            #endif //DBG_PRINT
         }
         continue;

      invalid:
         fprintf(stderr, "illegal UTF-8 sequence\n");
         global_utf_return_code++;
         fprintf(out, "&#xfffd;"); /* "REPLACEMENT CHARACTER" */
      }
      if (i < buffer_length)
         memcpy(work_buffer, work_buffer+i, buffer_length-i);
      buffer_length -= i;
   }
}

void init_all_supported_utf_8(void){
   static const char*char_not_found = STRING_UTF_NOT_FOUND;
   int found;
   for (unsigned int j=0;j<VALUE_RANGE_U8;j++){
       found=0;
       for (int i = 0; i< sizeof(utf8_html_mapping)/sizeof(UTF8Mapping);i++){
         if(j==utf8_html_mapping[i].utf8_codepoint){
           hex2html_array[j] = utf8_html_mapping[i].html_entity;
           found = 1;
           break;
         }
         //printf("%02x --> %s\n",utf8_html_mapping[i].utf8_codepoint,utf8_html_mapping[i].html_entity);
       }
       if(found == 0){
         hex2html_array[j] = char_not_found;
       }
   }
}

void print_all_supported_utf_8(void){
   printf("<html><title>dec hex to HTML</title><body><table border=1>\n");
   printf("<tr><td>dec</td> <td>hex</td> <td>char</td> </tr>\n");

   for (unsigned int j=0;j<VALUE_RANGE_U8;j++){
     printf("<tr><td>%3d</td> <td>%02x</td> <td>%s</td> </tr>\n",j,j,hex2html_array[j]);   
   }  
   printf("</table></body></html>\n");
}


int main (int argc, char *argv[])
{
   char *filename = NULL;
   char *prg = argv[0];
   bool HTML_header_trailer = false;   
   bool dbg = false;   

   // init mapping of UTF-8 characters
   init_all_supported_utf_8();
   initialize_buffers();
   
   if (argc == 1) {
      help(prg);
      return 0;
   }
   while (argc > 1) {
      if (argv[1][0] == '-' ) {
         switch(argv[1][1]) {
            case 'h': 
              help(prg); 
              exit(0);
              break;
            case 'H': 
              HTML_header_trailer = true; 
              exit(0);
              break;
            case 'i': 
              if(argc>2){
                filename = argv[2];
              }else{
                fprintf(stderr,"ERROR : not enough arguments for filename\n");                
                exit(ERROR_ARGUMENT_ISSUE);
              }
              break;
            case 'd': 
              print_all_supported_utf_8();
              exit(ERROR_NO_ERROR_EXPECTED_USAGE);
              break;
            case 'v': 
              dbg = true;
              break;
         };
      }
      argc--; argv++;
   }
   
      if(filename == NULL){
        fprintf(stderr,"ERROR : filename of file to be opened not set\n");
        help(prg);
        exit(ERROR_CODE_FILENAME_NOT_SET);
      }
      FILE *fp = fopen(filename, "r");
      if(fp == NULL){
        fprintf(stderr,"ERROR: file %s could not beopened\n",filename);
        exit(ERROR_FILE_OPEN_ISSUE);
      }
      if(HTML_header_trailer){
        printf("<html>\n  <title>test</title>\n<body>\n");
      }
      if (fp)
         from_utf8(fp, stdout,dbg);
      else
         perror("fopen");

      fclose(fp);
      if(HTML_header_trailer){
        printf("</body>\n<html>\n");
      }

   if(anzahl_an_chatgpt_watermarks>0){
     fprintf(stderr,"Da hat wohl der chat GPT %d mal seine Finger beim Text schreiben dringehabt\nruf mal \n./utf82html %s | grep %x 2>/dev/null\nauf, um  die Watermark Textstellen zu sehen\n",anzahl_an_chatgpt_watermarks,argv[0],CHAT_GPT_WATERMARK);
   }
   return global_utf_return_code;
}
