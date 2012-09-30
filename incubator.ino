#include <Time.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// project settings
#define PIN_RELAY       2    // NC relay
#define PIN_TEMPERATURE 3    // DS18B20 temperature sensor

#define CMD_SYNC       'T'   // Header tag for serial time sync message
#define TMP_SYNC       'S'   // Header tag for the temperature sync message

#define TIME_LENGTH     10   // Unix time_t as ten ASCII digits
#define TEMP_LENGTH     2    // Unix flaot as 2 ASCII digits
#define TIME_FREQ       15   // run actions every 15 seconds

#define TEMP_OFFSET     1.0  // Temperature offset
#define TEMP_TARGET     64.0 // Temperature target

// project variables
int relay_status        = HIGH; // Fridge off
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
			case CMD_SYNC:
				while (Serial.available() >= TIME_LENGTH) {
					time_t pctime = 0;
					for (int i=0; i<TIME_LENGTH; i++) {
						c = Serial.read();
						if (c >= '0' && c <= '9') {
							pctime = (10 * pctime) + (c - '0');
						}
					}
					setTime(pctime);
					logData("sync", (long)pctime);
				}
				break;
                        case TMP_SYNC:
                                while (Serial.available() >= TEMP_LENGTH) {
					double temp = 0;
					for (int i=0; i<TEMP_LENGTH; i++) {
						c = Serial.read();
						if (c >= '0' && c <= '9') {
							temp = (10 * temp) + (c - '0');
						}
					}
					targetTemp = temp;
					logData("temp", (double)temperature);
				}
				break;
		}
	}
}

/**
 * Reads and store the temperature data from the DS18B20
 * sensor.
 */
void readTemperature() {
	sensors.requestTemperatures();
	temperature = (double)sensors.getTempFByIndex(0);
}

/**
 * Checks the current temperature and enables/disables the relay
 */
void checkTemperature() {
	readTemperature();
	logData("temp", temperature);
        logData("max", targetTemp+TEMP_OFFSET);
        logData("min", targetTemp-TEMP_OFFSET);
        logData("relay", relay_status);
	if (temperature > targetTemp+TEMP_OFFSET) {
		if (relay_status != LOW) {
                        logData("cooler", "turnOn");
			relay_status = LOW;
			digitalWrite(PIN_RELAY, relay_status);
			logData("switch", 1);
		} else {
                        logData("cooler", "isOn");
                }
	} else {
		if (temperature < targetTemp-TEMP_OFFSET) {
			if (relay_status != HIGH) {
                                logData("cooler", "turnOff");
				relay_status = HIGH;
				digitalWrite(PIN_RELAY, relay_status);
				logData("switch", 0);
			} else {
                                logData("cooler", "isOff");
                        }
		} else {
                        logData("cooler", "?");
                }
	}
}

void setup() {
	Serial.begin(9600);
	// setup temperature sensors
	sensors.begin();
        targetTemp = TEMP_TARGET;
	// setup heater relay
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
