#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio (7, 8);

const byte address[6] = "00001";

const byte MSG = 39;
const byte ACK[1] = {90};

#define LED_PIN 4

unsigned long prevTime;
bool ledState = false;
#define FREQUENCY 100
#define FLASHES 40

bool alarmState = false;
unsigned long alarmStart = 0;

bool messageAvailable() {
  if (radio.available()) {
    byte rec[1];
    radio.read(&rec, sizeof(rec));
    if (rec[0] == MSG) {      
      radio.writeAckPayload(1, ACK, sizeof(ACK));
      return true;
    }
  }
  return false;
}

void alarm() {
  if (alarmState == true) {
    unsigned long dif = millis() - alarmStart;
    long x = dif / FREQUENCY;
    if (x < FLASHES) {
      digitalWrite(LED_PIN, (x + 1) % 2);
    }
    else {
      alarmState = false;
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);

  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  radio.setRetries(15, 15);
}

void loop() {
  if (messageAvailable()) {
    alarmState = true;
    alarmStart = millis();
  }

  alarm();
}

