#include <Arduino.h>
#include "gfx.h"
#include "NettiRadio.h"
Lgfx gfx;
#include "globals.h"
#include <SPIFFS.h>
#include <FS.h>
#include<Wire.h>
#include <WiFi.h>

#include <DS3231.h>

#include "si5351.h"
// Inicializace SI5351

#include "tlacitka.h"
#include "timemanagement.h"
#include <Fonts/FreeSansOblique18pt7b.h>


#define ADC_PIN 33

#define TOCITKO_CLK 12
#define TOCITKO_DT 13


#define I2S_DOUT  17
#define I2S_BCLK  15

#define I2S_LRC   16
#define ZES_ON 14

unsigned long frequencyHz;
volatile int rezim = 0; // 0: ladeni, 1: pamet, 2: internet
const char* rezim_nazvy[] = {"LADENI", "PAMET", "INTERNET"};
const int pocet_rezimu = 3;
std::vector<InternetStation> internet_stations;
volatile int selected_station_index = 0;
volatile bool go_pressed = false;

const char* internet_radio_csv =
    "CRO Plus;http://rozhlas.stream/plus.mp3;"
    "ČRo Vltava;https://rozhlas.stream/vltava.mp3;"
    "Järviradio;https://jarviradio.radiotaajuus.fi:9000/jr;"
    "Rádio Devín;https://icecast.stv.livebox.sk/devin_128.mp3;"
;


NettiRadio netradio(I2S_BCLK, I2S_LRC, I2S_DOUT, ZES_ON);

void parseInternetStations() {
    String csv = internet_radio_csv;
    int last_pos = 0;
    while (last_pos < csv.length()) {
        int name_end = csv.indexOf(';', last_pos);
        if (name_end == -1) break;
        String name = csv.substring(last_pos, name_end);

        int url_end = csv.indexOf(';', name_end + 1);
        if (url_end == -1) break;
        String url = csv.substring(name_end + 1, url_end);

        internet_stations.push_back({name, url});
        last_pos = url_end + 1;
    }
}

long lastWrittenFreq = 0;
long lastFreq = 0;
const double trimFreq = 124999500;
void setSI5351Frequency(unsigned long freq);

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#include <stdio.h>

volatile long encoderPos = 0;
long lastEncoded = 0;



long krok = 50;

char bylo4;



DS3231 myRTC;

long steps[]={ 500, 1000, 5000,100000,1000000};

int currentStepIndex = 0; // Globální proměnná pro sledování aktuálního indexu v poli steps

#define LAST_SETUP_FILE "/last_setup.txt"


struct Frequency {
    long freq;
    char name[30];
};
//5780 	Harmony Radio

//7415 	CHINA RADIO INTERNATIONAL	19:00	20:00	1234567	Czech


Frequency frequencies[] = {
    {6070000, "Radio Ch 292"},
    {5780000, "Harmony Radio"},
    {9670000, "Radio Ch 292"},
    {7475000, "R.THAILAND"},
    {7415, "CHINA RADIO INTERNATIONAL"}


    // Add more frequencies as needed
};




void checkLastFreq()
{
    // Check if the frequency file exists
    if (SPIFFS.exists(LAST_SETUP_FILE)) {
        File file = SPIFFS.open(LAST_SETUP_FILE, FILE_READ);
        if (file) {
            String lastLine;
            while (file.available()) {
                lastLine = file.readStringUntil('\n');
                //Serial.print("> "+lastLine);

                if (lastLine.length() > 0) {
                      //  Serial.print("> " + lastLine);
                } else {
                        break; // Exit the loop if an empty line is encountered
                }
            }
            frequencyHz = lastLine.toInt();
            if (frequencyHz < 1000) {
                Serial.println("Invalid frequency loaded from file: " + String(lastLine));
                return ;
            }
            lastWrittenFreq = frequencyHz;
            file.close();
            Serial.println("Frequency loaded from file: " + String(lastLine));
        } else {
            Serial.println("Failed to open frequency file for reading");
        }
    } else {
        Serial.println("Frequency file does not exist");
    }
}



