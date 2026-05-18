#include <SPI.h>
#include <RF24.h>

RF24 radio(4, 5); // CE, CSN

const byte address[6] = "GH001";   // shared pipe (star network)

#define NODE_ID 1   // CHANGE THIS PER NODE

struct Packet {
  uint8_t node_id;
  uint8_t type;
  uint16_t humidity;
  uint16_t battery;
  uint16_t counter;
};

struct AckPacket {
  uint8_t node_id;
  uint8_t status;
};

uint16_t counter = 0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);
  radio.stopListening();
}

void loop() {

  // ---- simulate sensor (replace with real ADC) ----
  uint16_t humidity = analogRead(34);  // ESP32 example
  uint16_t battery = 370;              // fake value (3.7V)
  counter++;

  Packet data;
  data.node_id = NODE_ID;
  data.type = 0;
  data.humidity = humidity;
  data.battery = battery;
  data.counter = counter;

  // ---- send ----
  radio.stopListening();
  bool ok = radio.write(&data, sizeof(data));

  Serial.print("Sent Node ");
  Serial.print(NODE_ID);
  Serial.print(" | Status: ");
  Serial.println(ok ? "OK" : "FAIL");

  // ---- wait for ACK ----
  radio.startListening();
  unsigned long start = millis();
  bool gotAck = false;

  while (millis() - start < 100) { // 100ms window
    if (radio.available()) {
      AckPacket ack;
      radio.read(&ack, sizeof(ack));

      Serial.print("ACK from hub for Node ");
      Serial.println(ack.node_id);

      gotAck = true;
      break;
    }
  }

  radio.stopListening();

  if (!gotAck) {
    Serial.println("No ACK");
  }

  delay(3000); // plant sends every few seconds (change to minutes later)
}