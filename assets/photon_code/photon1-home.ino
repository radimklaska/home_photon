// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
#define TIMEOUT_OFFLINE (15 * 60 * 1000)
unsigned long lastComms = millis();

#define RELAYPIN D0
#define LEDPIN D7
#define DHTPIN D1
#define DHTTYPE DHT22		// DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

#define THERMOSTAT_THRESHOLD 22

boolean offlinemode = false;

void setup()
{
    Particle.syncTime();
    Particle.function("setHeater", setHeater);
    Particle.function("dth1_measure", dth1_measure);

    dht.begin();

    pinMode(LEDPIN, OUTPUT);
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(LEDPIN, HIGH);
    digitalWrite(RELAYPIN, LOW);
}

void loop()
{
    //Sync time once a day.
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        // Request time synchronization from the Particle Cloud
        Particle.syncTime();
    }

    // Fallback to offline mode.
    // Work as regular thermostat when offline
    // if (millis() - lastComms > (10 * 1000) || offlinemode == true) {
    if (millis() - lastComms > TIMEOUT_OFFLINE || offlinemode == true) {
        // Indicate offline mode by LED
        digitalWrite(LEDPIN, HIGH);
        if (dht.getTempCelcius() < THERMOSTAT_THRESHOLD) {
            digitalWrite(RELAYPIN,HIGH);
        }
        else {
            digitalWrite(RELAYPIN,LOW);
        }
        // Prevent quick ON/OFF switching when arount threshold.
        delay(TIMEOUT_OFFLINE);
    }
    else {
        // Indicate offline mode by LED
        digitalWrite(LEDPIN, LOW);
    }
}

int dth1_measure(String command) {
    lastComms = millis();
    if (command=="getHumidity") {
        return dht.getHumidity() * 100;
    }
    else if (command=="getTempCelcius") {
        return dht.getTempCelcius() * 100;
    }
    else if (command=="getTempFarenheit") {
        return dht.getTempFarenheit() * 100;
    }
    else {
        return -1;
    }
}

int setHeater(String command) {
    lastComms = millis();
    if (command=="on") {
        digitalWrite(RELAYPIN,HIGH);
        offlinemode = false;
        return 1;
    }
    else if (command=="off") {
        digitalWrite(RELAYPIN,LOW);
        offlinemode = false;
        return 1;
    }
    else if (command=="offlinemode_on") {
        offlinemode = true;
        return 1;
    }
    else if (command=="offlinemode_off") {
        offlinemode = false;
        return 1;
    }
    else {
        return -1;
    }
}
