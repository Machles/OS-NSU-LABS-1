#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main() {
    time_t toNow;
    struct tm *currTime;

    /* Сохраняем время в секундах в toNow от 00:00:00 UTC 1 января 1970
     * Преобразование типа возвращаемого значения в void означает, что возвращаемое значение не будет использоваться. */
    (void) time( &toNow );

    if(putenv("TZ=America/Tijuana")){
        perror("Error with changing TZ.\n");
    }

    /* Преобразует календарное время в ASCII-строку формата date(1).
     * Адрес, возвращенный этой функцией, используется в качестве параметра printf для печати ASCII-строки. */
    printf("%s", ctime( &toNow ) );

    /* Функция localtime(3C) заполняет значениями поля структуры tm.*/
    currTime = localtime(&toNow);

    printf("%d/%d/%02d %d:%02d %s\n",
           currTime->tm_mon + 1, currTime->tm_mday, currTime->tm_year + 1900, currTime->tm_hour, currTime->tm_min, tzname[currTime->tm_isdst]);

    return 0;
}

