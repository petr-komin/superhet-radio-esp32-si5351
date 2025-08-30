#include <Adafruit_GFX.h>    // Core graphics library

#include <DS3231.h>
#include <vector>
#include "globals.h"

#define BLACK   0x0000
#define BLUE    0x0010
#define RED     0xF800
#define GREEN   0x0400
#define CYAN    0x0410
#define MAGENTA 0x8010
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGBTO565(_r, _g, _b) ((((_r) & B11111000)<<8) | (((_g) & B11111100)<<3) | ((_b) >>3))



class Lgfx{
  
  
  
  public:
    void init();
    void refreshFreq(unsigned long fr);
    void refreshOvladani(long step);
    void refreshVoltages(float v1, float v2);
    void refreshRezim(const char* rezim_name);
    void showStatus(const char* message);

    void showLadeniScreen();
    void hideLadeniScreen();
    void showInternetRadioMenu(int selected_index);
    void hideInternetRadioMenu();

    int last_adcValue = 0;
    bool last_wifi_status = false;

    void refreshADC(int adcValue);
    void refreshSilomer(int adcValue);
    void updateTime( DS3231 *rtc);
    void updateTime(int hour, int minute, int second);
    void updateWifiStatus(bool connected);

  };
