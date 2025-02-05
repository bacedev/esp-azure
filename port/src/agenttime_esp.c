// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/apps/sntp.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/xlogging.h"

#include "esp_log.h"

static const char* TAG = "azure_sntp";

void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "europe.pool.ntp.org");
    sntp_setservername(2, "uk.pool.ntp.org ");
	sntp_setservername(3, "us.pool.ntp.org");
	sntp_setservername(4, "time1.google.com");
    sntp_init();
}

static void obtain_time(void)
{
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;

    while(timeinfo.tm_year < (2016 - 1900) && retry < 60 )
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... tm_year:%d[times:%d]", timeinfo.tm_year, ++retry);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

time_t sntp_get_current_timestamp()
{
    time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP. timeinfo.tm_year:%d",timeinfo.tm_year);
		obtain_time();
		// update 'now' variable with current time
		time(&now);
	}
	localtime_r(&now, &timeinfo);
	return now;
}

time_t get_time(time_t* currentTime)
{
    return sntp_get_current_timestamp();

}

double get_difftime(time_t stopTime, time_t startTime)
{
    return (double)stopTime - (double)startTime;
}

struct tm* get_gmtime(time_t* currentTime)
{
    return NULL;
}

char* get_ctime(time_t* timeToGet)
{
    return NULL;
}
