/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.1 January, 2019
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

 #include <Arduino.h>
 #include <IRremoteESP8266.h>
 #include <IRsend.h>
 
 const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
 
 IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
 
 // Example of data captured by IRrecvDumpV2.ino
 uint16_t rawData[583] = {440, 396,  466, 398,  464, 400,  464, 402,  460, 404,  464, 25278,  3474, 1720,  438, 1270,  466, 396,  434, 430,  464, 400,  466, 1288,  442, 400,  468, 396,  458, 406,  464, 400,  464, 1290,  442, 400,  464, 1288,  438, 1292,  440, 400,  460, 1294,  412, 1318,  412, 1320,  410, 1298,  460, 1290,  418, 422,  434, 430,  466, 1288,  414, 428,  434, 430,  460, 404,  468, 396,  466, 398,  436, 430,  436, 428,  468, 396,  462, 402,  460, 404,  464, 1288,  438, 402,  466, 1288,  438, 402,  468, 396,  462, 404,  436, 1296,  464, 1288,  410, 430,  466, 398,  464, 402,  436, 428,  460, 404,  466, 400,  464, 402,  468, 396,  464, 400,  434, 430,  466, 398,  462, 404,  462, 402,  436, 430,  464, 400,  464, 400,  462, 1292,  412, 1318,  412, 1298,  456, 406,  460, 1294,  442, 400,  462, 1290,  440, 1290,  442, 34608,  3468, 1726,  442, 1288,  438, 404,  466, 398,  466, 398,  470, 1284,  414, 428,  468, 396,  464, 400,  460, 404,  460, 1292,  440, 402,  466, 1288,  438, 1292,  440, 402,  462, 1290,  438, 1292,  412, 1318,  438, 1294,  410, 1318,  442, 400,  466, 398,  466, 1288,  440, 400,  462, 402,  462, 402,  462, 402,  436, 430,  468, 396,  460, 404,  466, 398,  462, 402,  464, 400,  464, 400,  438, 1316,  442, 398,  464, 400,  464, 400,  462, 402,  460, 1294,  442, 400,  464, 400,  462, 402,  436, 430,  460, 404,  462, 404,  466, 398,  458, 406,  460, 404,  460, 404,  460, 404,  460, 404,  464, 400,  462, 402,  434, 430,  436, 428,  464, 400,  462, 402,  466, 398,  462, 1292,  436, 404,  466, 1288,  412, 430,  464, 1288,  410, 430,  460, 34614,  3466, 1726,  440, 1290,  440, 400,  466, 398,  466, 398,  468, 1286,  442, 400,  462, 402,  468, 396,  436, 428,  464, 1290,  436, 404,  466, 1286,  440, 1290,  412, 430,  464, 1288,  440, 1290,  414, 1318,  442, 1288,  438, 1292,  412, 430,  464, 400,  466, 1288,  442, 398,  458, 406,  464, 402,  464, 400,  464, 400,  434, 430,  440, 426,  460, 404,  466, 398,  466, 398,  464, 400,  464, 402,  462, 402,  466, 398,  464, 402,  436, 428,  466, 400,  458, 406,  466, 398,  460, 404,  468, 396,  460, 1292,  416, 426,  462, 402,  464, 402,  464, 400,  460, 404,  464, 1290,  442, 1288,  444, 1286,  440, 1288,  438, 404,  464, 1288,  442, 1288,  438, 404,  466, 398,  466, 400,  464, 400,  460, 404,  434, 430,  460, 404,  464, 1288,  442, 398,  466, 398,  462, 404,  436, 430,  466, 398,  462, 1292,  436, 406,  462, 1292,  444, 398,  436, 428,  466, 400,  436, 428,  466, 400,  468, 396,  464, 402,  436, 428,  434, 430,  460, 404,  468, 398,  464, 400,  468, 398,  462, 402,  466, 398,  462, 402,  458, 406,  462, 1290,  440, 1288,  442, 400,  468, 396,  468, 398,  458, 406,  436, 428,  466, 400,  460, 404,  436, 428,  460, 406,  468, 396,  466, 1286,  438, 1292,  444, 398,  462, 404,  434, 430,  464, 400,  464, 400,  436, 430,  464, 400,  466, 400,  462, 404,  460, 406,  464, 402,  456, 410,  434, 452,  412, 452,  410, 454,  414, 452,  412, 452,  414, 1314,  418, 450,  440, 424,  414, 452,  438, 426,  440, 402,  460, 1268,  464, 1266,  464, 400,  462, 402,  458, 406,  460, 404,  460, 404,  436, 430,  460, 404,  458, 406,  458, 408,  458, 406,  436, 428,  434, 430,  434, 430,  432, 432,  442, 424,  432, 432,  432, 1318,  412, 1318,  412, 1320,  412, 1318,  412, 1318,  410, 1320,  410, 434,  430, 456,  408};
 // Example Samsung A/C state captured from IRrecvDumpV2.ino
 uint8_t samsungState[kSamsungAcStateLength] = {
     0x02, 0x92, 0x0F, 0x00, 0x00, 0x00, 0xF0,
     0x01, 0xE2, 0xFE, 0x71, 0x40, 0x11, 0xF0};
 
 void setup() {
   irsend.begin();
 #if ESP8266
   Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
 #else  // ESP8266
   Serial.begin(115200);
 #endif  // ESP8266
 }
 
 void loop() {
//    Serial.println("NEC");
//    irsend.sendNEC(0x00FFE01FUL);
//    delay(2000);
//    Serial.println("Sony");
//    irsend.sendSony(0xa90, 12, 2);  // 12 bits & 2 repeats
//    delay(2000);
   Serial.println("a rawData capture from IRrecvDumpV2");
   irsend.sendRaw(rawData, 583, 38);  // Send a raw data capture at 38kHz.
   delay(2000);
//    Serial.println("a Samsung A/C state from IRrecvDumpV2");
//    irsend.sendSamsungAC(samsungState);
//    delay(2000);
 }
 