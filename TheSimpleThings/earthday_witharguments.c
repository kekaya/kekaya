#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

struct char2num { 
                                    const char *str;
                                    int num;
                                  };
/*
  generating the Weekday to number mapping
  for i in 0 1 2 3 4 5 6 7 ; do  date  +%A__%w -d "+$i days" | awk '{gsub("__"," ");print "{\"" $1 "\","$2"}," }'; done
*/

struct char2num weekdayASCII_2_weekday_english[] = {
                                                    {"Monday",1},
                                                    {"Tuesday",2},
                                                    {"Wednesday",3},
                                                    {"Thursday",4},
                                                    {"Friday",5},
                                                    {"Saturday",6},
                                                    {"Sunday",0}
                                                };

/*
  generating the Month to number mapping
  for i in  1 2 3 4 5 6 7 8 9 10 11 12 ; do  date  +%B__%m -d "+$i months" | awk '{gsub("__"," ");print "{\"" $1 "\","sprintf("%d",$2)"}," }'; done
*/                                  
struct char2num monthASCII_2_num_english[] = {
                                                        {"June",6},
                                                        {"July",7},
                                                        {"August",8},
                                                        {"September",9},
                                                        {"October",10},
                                                        {"November",11},
                                                        {"December",12},
                                                        {"January",1},
                                                        {"February",2},
                                                        {"March",3},
                                                        {"April",4},
                                                        {"May",5}
                                                };
                                  


/**
 * Get the last occurrence of a given weekday in a month/year.
 * weekday: 0 = Sunday, 1 = Monday, ..., 6 = Saturday
 */
 
#define MIN_YEAR 1900
#define DAYS_PER_WEEK 7
struct tm test_date_function(int year, int month, int weekday) {
    struct tm date_info = {0};


    // Start from the last day of the month
    date_info.tm_year = year - MIN_YEAR; // tm_year is years since 1900
    date_info.tm_mon = month;        // Set to next month
    date_info.tm_mday = 0;           // Day 0 means "last day of previous month"
    mktime(&date_info);              // Normalize to get last day of given month

    printf("tm_sec: %d,\ntm_min: %d,\ntm_hour: %d,\ntm_mday: %d,\ntm_mon: %d,\ntm_year: %d,\ntm_wday: %d,\ntm_yday: %d,\ntm_isdst: %d,\n",date_info.tm_sec,date_info.tm_min,date_info.tm_hour,date_info.tm_mday,date_info.tm_mon,date_info.tm_year,date_info.tm_wday,date_info.tm_yday,date_info.tm_isdst);
    // Backtrack to the desired weekday
    
    int diff = (date_info.tm_wday - weekday + DAYS_PER_WEEK) % DAYS_PER_WEEK;
    date_info.tm_mday -= diff;
    mktime(&date_info); // Normalize again
    printf("\ntm_sec: %d,\ntm_min: %d,\ntm_hour: %d,\ntm_mday: %d,\ntm_mon: %d,\ntm_year: %d,\ntm_wday: %d,\ntm_yday: %d,\ntm_isdst: %d,\n",date_info.tm_sec,date_info.tm_min,date_info.tm_hour,date_info.tm_mday,date_info.tm_mon,date_info.tm_year,date_info.tm_wday,date_info.tm_yday,date_info.tm_isdst);

    printf(" %04d-%02d-%02d\n",
           date_info.tm_year + MIN_YEAR,
           date_info.tm_mon + 1,
           date_info.tm_mday);

    return date_info;
}

struct tm last_weekday_of_month(int year, int month, int weekday) {
    struct tm date_info = {0};


    // Start from the last day of the month
    date_info.tm_year = year - MIN_YEAR; // tm_year is years since 1900
    date_info.tm_mon = month;        // Set to next month
    date_info.tm_mday = 0;           // Day 0 means "last day of previous month"
    mktime(&date_info);              // Normalize to get last day of given month

    // Backtrack to the desired weekday
    int diff = (date_info.tm_wday - weekday + DAYS_PER_WEEK) % DAYS_PER_WEEK;
    date_info.tm_mday -= diff;
    mktime(&date_info); // Normalize again

    return date_info;
}

