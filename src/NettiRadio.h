//
// Created by manx on 29.08.25.
//

#ifndef LADITKO_ESP32_SI5351_NETTIRADIO_H
#define LADITKO_ESP32_SI5351_NETTIRADIO_H

#include "Arduino.h"
#include "Audio.h"


class NettiRadio {
public:
    NettiRadio(uint8_t i2s_bclk, uint8_t i2s_lrc, uint8_t i2s_dout, uint8_t zes_on);
    void begin(uint8_t initial_volume = 20);
    void loop();
    void setVolume(uint8_t volume);
    bool connectToURL(const char* url);

    void stop();
    bool isRunning();
    void playTestTone();

private:
    Audio audio;
    uint8_t _i2s_bclk;
    uint8_t _i2s_lrc;
    uint8_t _i2s_dout;
    uint8_t _zes_on;
    uint8_t _volume;
};

#endif //LADITKO_ESP32_SI5351_NETTIRADIO_H