// Funkce pro nastavení frekvence modulu SI5351


#define NUM_FREQUENCIES 5





Si5351 si5351;
Tlacitka tlacitka;
int32_t cal_factor = 34500;
int32_t old_cal = 0;

const uint64_t freq_mf = 10700000;

void setSI5351Frequency(unsigned long freq) {
    si5351.set_freq((freq+freq_mf) * 100ULL, SI5351_CLK0);
    si5351.set_freq((freq+freq_mf) * 100ULL, SI5351_CLK1);
    si5351.update_status();
}

void klok_tocitko(){


    //if (millis() - lastInterruptTime  < DEBOUNCE_DELAY) {
  //      return;
//    }

//    lastInterruptTime = millis();

    char q=digitalRead(TOCITKO_DT);
    char ted = digitalRead(TOCITKO_CLK);

    if (ted==1 && bylo4==0){
        if (q>0)
            frequencyHz+=krok;
        else
            frequencyHz-=krok;

    }

    bylo4=ted;


    //gfx.refreshFreq(frequencyHz);
    //DDS.setfreq(frequencyHz, 0);
    //writeFreq(dds, frequencyHz);
    //}
    //setSI5351Frequency(frequencyHz);


}

bool century = false;
bool h12Flag=false;
bool pmFlag=false;
void timetest()
{
    Serial.println("DS3231 Test");

    Serial.print(myRTC.getYear(), DEC);
    Serial.print("-");
    Serial.print(myRTC.getMonth(century), DEC);
    Serial.print("-");
    Serial.print(myRTC.getDate(), DEC);
    Serial.print(" ");
    Serial.print(myRTC.getHour(h12Flag, pmFlag), DEC); //24-hr
    Serial.print(":");
    Serial.print(myRTC.getMinute(), DEC);
    Serial.print(":");
    Serial.println(myRTC.getSecond(), DEC);

}
Timemanagement tm;


void setup() {
    frequencyHz = 6070000;
    Serial.begin(9600);
    gfx.init();
    gfx.refreshRezim(rezim_nazvy[rezim]);

    parseInternetStations();

    Wire.begin();
  //  Wire.setClock(40000);
    si5351.reset();
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
    si5351.set_correction(cal_factor, SI5351_PLL_INPUT_XO);


    si5351.set_freq((10700000 + 455000) * 100ULL, SI5351_CLK2);
    si5351.update_status();

    gfx.showStatus("WIFI...");
    if (tm.connectToWiFi()) {
        gfx.showStatus("TIME...");
        tm.setupTime();
        tm.printLocalTime();
        tm.syncRTC(myRTC);


        netradio.begin();
/*
        netradio.connectToURL("https://jarviradio.radiotaajuus.fi:9000/jr");
        netradio.loop();
        netradio.stop();
*/

    }
    gfx.showStatus("");


    myRTC.enableOscillator(true, false, 3);
/*
    myRTC.setHour(10);
    myRTC.setMinute(11);
    myRTC.setSecond(0);
*/
    timetest();
    delay(1000);

    gfx.updateTime(&myRTC);




    tlacitka.init();

    // write your initialization code here

 // Initialize the ADC pin
    pinMode(ADC_PIN, INPUT);

    setSI5351Frequency(frequencyHz);
delay(100);




 //   gfx.refreshOvladani(krok);

    pinMode(TOCITKO_CLK, INPUT_PULLUP);
    pinMode(TOCITKO_DT, INPUT_PULLUP);
attachInterrupt(TOCITKO_CLK, klok_tocitko, CHANGE);


     if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    // Get SPIFFS filesystem information
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    size_t freeBytes = totalBytes - usedBytes;

    Serial.println("SPIFFS Filesystem Info:");
    Serial.println("Total space: " + String(totalBytes) + " bytes");
    Serial.println("Used space: " + String(usedBytes) + " bytes");
    Serial.println("Free space: " + String(freeBytes) + " bytes");

    timetest();

    checkLastFreq();
    setSI5351Frequency(frequencyHz);
    gfx.refreshFreq(frequencyHz);
 analogSetPinAttenuation(ADC_PIN, ADC_11db);

/*
 *ADC_0db: 0 to 1.1V
ADC_2_5db: 0 to 1.5V
ADC_6db: 0 to 2.2V
ADC_11db: 0 to 3.9V
 *
 *
 */

}

