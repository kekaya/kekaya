// Source written in 2004 by Peter Samuelson
// Copyright abandoned by the author


typedef struct {
    unsigned int utf8_codepoint;
    const char *html_entity;
} UTF8Mapping;

static const UTF8Mapping utf8_html_mapping[] = {
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
    {0xFF, "&yuml;"}
};

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//#define DBG_PRINT

#define CHAT_GPT_WATERMARK 0x202f


int global_utf_return_code = 0;

unsigned anzahl_an_chatgpt_watermarks = 0;

void help() {
   fprintf(stdout,"$doc");
   exit(0);
}


char memory4x[100];

const char * plain_ascii_check(){
  return (const char *)NULL;
}

const char*  hex2html(unsigned int ch){
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
}

void from_utf8 (FILE *in, FILE *out)
{
   char buf[4096 + 17];
   size_t len = 0;

   while (!feof(in)) {
      size_t usable, i;

      len += fread(buf+len, 1, 4096, in);
      #ifdef DBG_PRINT
        printf("DBG : len %d : ",(int)len);
      #endif //DBG_PRINT
      buf[len] = 0;
      if (feof(in) || len < 16)
         usable = len;
      else
         usable = len - 16;

      for (i=0; i<usable; i++) {
         int t;
         unsigned ch, len;

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

         if (!(buf[i] & 0x80)) {
            plain_ascii_check();
            #ifdef DBG_PRINT
              printf("DBG : plain ASCII (pos %d) %c \n",i,buf[i]);
            #else
              putc(buf[i], out);
            #endif //DBG_PRINT

            continue;
         }
         for (t = 0; uprop[t].minval; t++)
            if ((buf[i] & uprop[t].b0mask) == uprop[t].b0)
               break;
         if (!uprop[t].minval)
            goto invalid;

         ch = buf[i] & ~uprop[t].b0mask;
         for (len = uprop[t].len - 1; len; len--,i++) {
            if ((buf[i+1] & 0xc0) != 0x80)
               goto invalid;

            ch = (ch << 6) | (buf[i+1] & 0x3f);
         }
         if (ch < uprop[t].minval)
            goto invalid;

         if(ch > 255){
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
      if (i < len)
         memcpy(buf, buf+i, len-i);
      len -= i;
   }
}

int main (int argc, char *argv[])
{
   /* for (int i = 0; i< sizeof(utf8_html_mapping)/sizeof(UTF8Mapping);i++){
     printf("%02x --> %s\n",utf8_html_mapping[i].utf8_codepoint,utf8_html_mapping[i].html_entity);
   }
   return 1;
   */

   if (argc == 1) {
      from_utf8(stdin, stdout);
      return 0;
   }
   while (argc > 1) {
      if (argv[1][0] == '-' ) {
         switch(argv[1][1]) {
            case 'h': help(); exit(0);
            case 'V': printf("$version\n"); exit(0);
         };
      }
      FILE *fp = fopen(argv[1], "r");
      if (fp)
         from_utf8(fp, stdout);
      else
         perror("fopen");

      fclose(fp);
      argc--; argv++;
   }
   if(anzahl_an_chatgpt_watermarks>0){
     fprintf(stderr,"Da hat wohl der chat GPT %d mal seine Finger beim Text schreiben dringehabt\nruf mal \n./utf82html %s | grep %x 2>/dev/null\nauf, um  die Watermark Textstellen zu sehen\n",anzahl_an_chatgpt_watermarks,argv[0],CHAT_GPT_WATERMARK);
   }
   return global_utf_return_code;
}
