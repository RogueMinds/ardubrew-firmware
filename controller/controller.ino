#include <OneWire.h>
#include <DallasTemperature.h>
#include <Time.h>

#define PIN_RELAY       2    // Relay controller
#define PIN_TEMPERATURE 3    // DS18B20 temperature sensor

#define TIME_CMD       'T'   // Header tag for serial time sync message
#define TIME_LEN        10   // Length of time payload
#define TIME_FREQ       15   // run actions every 15 seconds

#define TEMP_CMD       'S'   // Header tag for the temperature sync message
#define TEMP_LEN        2    // Length of temperature payload
#define TEMP_OFFSET     1.0  // Temperature offset
#define TEMP_TARGET     64.0 // Temperature target

int relay_status        = HIGH;
long timestamp          = 0;
double temperature      = 0.0;
double targetTemp       = 0.0;

OneWire temperatureWire(PIN_TEMPERATURE);
DallasTemperature sensors(&temperatureWire);

void logAction(const char* action) {
    Serial.print(action);
    Serial.print(":");
    Serial.print(timestamp);
}

void logData(const char* action) {
    logAction(action);
    Serial.println();
}

void logData(const char* action, int value) {
    logAction(action);
    Serial.print(":");
    Serial.println(value);
}

void logData(const char* action, long value) {
    logAction(action);
    Serial.print(":");
    Serial.println(value);
}

void logData(const char* action, double value) {
    logAction(action);
    Serial.print(":");
    Serial.println(value);
}

void logData(const char* action, const char* value) {
    logAction(action);
    Serial.print(":");
    Serial.println(value);
}

/**
 * Reads the input data from the Serial port and processes
 * it accordingly.
 */
void readInputData() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        switch (c) {
            case TIME_CMD:
                while (Serial.available() >= TIME_LEN) {
                    time_t intime = 0;
                    for (int i=0; i<TIME_LEN; i++) {
                        c = Serial.read();
                        if (c >= '0' && c <= '9') {
                            intime = (10 * intime) + (c - '0');
                        }
                    }
                    setTime(intime);
                    logData("settime", (long)intime);
                }
                break;
            case TEMP_CMD:
                while (Serial.available() >= TEMP_LEN) {
                    double intemp = 0;
                    for (int i=0; i<TEMP_LEN; i++) {
                        c = Serial.read();
                        if (c >= '0' && c <= '9') {
                            intemp = (10 * intemp) + (c - '0');
                        }
                    }
                    targetTemp = intemp;
                    logData("settemp", (double)intemp);
                }
                break;
        }
    }
}

/**
 * Reads and store the temperature data from the DS18B20 sensor.
 */
void readTemperature() {
    sensors.requestTemperatures();
    temperature = (double)sensors.getTempFByIndex(0);
    logData("temp", temperature);
}

/**
 * Checks the current temperature and enables/disables the relay.
 */
void checkTemperature() {
    readTemperature();
    if (temperature > targetTemp+TEMP_OFFSET) {
        if (relay_status != LOW) {
            relay_status = LOW;
            digitalWrite(PIN_RELAY, relay_status);
            logData("relay", "on");
        }
    } else {
        if (temperature < targetTemp-TEMP_OFFSET) {
            if (relay_status != HIGH) {
                relay_status = HIGH;
                digitalWrite(PIN_RELAY, relay_status);
                logData("relay", "off");
            }
        }
    }
}

void setup() {
    Serial.begin(9600);
    // setup temperature sensors
    sensors.begin();
    targetTemp = TEMP_TARGET;
    // setup cooler relay
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, relay_status);
}

void loop() {
    // process serial input
    if (Serial.available()) {
        readInputData();
    }
    // read the current timestamp
    timestamp = (long)now();
    // timed logic
    if (timestamp % TIME_FREQ == 0) {
        checkTemperature();
    }
    // limit the loop to once per second
    delay(1000);
}

