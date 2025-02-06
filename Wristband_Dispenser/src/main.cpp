#include <Arduino.h>
#define EN_PIN    9  // Nano v3:	16 Mega:	38	//enable (CFG6)
#define DIR_PIN   3  //			19			55	//direction
#define STEP_PIN  2  //			18			54	//step
#define CS_PIN    8  //			17			64	//chip select

bool dir = true;

#include <TMC2130Stepper.h>
TMC2130Stepper driver = TMC2130Stepper(EN_PIN, DIR_PIN, STEP_PIN, CS_PIN);

void setup() {
	Serial.begin(115200);
	while(!Serial);
	Serial.println("Start...");
	SPI.begin();
	pinMode(MISO, INPUT_PULLUP);
	driver.begin(); 			// Initiate pins and registeries
	driver.rms_current(1000); 	// Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
	driver.stealthChop(1); 	// Enable extremely quiet stepping
	
	digitalWrite(EN_PIN, LOW);
}

void loop() {
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(10);
	uint32_t ms = millis();
	static uint32_t last_time = 0;
	if ((ms - last_time) > 2000) {
		if (dir) {
			Serial.println("Dir -> 0");
			driver.shaft_dir(0);
		} else {
			Serial.println("Dir -> 1");
			driver.shaft_dir(1);
		}
		dir = !dir;
		last_time = ms;
	}
}