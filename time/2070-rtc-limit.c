#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <linux/rtc.h>

typedef int bool;

static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

int rtc_valid_tm(struct rtc_time *tm)
{
	if (tm->tm_year < 70 ||
	    tm->tm_year > (INT_MAX - 1900) ||
	    ((unsigned int)tm->tm_mon) >= 12 ||
	    tm->tm_mday < 1 ||
	    tm->tm_mday > rtc_month_days(tm->tm_mon,
					 ((unsigned int)tm->tm_year + 1900)) ||
	    ((unsigned int)tm->tm_hour) >= 24 ||
	    ((unsigned int)tm->tm_min) >= 60 ||
	    ((unsigned int)tm->tm_sec) >= 60)
		return -1;

	return 0;
}

int main() {
    struct rtc_time rtc_tm;

    // Set the time structure
    rtc_tm.tm_sec = 0;
    rtc_tm.tm_min = 0;
    rtc_tm.tm_hour = 0;
    rtc_tm.tm_mday = 1;
    rtc_tm.tm_mon = 0;  // January (0-indexed)
    rtc_tm.tm_year = 170;  // Years since 1900
    rtc_tm.tm_wday = 0;
    rtc_tm.tm_yday = 0;
    rtc_tm.tm_isdst = 0;

    int year;
    // Prompt the user for input
    printf("Enter a year (e.g., 1970): ");
    if (scanf("%d", &year) != 1) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    // Set the year in the rtc_tm structure
    rtc_tm.tm_year = year - 1900;
    
    // Validate the rtc_tm structure
    int result = rtc_valid_tm(&rtc_tm);

    if (result == 0) {
        printf("Year %d is valid.\n", year);
    } else {
        printf("Year %d is not valid.\n", year);
    }

    return 0;
}