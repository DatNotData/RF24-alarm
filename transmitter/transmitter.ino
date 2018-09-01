#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h> 

RF24 radio (7, 8);

const byte address[6] = "00001";

bool prevState = false;

const byte MSG[1] = {39};
const byte ACK = 90;

bool sendMessage = false;
unsigned long sendMessageTime;
#define FREQUENCY 500
int tries = 0;

bool updateRadio() {
  if (sendMessage == true) {
    radio.stopListening();
    if (tries < 15) {
      if (millis() - sendMessageTime >= FREQUENCY) {
        sendMessageTime = millis();
        radio.write(&MSG, sizeof(MSG));
        bool AckAvailable = radio.isAckPayloadAvailable();
        if (AckAvailable) {
          byte rec[1];
          radio.read(&rec, sizeof(rec));
          if (rec[0] == ACK) {
            sendMessage = false;
            return true;
          }
        }
        else {
          tries ++;
        }
      }
    }
  }

  else {
    tries = 0;
  }
  return false;
}

void setup() {
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MIN);

  pinMode(6, INPUT);
}

void loop() {
  bool state = digitalRead(6);
  if (state && prevState == false) {
    sendMessage = true;
    sendMessageTime = millis() - FREQUENCY;
  }

  updateRadio();
  prevState = state;
}

