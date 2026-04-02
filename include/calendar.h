#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <cjson/cJSON.h>

typedef struct{

	uint16_t time;
	char* title;
} Entry;

typedef struct{

	int init;
	uint8_t dnum;
	int count;
	Entry* entries;
} Day;

typedef struct{

	int init;
	uint8_t mnum;
	Day days[31];
} Month;

typedef struct{
	
	uint16_t ynum;
	Month months[12];
} Year;

uint16_t get_year();

int free_year(Year* _yr);

Day fday(cJSON* dayJSON);

Month fmonth(cJSON* monthJSON);

Year* load_yr(uint16_t _ynum, cJSON* _yearJSON, cJSON* all_monthsJSON);

int new_entry(Month* _mt, cJSON* _mtJSON, int dnum_in, const char* _time, const char* _title);

int del_entry(Month* _mt, cJSON* _mtJSON, int dnum_del, const char* _time);

int reset_day(Day* _day);

#endif