int checkmonth(const char*m){
  int i,len = sizeof(monthASCII_2_num_english)/sizeof(struct char2num);
  for(i=0;i<len;i++){
    if( !strcmp(monthASCII_2_num_english[i].str,m) ){
      /* fprintf(stderr,"Month %s --> %d\n",
           monthASCII_2_num_english[i].str,
           monthASCII_2_num_english[i].num);
      */
      return monthASCII_2_num_english[i].num;
    }
  }
   fprintf(stderr," checkmonth: %s not a valid Month \n",m);
   exit(EXIT_FAILURE);
  
  return -1;
}

int checkweekday(const char*m){
  int i,len = sizeof(weekdayASCII_2_weekday_english)/sizeof(struct char2num);
  for(i=0;i<len;i++){
    if( !strcmp(weekdayASCII_2_weekday_english[i].str,m) ){
      /* fprintf(stderr,"Weekday %s --> %d\n",
           weekdayASCII_2_weekday_english[i].str,
           weekdayASCII_2_weekday_english[i].num);
      */
      return weekdayASCII_2_weekday_english[i].num;
    }
  }
   fprintf(stderr," checkweekday: %s not a valid Weekday \n",m);
   exit(EXIT_FAILURE);
  
  return -1;
  
}

#define BUF_SIZE 100
#define BUF_TRAIL_SIZE 100
int main(int argc, char **argv) {
    int i,j,ret;
    int year, month, weekday;
    char smonth[BUF_SIZE+BUF_TRAIL_SIZE], sweekday[BUF_SIZE+BUF_TRAIL_SIZE];
    if(argc != 4){
      /*
        command line API 
      */
      fprintf(stderr,"error: usage: $0 <year> <weekday> <month>\n");
      exit(EXIT_FAILURE);
    }

        //printf("    // arg %d: %s\n",i,argv[i]);
        // arg 1: year
         errno = 0;
         ret = sscanf(argv[1], "%d", &year);
           if (ret == 1) {
               //printf("read year: %d\n", year);
               if(year <MIN_YEAR){
                   fprintf(stderr, "year < %d : %d\n",MIN_YEAR,year);
                   exit(EXIT_FAILURE);
               }
           } else {
               if (errno != 0) {
                   perror("sscanf");
                   exit(EXIT_FAILURE);
               } else {
                   fprintf(stderr, "No matching characters\n");
                   exit(EXIT_FAILURE);
               }
           }  
        // arg 2: weekday
         errno = 0;
         if(strlen(argv[2])>BUF_SIZE){
           fprintf(stderr, "Strlen for weekday >%d: %d\n",BUF_SIZE,(int)strlen(argv[2]));
           exit(EXIT_FAILURE);
         }
         ret = sscanf(argv[2], "%s", (char *)&sweekday);
           if (ret == 1) {
               //printf("read weekday: %s\n",sweekday);
               weekday = checkweekday(sweekday);
           } else {
               if (errno != 0) {
                   perror("sscanf");
                   exit(EXIT_FAILURE);
               } else {
                   fprintf(stderr, "No matching characters\n");
                   exit(EXIT_FAILURE);
               }
           }          
        // arg 3: month      
         errno = 0;
         if(strlen(argv[3])>BUF_SIZE){
           fprintf(stderr, "Strlen for month >%d: %d\n",BUF_SIZE,(int)strlen(argv[3]));
           exit(EXIT_FAILURE);
         }
         ret = sscanf(argv[3], "%s", (char *)&smonth);
           if (ret == 1) {
               //printf("read month: %s\n",smonth);
               month = checkmonth(smonth);
           } else {
               if (errno != 0) {
                   perror("sscanf");
                   exit(EXIT_FAILURE);
               } else {
                   fprintf(stderr, "No matching characters\n");
                   exit(EXIT_FAILURE);
               }
           }  
    
    struct tm result = last_weekday_of_month(year, month, weekday);

    printf("Last %s of %s %d-%02d is: %04d-%02d-%02d\n",
           sweekday, smonth,
           year, month,
           result.tm_year + 1900,
           result.tm_mon + 1,
           result.tm_mday);

    //test_date_function(year, month, weekday);

    return 0;
}

