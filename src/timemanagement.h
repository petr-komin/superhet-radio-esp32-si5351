//
// Created by manx on 2.11.24.
//

#ifndef TIMEMANAGEMENT_H
#define TIMEMANAGEMENT_H

#include <DS3231.h>

class Timemanagement {
public:
    bool connectToWiFi();
    void setupTime();
    void printLocalTime();
    void syncRTC(DS3231 &rtc);
};



#endif //TIMEMANAGEMENT_H
