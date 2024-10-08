/*
   -------sd------------------------------------------------------------------------------------------------------------
   Example to change UID of changeable MIFARE card.
   --------------------------------------------------------------------------------------------------------------------
   This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid

   This sample shows how to set the UID on a UID changeable MIFARE card.
   NOTE: for more informations read the README.rst

   @author Tom Clement
   @license Released into the public domain.

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
#include <MFRC522.h>
#include <MFRC522Hack.h>

constexpr uint8_t RST_PIN = 9; // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10; // Configurable, see typical pin layout above
const int tone_pin = 3;
const int LED_pin = 4;
const int DO_frequency = 442;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522Hack mfrc522Hack(&mfrc522); // Create MFRC522Hack instance.

/* Set your new UID here! */
// byte newUid[] = {0x36, 0xE5, 0x5B, 0x6F};

MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial)
    ; // Do nothing if no serial port is opened (added for Arduinos based on
      // ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println(F("Warning: this example overwrites the UID of your UID "
                   "changeable card, use with care!"));

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the
  // factory.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  pinMode(tone_pin, OUTPUT);
  pinMode(LED_pin, OUTPUT);
}

// Setting the UID can be as simple as this:
// void loop() {
//  byte newUid[] = {0x65, 0x2D, 0xFC, 0x3C};
//  if ( mfrc522Hack.MIFARE_SetUid(newUid, (byte)4, true) ) {
//    Serial.println("Wrote new UID to card.");
//  }
//  delay(1000);
//}

// But of course this is a more proper approach
unsigned long last_blink = 0;
uint8_t led_status = 0;
void loop() {
  if(millis() - last_blink > 500) {
    digitalWrite(LED_pin, led_status);
    led_status = !led_status;
    last_blink = millis();
  }

  // Look for new cards, and select one if present
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  // Dump UID
  Serial.print(F("Source Card UID:"));
  byte newUid[4] = {0, 0, 0, 0};

  for (byte i = 0; i < 4; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    newUid[i] = mfrc522.uid.uidByte[i];
  }
  buzzer_start();
  delay(3000);

  //  for (byte i = 0; i < mfrc522.uid.size; i++) {
  //    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //    Serial.print(mfrc522.uid.uidByte[i], HEX);
  //  }
  Serial.println();

  // Dump PICC type
  //  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  //  Serial.print(F("PICC type: "));
  //  Serial.print(mfrc522.PICC_GetTypeName(piccType));
  //  Serial.print(F(" (SAK "));
  //  Serial.print(mfrc522.uid.sak);
  //  Serial.print(")\r\n");
  //  if (  piccType != MFRC522::PICC_TYPE_MIFARE_MINI
  //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
  //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
  //    Serial.println(F("This sample only works with MIFARE Classic cards."));
  //    return;
  //  }

  // Set new UID
  if (mfrc522Hack.MIFARE_SetUid(newUid, (byte)4, true)) {
    Serial.println(F("Copy UID to destination."));
  } else {
    Serial.println("Failed to copy UID to destination");
    buzzer_error();
    delay(3000);
    return;
  }
  Serial.print("Destination Card UID: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Halt PICC and re-select it so DumpToSerial doesn't get confused
  if(mfrc522.PICC_HaltA() != MFRC522::StatusCode::STATUS_OK) {
    Serial.println("Failed to Halt");
    buzzer_error();
    delay(3000);
    return;
  }

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    Serial.println("No new card is presented or unable to read card serial");
    buzzer_error();
    delay(3000);
    return;
  }

  buzzer_pass();
  // Dump the new memory contents
  //  Serial.println(F("New UID and contents:"));
  //  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  delay(2000);
}


void buzzer_error(void) {
  for(int i = 0; i < 3; i++) {
    tone(tone_pin, DO_frequency);
    delay(250);
    noTone(tone_pin);
    delay(100);
  }
}

void buzzer_pass(void) {
  for(int i = 0; i < 2; i++) {
    tone(tone_pin, DO_frequency);
    delay(250);
    noTone(tone_pin);
    delay(100);
  }
}

void buzzer_start(void) {
  for(int i = 0; i < 1; i++) {
    tone(tone_pin, DO_frequency);
    delay(250);
    noTone(tone_pin);
    delay(100);
  }
}