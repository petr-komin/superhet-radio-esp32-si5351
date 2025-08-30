//
// Created by manx on 29.08.25.
//

#include "NettiRadio.h"
#include "driver/i2s.h"

NettiRadio::NettiRadio(uint8_t i2s_bclk, uint8_t i2s_lrc, uint8_t i2s_dout, uint8_t zes_on)
    : _i2s_bclk(i2s_bclk), _i2s_lrc(i2s_lrc), _i2s_dout(i2s_dout), _zes_on(zes_on), _volume(0) {}

void NettiRadio::begin(uint8_t initial_volume) {
    pinMode(_zes_on, OUTPUT);
    digitalWrite(_zes_on, LOW); // Zesilovač je na začátku vypnutý
    audio.setPinout(_i2s_bclk, _i2s_lrc, _i2s_dout);
    setVolume(initial_volume);
}

void NettiRadio::loop() {
    audio.loop();
}

void NettiRadio::setVolume(uint8_t volume) {
    _volume = volume;
    audio.setVolume(_volume);
}

bool NettiRadio::connectToURL(const char* url) {
    digitalWrite(_zes_on, HIGH); // Zapnout zesilovač
    return audio.connecttohost(url);
}


void NettiRadio::stop() {
    if (audio.isRunning()) {
        audio.stopSong();
    }
    digitalWrite(_zes_on, LOW); // Vypnout zesilovač
}

bool NettiRadio::isRunning() {
    return audio.isRunning();
}

void NettiRadio::playTestTone() {
    stop(); // Zastavíme případné přehrávání
    digitalWrite(_zes_on, HIGH); // Zapneme zesilovač

    const int freq = 1000; // 1kHz
    const int durationMs = 2000; // 2s
    const int sampleRate = 44100;
    const int amplitude = 20000;
    const int samples = (durationMs * sampleRate) / 1000;
    const double twoPiF = 2.0 * M_PI * freq;
    size_t bytes_written;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = (uint32_t)sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

    i2s_pin_config_t pin_config = {
        .bck_io_num = _i2s_bclk,
        .ws_io_num = _i2s_lrc,
        .data_out_num = _i2s_dout,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    i2s_set_pin(I2S_NUM_0, &pin_config);

    for (int i = 0; i < samples; ++i) {
        double t = (double)i / sampleRate;
        int16_t sample = (int16_t)(amplitude * sin(twoPiF * t));
        uint32_t both_channels = ((uint16_t)sample << 16) | (uint16_t)sample;
        i2s_write(I2S_NUM_0, &both_channels, sizeof(both_channels), &bytes_written, portMAX_DELAY);
    }

    i2s_driver_uninstall(I2S_NUM_0);
    digitalWrite(_zes_on, LOW); // Vypneme zesilovač
    // Obnovíme nastavení pro knihovnu Audio
    audio.setPinout(_i2s_bclk, _i2s_lrc, _i2s_dout);
}
