#include <SPI.h>
#include <RF24.h>

RF24 radio(2, 15); // CE, CSN (change for your board)

const byte address[6] = "GH001";

struct Packet
{
    uint8_t node_id;
    uint8_t type;
    uint16_t humidity;
    uint16_t battery;
    uint16_t counter;
};

struct AckPacket
{
    uint8_t node_id;
    uint8_t status;
};

void setup()
{
    Serial.begin(9600);

    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);

    radio.openReadingPipe(1, address);
    radio.openWritingPipe(address);

    radio.startListening();
}

void loop()
{

    if (radio.available())
    {

        Packet data;
        radio.read(&data, sizeof(data));

        Serial.println("---- RECEIVED ----");
        Serial.print("Node ID: ");
        Serial.println(data.node_id);

        Serial.print("Humidity: ");
        Serial.println(data.humidity);

        Serial.print("Battery: ");
        Serial.println(data.battery);

        Serial.print("Counter: ");
        Serial.println(data.counter);

        // ---- send ACK ----
        radio.stopListening();

        AckPacket ack;
        ack.node_id = data.node_id;
        ack.status = 1;

        radio.write(&ack, sizeof(ack));

        radio.startListening();
    }
}