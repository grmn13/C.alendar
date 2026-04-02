#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "calendar.h"

//REMINDER!! update your inits!!

void test_loop(Year* yr){

	for(int m = 0; m < 12; m++){

		if(yr->months[m].init == 1){

			for(int d = 0; d < 31; d++){

				if(yr->months[m].days[d].init == 1){

					printf("MONTH (%d) DAY (%d:%d) WITH (%d) ENTRIES:\n", m, d, yr->months[m].days[d].dnum,  yr->months[m].days[d].count);

					for(int i = 0; i < yr->months[m].days[d].count; i++){

						printf("	%d:%d | %s\n",  yr->months[m].days[d].entries[i].time / 60,  yr->months[m].days[d].entries[i].time % 60,  yr->months[m].days[d].entries[i].title);
					}
				}
			}
		}
	}

}

int main(){

	cJSON* yearJSON = NULL;
	cJSON* all_monthsJSON = NULL;
	cJSON* mtJSON = NULL;
	cJSON* dayJSON = NULL;
	cJSON* entry = NULL;

	Year* yr = load_yr(get_year(), yearJSON, all_monthsJSON);

	if(yr == NULL){
		return 1;
	}


	//TEST


	//ENDTEST

	//i have just created the functions to delete an entry and reset a day if the count is 0
	//i should try both together and the reset of a day also on its own

	int check = free_year(yr);
	printf("%d\n", check);

	return 0;
}

/*
*
*
*
*
	for(int m = 0; m < 12; m++){

		if(yr->months[m].init == 1){

			for(int d = 0; d < 31; d++){

				if(yr->months[m].days[d].init == 1){

					printf("LOADED MONTH (%d) DAY (%d:%d) WITH (%d) ENTRIES:\n", m, d, yr->months[m].days[d].dnum,  yr->months[m].days[d].count);

					for(int i = 0; i < yr->months[m].days[d].count; i++){

						printf("	TIME: %d:%d | TITLE: %s\n",  yr->months[m].days[d].entries[i].time / 60,  yr->months[m].days[d].entries[i].time % 60,  yr->months[m].days[d].entries[i].title);
					}
				}
			}
		}
	}
*
*
*
*/
