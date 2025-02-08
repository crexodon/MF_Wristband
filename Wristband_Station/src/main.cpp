#include <Arduino.h>

#include <SPI.h>
#include <MFRC522.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include <secrets.h>

#include <FastLED.h>
#define NUM_LEDS 1
#define DATA_PIN 14

#define RST_PIN   21
#define SS_PIN    5
#define MOSI_PIN  23
#define MISO_PIN  19
#define CLK_PIN   18

CRGB leds[NUM_LEDS];
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Gets UID from tag and converts to string
String convertUIDtoString(MFRC522::Uid *uid) {
    // Print hex values
    Serial.print("Hex values: ");
    for(int i = 0; i < uid->size; i++) {
        Serial.print(String(uid->uidByte[i], HEX));
        Serial.print(" ");
    }
    Serial.println();
    
    // Convert bytes to string
    char hexStr[20];  // Using fixed-size array instead of String
    memset(hexStr, '0', sizeof(hexStr));
    hexStr[sizeof(hexStr)-1] = '\0';
    
    int offset = 0;
    for(int i = 0; i < uid->size; i++) {
        sprintf(&hexStr[offset], "%02X", uid->uidByte[i]);
        offset += 2;
    }
    
    Serial.print("Final hex string: ");
    Serial.println(hexStr);
    
    return hexStr;
}

// Sends string to REST API Endpoint. Returns http status code from response
int sendTagNumber(String tag_id) {
  WiFiClient client;
  HTTPClient http;
  
  //JSONVar jsonObject;
  //jsonObject["tag_id"] = tag_id;
  
  //String jsonString = JSON.stringify(jsonObject);

  String connection = rest_server + "/flag/" + tag_id + flag_url;

  Serial.println("Connection Name: " + connection);
  
  http.begin(client, connection);
  //http.addHeader("Content-Type", "application/json");
  
  //int httpResponseCode = http.POST(jsonString);
  int httpResponseCode = http.POST("");
  
  Serial.print("HTTP Response Code: ");
  Serial.println(httpResponseCode);
  Serial.println("Response:");
  Serial.println(http.getString());
  
  http.end();
  
  return httpResponseCode;
}

void setup() {
	Serial.begin(115200);

	SPI.begin(CLK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
	mfrc522.PCD_Init();
	
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(100);leds[0] = CRGB::Purple;
    FastLED.show();
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(WiFi.localIP());

  leds[0] = CRGB::Yellow;
  FastLED.show();

}

void loop() {
  while(WiFi.status() == WL_CONNECTED){
    leds[0] = CRGB::Yellow;
    FastLED.show();

	  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    }

    // Select one of the cards
	  if ( ! mfrc522.PICC_ReadCardSerial()) {
		  return;
    }

    // When Loop enters here, a card is present
    leds[0] = CRGB::Blue;
    FastLED.show();

    //mfrc522.PICC_DumpToSerial(&(mfrc522.uid)); //Output Card Info

	  String tag_id = convertUIDtoString(&mfrc522.uid);

    Serial.printf("TagID: ", tag_id);
      
    int ret;
    ret = sendTagNumber(tag_id);

    if(ret == 200){
        Serial.printf("Successfully sent tag number");
        leds[0] = CRGB::Green;
        FastLED.show();
        delay(500);
    } else {
        Serial.println("Failed to send tag number with code: " + ret);
        leds[0] = CRGB::Red;
        FastLED.show();
        delay(500);
    }
    
    // Stop NFC readout
    mfrc522.PICC_HaltA();
    leds[0] = CRGB::Yellow;
    FastLED.show();
  }

  Serial.print("WiFi not connected...");
  leds[0] = CRGB::Purple;
  FastLED.show();
  delay(200);
}