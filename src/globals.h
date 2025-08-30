#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <vector>

extern unsigned long frequencyHz;
extern volatile int rezim;
extern const char* rezim_nazvy[];
extern const int pocet_rezimu;

struct InternetStation {
    String name;
    String url;
};

extern std::vector<InternetStation> internet_stations;
extern volatile int selected_station_index;
extern volatile bool go_pressed;


#endif //GLOBALS_H
