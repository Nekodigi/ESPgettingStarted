//*Send array of int in byte based way.

//Recommend to use VSCode serial monitor
//Example :0100100000010010
//Result should be 1 16 256 4096 

#include <Arduino.h>
const int dataSize = 4;  // Adjust based on expected data size
uint16_t receivedData[dataSize];

void setup() {
    Serial.begin(115200);
}

void loop() {
    if (Serial.available() >= dataSize * 2) {  // Wait for all bytes
        Serial.readBytes((char *)receivedData, dataSize * 2);
        Serial.println("Binary Data Received:");
        
        // Print received values
        for (int i = 0; i < dataSize; i++) {
            Serial.print(receivedData[i]);
            Serial.print(" ");
        }
        Serial.println();
    }
}