unsigned long lastChangeTime = 0;
const unsigned long interval = 20000; // 20 seconds
float voltage1 = 0.0;
float voltage2 = 0.0;
long lastVoltTime = 0;




// Function to write the frequency to a text file
void writeFrequencyToFile(unsigned long freq) {
    if (freq == lastWrittenFreq) {
        return;
    }
    lastWrittenFreq = freq;
    File file = SPIFFS.open(LAST_SETUP_FILE, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.println(freq);
    file.close();
    Serial.println("Frequency written to file: " + String(freq));
}


int last_minute=-1;
int last_second=-1;
struct tm timeinfo;
int last_rezim = -1;
int last_selected_station_index = -1;

void loop() {
// write your code here
    netradio.loop();


    if (rezim != last_rezim) {
        gfx.refreshRezim(rezim_nazvy[rezim]);

        if (rezim == 2) { // entering internet mode
            gfx.hideLadeniScreen();
            gfx.showInternetRadioMenu(selected_station_index);
            // Vypnuti Si5351
            si5351.output_enable(SI5351_CLK0, 0);
            si5351.output_enable(SI5351_CLK1, 0);
            si5351.output_enable(SI5351_CLK2, 0);
        } else { // entering LADENI or PAMET mode
            if (last_rezim == 2) { // leaving internet mode
                gfx.hideInternetRadioMenu();
                // Zapnuti Si5351
                si5351.output_enable(SI5351_CLK0, 1);
                si5351.output_enable(SI5351_CLK1, 1);
                si5351.output_enable(SI5351_CLK2, 1);
                setSI5351Frequency(frequencyHz); // Obnoveni frekvence
            }
            gfx.showLadeniScreen();
        }

        last_rezim = rezim;
        last_selected_station_index = -1; // force redraw
    }

    if (rezim == 2) { // internet mode
        if (selected_station_index != last_selected_station_index) {
            gfx.showInternetRadioMenu(selected_station_index);
            last_selected_station_index = selected_station_index;
        }
        if (go_pressed) {
            go_pressed = false;
            if (selected_station_index < internet_stations.size()) {
                netradio.stop();
                gfx.showStatus("Connecting...");
                netradio.connectToURL(internet_stations[selected_station_index].url.c_str());
                gfx.showStatus(internet_stations[selected_station_index].name.c_str());
            }
        }
    }


    //delay(100); // TENTO ŘÁDEK ZPŮSOBUJE PROBLÉMY S PŘEHRÁVÁNÍM
    // Získání aktuální sekundy z interního času ESP32
    if (!getLocalTime(&timeinfo, 10)) {
        // Pokud se nepodaří získat čas, přeskočíme zbytek smyčky
        return;
    }

        if (timeinfo.tm_sec != last_second)
        {
            last_second = timeinfo.tm_sec;

            // Aktualizace displeje pomocí interního času
            gfx.updateTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            gfx.updateWifiStatus(WiFi.status() == WL_CONNECTED);
        }

    if (rezim != 2 && frequencyHz!=lastFreq){
        gfx.refreshFreq(frequencyHz);
        lastFreq=frequencyHz;
        setSI5351Frequency(frequencyHz);
        lastChangeTime = millis();

    }
 // Check if the frequency has not changed for more than 20 seconds
    if (millis() - lastChangeTime > interval && millis() - lastChangeTime < interval+1000) {
        writeFrequencyToFile(frequencyHz);
        lastChangeTime = millis(); // Reset the timer after writing to file
    }

    if (millis() - lastVoltTime > 1000) {
        lastVoltTime = millis();
        int adcValue = analogRead(ADC_PIN);
        gfx.refreshSilomer(adcValue);
    }


}