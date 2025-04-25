// Source written in 2004 by Peter Samuelson
// Copyright abandoned by the author

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//#define DBG_PRINT


void help() {
   fprintf(stdout,"$doc");
   exit(0);
}


char memory4x[100];

const char * plain_ascii_check(){
  return (const char *)NULL;
}

const char*  hex2html(unsigned ch){
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
    case 0xe0:
      return "&aacute;";
      break;
    default:
      sprintf(memory4x,"<b>failed(0x%02x)</b>",ch);
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
         fprintf(out, "&#xfffd;"); /* "REPLACEMENT CHARACTER" */
      }
      if (i < len)
         memcpy(buf, buf+i, len-i);
      len -= i;
   }
}

int main (int argc, char *argv[])
{
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
   return 0;
}
