//
// Created by manx on 2.11.24.
//

#include "timemanagement.h"


#include <time.h>
#include <WiFi.h>
#include "secrets.h"

bool Timemanagement::connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected");
        return true;
    } else {
        Serial.println("\nFailed to connect to WiFi.");
        return false;
    }
}
//Set up the time:
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; // Adjust for your timezone
const int daylightOffset_sec = 0; // Adjust for daylight saving time

void Timemanagement::setupTime() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Time synchronized");
}


//  Get the current time:
void Timemanagement::printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void Timemanagement::syncRTC(DS3231 &rtc) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time for RTC sync");
        return;
    }
    // tm_year is years since 1900. DS3231 library expects year - 2000.
    rtc.setYear(timeinfo.tm_year + 1900 - 2000);
    // tm_mon is 0-11
    rtc.setMonth(timeinfo.tm_mon + 1);
    rtc.setDate(timeinfo.tm_mday);
    rtc.setHour(timeinfo.tm_hour);
    rtc.setMinute(timeinfo.tm_min);
    rtc.setSecond(timeinfo.tm_sec);
    Serial.println("RTC synchronized with NTP time.");
}
