#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
extern char *tzname[];

int main(void)
{
    time_t now;
    struct tm *sp;

    // TODO: Read about putenv, enviroment vars,

    (void)time(&now);     //time in second to now
    putenv("TZ=PST8PDT"); //set time zone

    // ctime throws EOVERFLOW
    // The result cannot be represented.
    printf("%s", ctime(&now)); // print time by using ctime, that returns 26-smb--string

    // localtime throws EOVERFLOW
    sp = localtime(&now); // localtime returns pointer to tm structure. Tm contains fields with year/month/day/h/m/s and daylite time flag
    printf("%d/%d/%02d %d:%02d %s\n",
           sp->tm_mon + 1, sp->tm_mday, sp->tm_year,
           sp->tm_hour, sp->tm_min,
           tzname[sp->tm_isdst]);

    return EXIT_SUCCESS;
}