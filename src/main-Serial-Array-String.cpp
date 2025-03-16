//*Send array of int in string based way.

//readable from Serial Monitor

//Example : START,1,2,3,4,END

#include <Arduino.h>

const int dataSize = 4;
uint16_t receivedData[dataSize];
int i = 0;
bool receiving = false;

void setup() {
    Serial.begin(115200);
}

void loop() {
    if (Serial.available()) {
        Serial.println("some serial init");
        String data = Serial.readStringUntil('\n');  // Read full line
        if (data.startsWith("START") && data.endsWith("END")) {
            data.replace("START,", "");
            data.replace(",END", "");

            i = 0;
            char *token = strtok((char *)data.c_str(), ",");

            while (token != NULL && i < dataSize) {
                receivedData[i++] = atoi(token);
                token = strtok(NULL, ",");
            }

            Serial.println("Data Received Successfully!");
            for (int i = 0; i < dataSize; i++) {
                Serial.print(receivedData[i]);
                Serial.print(" ");
            }
            Serial.println();
        }
        Serial.println("some serial");
    }
}
