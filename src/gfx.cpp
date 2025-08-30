#include "gfx.h"
#include <Adafruit_ST7789.h>


#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

//#include <Fonts/FreeMonoBoldOblique12pt7b.h>
// #include <Fonts/FreeMono24pt7b.h>
//#include "cmunobx8.h"
//#include "cmuntt14.h"
#include "cmuntt8.h"
#include "cmuntt28.h"

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC    2
#define TFT_RST   4



Adafruit_ST7789 tft(TFT_CS, TFT_DC,  TFT_RST);
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

uint16_t barva(char r, char g, char b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

 uint16_t barva_freq;
    uint16_t barva_vlna;
    uint16_t barva_band;
    uint16_t barva_krok = barva(255,255,255);
 
 void Lgfx::init(){

     pinMode(TFT_CS, INPUT_PULLDOWN);
     //tft.initR(INITR_BLACKTAB); // Initialize a ST7735S chip, black tab

    tft.init(240, 320, SPI_MODE2);
    tft.setRotation(3);

     tft.setRotation(3);
     tft.fillScreen(BLACK);
     tft.setCursor(22, 22);
    barva_freq = barva(055,255,95);
    barva_vlna = barva(255,175,0);
    barva_band = barva(255,255,255);
    Lgfx::refreshFreq(0);
  }

String formatFrequency(unsigned long frequencyHz) {
     float frequencykHz = frequencyHz / 1000.0;
     char buffer[20];
     dtostrf(frequencykHz, 8, 2, buffer); // Convert to string with 1 decimal place

     // Insert spaces every three digits from the end
     String str = buffer;
     int dotIndex = str.indexOf('.'); // Find the position of the decimal point
     for (int i = dotIndex - 3; i > 0; i -= 3) {
         str = str.substring(0, i) + " " + str.substring(i);
     }

     return str;
 }


String numformat(unsigned long freq){
    return formatFrequency(freq);
      unsigned long x,f,y;    
      x=  freq / 1000000;
      y= freq % 1000000;
      String str = String(""); 

      if (freq<10000000)str = " ";
      
      str+=x;
      str+=" ";
      x=  y / 1000;
      y= y % 1000;

      
      if (x<100)str+="0";
      if (x<10)str+="0";
      
      str+=x;
      str+=".";

      
      if (y<100)str+="0";
      if (y<10)str+="0";
      str+=y;
      
     return str;
      
  }
/*120 metres 	2300-2495 kHz 	Only used in tropical areas. (Strictly speaking not a short-wave band but a medium wave one!)
90 metres 	3200-3400 kHz 	Only used in tropical areas.
75 metres 	3900-4000 kHz 	Not used in the Americas. Restricted to 3950-4000 kHz in Europe, Africa and the Middle East.
60 metres 	4750-4995 kHz 	Only used in tropical areas.
49 metres 	5900-6200 kHz
41 metres 	7200-7450 kHz 	Restricted to 7300-7400 kHz in the Americas.
31 metres 	9400-9900 kHz
25 metres 	11600-12100 kHz
22 metres 	13570-13870 kHz
19 metres 	15100-15800 kHz
16 metres 	17480-17900 kHz 	Highest frequency band in common daily use.
15 metres 	18900-19020 kHz 	Virtually unused!
13 metres 	21450-21850 kHz
11 metres 	25670-26100 kHz 	Little activity other than tests of local digital services.*/

  String getBand(unsigned long freq_hz){
      long freq_khz = freq_hz / 1000;
        // broadcast bands: 522–1,602
//153 and 279
     if (freq_khz < 279 && freq_khz > 153) return "D.V.";
     if (freq_khz < 1602 && freq_khz > 522) return "S.V.";

        if (freq_khz < 2495 && freq_khz > 2300) return "120m";
        if (freq_khz < 3400 && freq_khz > 3200) return "90m";
        if (freq_khz < 4000 && freq_khz > 3900) return "75m";
        if (freq_khz < 4995 && freq_khz > 4750) return "60m";
        if (freq_khz < 6200 && freq_khz > 5900) return "49m";
        if (freq_khz < 7450 && freq_khz > 7200) return "41m";

        if (freq_khz < 9900 && freq_khz > 9400) return "31m";
        if (freq_khz < 12100 && freq_khz > 11600) return "25m";
        if (freq_khz < 13870 && freq_khz > 13570) return "22m";
        if (freq_khz < 15800 && freq_khz > 15100) return "19m";
        if (freq_khz < 17900 && freq_khz > 17480) return "16m";
        if (freq_khz < 19020 && freq_khz > 18900) return "15m";
        if (freq_khz < 21850 && freq_khz > 21450) return "13m";
        if (freq_khz < 26100 && freq_khz > 25670) return "11m";
        if (freq_khz < 30000 && freq_khz > 28500) return "10m";
         // ham bands:
        if (freq_khz < 7300 && freq_khz > 7000) return "40m";
        if (freq_khz < 10150 && freq_khz > 10100) return "30m";
        if (freq_khz < 14350 && freq_khz > 14000) return "20m";
        if (freq_khz < 18168 && freq_khz > 18068) return "17m";
        if (freq_khz < 21450 && freq_khz > 21000) return "15m";
        if (freq_khz < 24990 && freq_khz > 24890) return "12m";
        if (freq_khz < 29700 && freq_khz > 29300) return "10m";
        if (freq_khz < 55000 && freq_khz > 52000) return "6m";
        return "...";
  }



  unsigned long last_fr;

float last_metry=0;

#define BANDY_Y 35

void refreshMetry(float metry){
    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(10,BANDY_Y);
    tft.print(last_metry);
    tft.print("m");

    tft.setTextColor(barva_vlna);
    tft.setCursor(10,BANDY_Y);
    tft.print(metry);
    tft.print("m");
    last_metry=metry;
}

String last_band="";

void bandName(String band){
    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(250,BANDY_Y);
    tft.print(last_band);

    tft.setTextColor(barva_band);
    tft.setCursor(250,BANDY_Y);
    tft.print(band);
    last_band=band;
}

#define FREQ_Y 85
#define FREQ_X 10

  void Lgfx::refreshFreq(unsigned long fr){
    String newFreqStr = numformat(fr);
    String oldFreqStr = numformat(last_fr);
    
      
      tft.setFont(&cmuntt28pt7b);
    for (int i = 0; i < newFreqStr.length(); i++) {
        if (i >= oldFreqStr.length() || newFreqStr[i] != oldFreqStr[i]) {
            // Clear the old character
            tft.setTextColor(BLACK);
            tft.setCursor(FREQ_X + i * 28, FREQ_Y); // Adjust the spacing as needed
            tft.print(oldFreqStr[i]);

            // Draw the new character
            tft.setTextColor(barva_freq);
            tft.setCursor(FREQ_X + i * 28, FREQ_Y); // Adjust the spacing as needed
            tft.print(newFreqStr[i]);
        }
    }
    
      last_fr=fr;

      float f = fr/1000000.0;
      float vlna = 300.0/f;
      refreshMetry(vlna);
      bandName(getBand(fr));

  }

String last_step="";

void Lgfx::refreshOvladani(long step)
{
    String s = "";
    if (step >= 1000){
        s = String(step/1000)+ "k";
    }else
        s = String(step);

    s= "Krok "+s;


    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(10, 120);

    tft.print(last_step);

    tft.setTextColor(barva_krok);
    tft.setCursor(10, 120);

    tft.print(s);

    last_step = s;


}

String last_v1="";
String last_v2="";

void Lgfx::refreshVoltages(float v1, float v2)
{

    String u1 = String(v1,1);
    String u2 = String(v2,1);

    if (u1==last_v1 && u2==last_v2) return;

    int16_t x=10;
    int16_t y=140;

    tft.setFont(&cmuntt8pt7b);

    tft.setTextColor(BLACK);
    tft.setCursor(x, y);

    tft.print(last_v1);
    tft.print(" , ");
    tft.print(last_v2);
    tft.print("V");



    tft.setTextColor(YELLOW);
    tft.setCursor(x, y);

    tft.print(u1);
    tft.print(" , ");
    tft.print(u2);
    tft.print("V");



    last_v1=u1;
    last_v2=u2;

}


String last_rezim_name = "";
void Lgfx::refreshRezim(const char* rezim_name) {
    if (last_rezim_name == rezim_name) {
        return;
    }

    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(10, 15); // Adjust the position as needed
    tft.print(last_rezim_name);

    tft.setTextColor(CYAN);
    tft.setCursor(10, 15); // Adjust the position as needed
    tft.print(rezim_name);

    last_rezim_name = rezim_name;
}

String last_status_message = "";
void Lgfx::showStatus(const char* message) {
    if (last_status_message == message) {
        return;
    }

    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(150, 15); // Position next to mode
    tft.print(last_status_message);

    tft.setTextColor(YELLOW);
    tft.setCursor(150, 15);
    tft.print(message);

    last_status_message = message;
}

void Lgfx::showLadeniScreen() {
    refreshFreq(last_fr);
    refreshOvladani(0); // You might want to pass the actual step here
}

void Lgfx::hideLadeniScreen() {
    tft.fillRect(0, BANDY_Y - 15, 320, 120, BLACK);
}

int last_selected_index = -1;
void Lgfx::showInternetRadioMenu(int selected_index) {
    tft.setFont(&FreeMono9pt7b);
    int y = 40;
    int lineHeight = 20;

    // If this is the first time drawing the menu, draw all items.
    if (last_selected_index == -1) {
        for (int i = 0; i < internet_stations.size(); i++) {
            if (i == selected_index) {
                tft.setTextColor(YELLOW);
            } else {
                tft.setTextColor(WHITE);
            }
            tft.setCursor(10, y + i * lineHeight);
            tft.print(internet_stations[i].name);
        }
    } else {
        // Redraw only the changed items to move the highlight.

        // Un-highlight the previously selected item.
        tft.setTextColor(WHITE);
        tft.setCursor(10, y + last_selected_index * lineHeight);
        tft.print(internet_stations[last_selected_index].name);

        // Highlight the new selected item.
        tft.setTextColor(YELLOW);
        tft.setCursor(10, y + selected_index * lineHeight);
        tft.print(internet_stations[selected_index].name);
    }

    last_selected_index = selected_index;
}

void Lgfx::hideInternetRadioMenu() {
    tft.fillRect(0, 30, 320, 210, BLACK);
    last_selected_index = -1;
}


char last_time[20]="";

void Lgfx::refreshADC(int adcValue) {
    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(10, 170); // Adjust the position as needed
    tft.print("ADC: ");
    tft.print(last_adcValue);

    tft.setTextColor(YELLOW);
    tft.setCursor(10, 170); // Adjust the position as needed
    tft.print("ADC: ");
    tft.print(adcValue);

    last_adcValue = adcValue;
}

float last_dbm = 0.0;

void Lgfx::refreshSilomer(int adcValue)
{
    uint16_t stin = barva(25,25,25);

    float voltage = adcValue * (3.9 / 4095.0);
    float dBm = (voltage - 1.25) * 25.0; // Convert voltage to dBm (based on AD8307 datasheet)
  // Clear the previous dBm value
    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(10, 170); // Adjust the position as needed
    tft.print("dBm: ");
    tft.print(last_dbm,1);

    // Display the new dBm value
    tft.setTextColor(YELLOW);
    tft.setCursor(10, 170); // Adjust the position as needed
    tft.print("dBm: ");
    tft.print(dBm, 1);

    // Clear the previous bar


    // Draw the new bar
    int barLength = map(dBm, -10, 17, 0, 319); // Map dBm value to bar length (adjust range as needed)
    tft.fillRect(0, 130, barLength, 10, GREEN); // Adjust the position, size, and color as needed

    tft.fillRect(barLength+1, 130, 319 - barLength, 10, stin); // Draw the black bar on the remaining space


    last_dbm = dBm;
}


void Lgfx::updateTime(DS3231 *rtc)
{
    bool century = false;
    bool h12Flag=false;
    bool pmFlag=false;
    char time[20];
    sprintf(time, "%02d:%02d:%02d", rtc->getHour(h12Flag, pmFlag), rtc->getMinute(), rtc->getSecond());

    if (strcmp(time, last_time) == 0) {
        return;
    }

    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(210, 180); // Adjust the position as needed
    tft.print(last_time);

    tft.setTextColor(YELLOW);
    tft.setCursor(210, 180); // Adjust the position as needed
    tft.print(time);

    strcpy(last_time, time);
}

void Lgfx::updateTime(int hour, int minute, int second) {
    char time[20];
    sprintf(time, "%02d:%02d:%02d", hour, minute, second);

    if (strcmp(time, last_time) == 0) {
        return;
    }

    tft.setFont(&FreeMono9pt7b);
    tft.setTextColor(BLACK);
    tft.setCursor(210, 180); // Adjust the position as needed
    tft.print(last_time);

    tft.setTextColor(YELLOW);
    tft.setCursor(210, 180); // Adjust the position as needed
    tft.print(time);

    strcpy(last_time, time);
}

void Lgfx::updateWifiStatus(bool connected) {
    if (connected == last_wifi_status) {
        return;
    }

    tft.setFont(&FreeMono9pt7b);
    int16_t x = 10;
    int16_t y = 200; // Bottom left corner

    if (connected) {
        tft.setTextColor(GREEN);
    } else {
        tft.setTextColor(BLACK);
    }
    tft.setCursor(x, y);
    tft.print("W");

    last_wifi_status = connected;
}
