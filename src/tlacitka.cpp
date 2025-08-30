//
// Created by manx on 21.10.24.
//
#include <Arduino.h>
#include "tlacitka.h"
#include "globals.h"
#define NUM_BANDS 16
unsigned long bands[NUM_BANDS] = {279000,  522000, 2300000, 3200000,
  3900000, 6000000, 7200000, 9330000, 11600000, 13570000, 15100000, 17480000, 21450000  ,52000000, 55000000, 88000000};


#define DEBOUNCE_DELAY 200 // Debounce delay in milliseconds

//unsigned long *frequencyHz;
unsigned long lastInterruptTime = 0;


// Wrapper funkce pro přerušení

void handleButtonPress_REZIM() {
    static int lastState = HIGH;
    int currentState = digitalRead(TLACITKO_REZIM);
    if (currentState == LOW && lastState == HIGH) { // Detekce sestupné hrany
        if (millis() - lastInterruptTime < DEBOUNCE_DELAY) {
            lastState = currentState;
            return;
        }
        lastInterruptTime = millis();
        rezim = (rezim + 1) % pocet_rezimu;
    }
    lastState = currentState;
}

void handleButtonPress_GO() {
    static int lastState = HIGH;
    int currentState = digitalRead(TLACITKO_GO);
    if (currentState == LOW && lastState == HIGH) { // Detekce sestupné hrany
        if (millis() - lastInterruptTime < DEBOUNCE_DELAY) {
            lastState = currentState;
            return;
        }
        lastInterruptTime = millis();
        go_pressed = true;
    }
    lastState = currentState;
}


void handleButtonPress_BUP()
{
  static int lastState = HIGH;
  int currentState = digitalRead(TLACITKO_BUP);
  if (currentState == LOW && lastState == HIGH) { // Detekce sestupné hrany
    if (millis() - lastInterruptTime < DEBOUNCE_DELAY) {
      lastState = currentState;
      return;
    }
    lastInterruptTime = millis();

    if (rezim == 2) { // INTERNET mode
        if (selected_station_index > 0) {
            selected_station_index--;
        }
    } else { // LADENI or PAMET mode
        // podle aktualni frekvence se dostat na nejblizsi vyssi band
        for (int i = 0; i < NUM_BANDS; i++) {
          if (bands[i] > frequencyHz) {
            frequencyHz = bands[i];
            break;
          }
        }
    }
  }
  lastState = currentState; // Aktualizace předchozího stavu
}
void handleButtonPress_BDOWN()
{
  static int lastState = HIGH;
  int currentState = digitalRead(TLACITKO_BDOWN);
  if (currentState == LOW && lastState == HIGH) { // Detekce sestupné hrany
    if (millis() - lastInterruptTime < DEBOUNCE_DELAY) {
      lastState = currentState;
      return;
    }
    lastInterruptTime = millis();

    if (rezim == 2) { // INTERNET mode
        if (selected_station_index < internet_stations.size() - 1) {
            selected_station_index++;
        }
    } else { // LADENI or PAMET mode
        // podle aktualni frekvence se dostat na nejblizsi nizsi band
        for (int i = NUM_BANDS - 1; i >= 0; i--) {
          if (bands[i] < frequencyHz) {
            frequencyHz = bands[i];
            break;
          }
        }
    }
  }
  lastState = currentState; // Aktualizace předchozího stavu
}

void Tlacitka::init(){
  lastInterruptTime = millis();
  pinMode(TLACITKO_REZIM, INPUT_PULLUP);
  pinMode(TLACITKO_GO, INPUT_PULLUP);

  pinMode(TLACITKO_BUP, INPUT_PULLUP);
  pinMode(TLACITKO_BDOWN, INPUT_PULLUP);

  // inicializace tocitka

//  attachInterrupt(digitalPinToInterrupt(TLACITKO_UP1), handleButtonPress_UP1, FALLING);
//  attachInterrupt(digitalPinToInterrupt(TLACITKO_UP2), handleButtonPress_UP2, FALLING);
//  attachInterrupt(digitalPinToInterrupt(TLACITKO_DOWN1), handleButtonPress_DOWN1, FALLING);
//  attachInterrupt(digitalPinToInterrupt(TLACITKO_DOWN2), handleButtonPress_DOWN2, FALLING);


  attachInterrupt(digitalPinToInterrupt(TLACITKO_BUP), handleButtonPress_BUP, FALLING);
  attachInterrupt(digitalPinToInterrupt(TLACITKO_BDOWN), handleButtonPress_BDOWN, FALLING);
  attachInterrupt(digitalPinToInterrupt(TLACITKO_REZIM), handleButtonPress_REZIM, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TLACITKO_GO), handleButtonPress_GO, CHANGE);

}



Tlacitka::Tlacitka(){




}
