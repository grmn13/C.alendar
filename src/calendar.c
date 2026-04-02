#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "calendar.h"

uint16_t get_year(){

	time_t now = time(NULL);
	struct tm* tstc = localtime(&now);

	return tstc->tm_year + 1900;
}

int free_year(Year* _yr){
	
	if(_yr == NULL) return 0;

	for(int i = 0; i < 12; i++){

		for(int j = 0; j < 31; j++){
	
			Day* day = &_yr->months[i].days[j];

			for(int k = 0; k < day->count; k++){

				free(day->entries[k].title);
			}

		}
	}

	free(_yr);
	
	return 0;

}

Day fday(cJSON* dayJSON){

	Day _day = {0};

	int _count = cJSON_GetObjectItem(dayJSON, "count")->valueint;

	_day.init = 1;
	_day.dnum = atoi(dayJSON->string);
	_day.count = _count;

	_day.entries = malloc(sizeof(Entry) * _count);

	cJSON* entriesJSON = cJSON_GetObjectItem(dayJSON, "entries");
	cJSON* entry = NULL;

	int e_counter = 0;

	cJSON_ArrayForEach(entry, entriesJSON){

		if(e_counter > _count){

			printf("ERR: mismatch between _day->count and actual count of entries\n");
			return _day;
		}

		uint16_t _time = atoi(entry->string);

		if(_time > (uint16_t)1440){

			printf("ERR: Invalid time detected!\n");
			return _day;
			//should indicate user exact date but idk if i should pass the month
			//to the function just for this or maybe on the future it is indicated
			//on a global pointer or smt so ill leave it like this for now
		}

		_day.entries[e_counter].time = _time;
		_day.entries[e_counter++].title = strdup(entry->valuestring);
	}

	if(e_counter != _count){

		printf("ERR: mismatch between _day->count and actual count of entries\n");
	}

	return _day;
}


Month fmonth(cJSON* monthJSON){

	Month _mt = {0};

	int monthNum = atoi(monthJSON->string);
	
	_mt.init = 1;
	_mt.mnum = monthNum;


	char num[2];
	char big_num[3];

	num[0] = '0';
	num[1] = '\0';

	big_num[0] = '0';
	big_num[1] = '0';
	big_num[2] = '\0';

	Day tmp_day;

	cJSON* dayJSON = NULL;
	cJSON* countJSON = NULL;

	for(int i = 1; i <= 31; i++){

		//fetch the day
		if(i < 10){

			num[0] = i + 48;
			dayJSON = cJSON_GetObjectItem(monthJSON, num);
		}
		else{
			big_num[0] = i / 10 + 48;
			big_num[1] = i % 10 + 48;
			dayJSON = cJSON_GetObjectItem(monthJSON, big_num);
		}
	
		countJSON = cJSON_GetObjectItem(dayJSON, "count");
		
		if(dayJSON != NULL && countJSON->valueint > 0){

			tmp_day = fday(dayJSON);
			_mt.days[i - 1] = tmp_day;
		}

	}

	return _mt;

}

Year* load_yr(uint16_t _ynum, cJSON* _yearJSON, cJSON* all_monthsJSON){

	Year* _yr = calloc(1, sizeof(Year));
	_yr->ynum = get_year();

	char fname[32];
	snprintf(fname, sizeof(fname), "data/%d.json", _ynum);

	FILE* fp = fopen(fname, "rb");
	if (fp == NULL){

		printf("ERR: Error opening file %s\n", fname);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	char* buffer = malloc(size + 1);
	if(!buffer){
		fclose(fp);
		printf("ERR: No memory could be allocated for the json file");
		return NULL;
	}

	size_t rsize = fread(buffer, 1, size, fp);
	buffer[rsize] = '\0';
	fclose(fp);
	
	//get the full year into a JSON obj
	_yearJSON = cJSON_Parse(buffer);

	if(_yearJSON == NULL){
		const char* error_ptr = cJSON_GetErrorPtr();
		if(error_ptr != NULL){
			printf("ERR: %s\n", error_ptr);
		}
		cJSON_Delete(_yearJSON);
		return NULL;
	}

	//for each month

	//allocate space for the month struct
	Month tmp_month;
	
	for(int m = 1; m <= 12; m++){
		
		char month_k[3];
		sprintf(month_k, "%d", m);
		//retrieve month into JSON obj
		all_monthsJSON = cJSON_GetObjectItem(_yearJSON, "months");
		cJSON* monthJSON = cJSON_GetObjectItem(all_monthsJSON, month_k);

		if(monthJSON != NULL){

			tmp_month = fmonth(monthJSON);
			_yr->months[m - 1] = tmp_month;
		}
	}

	return _yr;
}

		//month		monthJSON	day		time		title
int new_entry(Month* _mt, cJSON* _mtJSON, int dnum_in, const char* _time, const char* _title){

	Day* _day = &_mt->days[dnum_in];
	Entry* _entries = _day->entries;

	char day_str[4];
	snprintf(day_str, sizeof(day_str), "%d", dnum_in + 1);

	cJSON* _dayJSON = cJSON_GetObjectItem(_mtJSON, day_str);
	cJSON* entryJSON = cJSON_GetObjectItem(_dayJSON, "entries");

	cJSON_AddStringToObject(entryJSON, _time, _title);

	//creating the struct to append
	Entry n_entry = {.time = atoi(_time), .title = strdup(_title)};

	Entry* rllc_ptr = realloc(_entries, (_day->count + 1) * sizeof(Entry));

	//first time doing realloc ts may be done wrong
	if(rllc_ptr == NULL){
		printf("REALLOC ERROR ON NEW ENTRY\n");
		return 1;
	}
	
	_day->entries = rllc_ptr;

	if(_mt->init == 0){
		_mt->init = 1;
	}
	if(_day->init == 0){
		_day->init = 1;
	}
	
	_day->entries[_day->count] = n_entry;
	_day->count += 1;

	return 0;
}

int reset_day(Day* _day){

	//should check at some point if this function actually workds
	
	for(int i = 0; i < _day->count; i++){

		free(_day->entries[i].title);
	}

	free(_day->entries);
	memset(_day, 0, sizeof(Day));

	return 0;
}

int del_entry(Month* _mt, cJSON* _mtJSON, int dnum_del, const char* _time){

	Day* _day = &_mt->days[dnum_del];
	Entry* _entries = _day->entries;

	char day_str[4];
	snprintf(day_str, sizeof(day_str), "%d", dnum_del);

	//delete from json tree
	cJSON* _dayJSON = cJSON_GetObjectItem(_mtJSON, day_str);
	cJSON* entryJSON = cJSON_GetObjectItem(_dayJSON, "entries");

	cJSON_DeleteItemFromObject(entryJSON, _time);

	//delete from memory
	for(int i = 0; i < _day->count; i++){

		if(_entries[i].time == atoi(_time)){
			
			free(_entries[i].title);

			_entries[i] = _entries[_day->count - 1];

			break;
		}
	}
	_day->count -= 1;

	if(_day->count <= 0){

		if(reset_day(_day) == 1){

			printf("reset day error\n");
		}
	}

	return 0;
}



