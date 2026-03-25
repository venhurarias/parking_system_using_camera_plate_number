#include <Adafruit_Fingerprint.h>
#include <Adafruit_NeoPixel.h>
#include <Chrono.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Streaming.h>

#define RST_PIN 49  // Configurable, see typical pin layout above
#define SS_PIN 53   // Configurable, see typical pin layout above
#define PIXEL_PIN 48
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

uint8_t id;
const int numSensor = 39;

const int sensorPins[numSensor] = { 15, 16, 17, 2, 3, 4, 9, 7, 46, 5, 12, 14, 6, 45, 8, A13, 20, 21, 44, 24, 22, 25, 27, 26, 29, 28, 30, 31, 32, 33, 43, 42, 41, 40, 39, 38, 37, 36, 34, 35 };
const int ledPins[numSensor] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 38, 37, 36, 35, 34, 33, 32, 31, 30 };
int assignedSlot[numSensor] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//0 - unavailable
//1 - available
//2 - assigned



Adafruit_NeoPixel strip(numSensor, PIXEL_PIN, NEO_RGB + NEO_KHZ800);
Chrono myChrono;




void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  Serial.setTimeout(100);

  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  finger.begin(57600);
  finger.getParameters();
  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
  }
  strip.begin();  // Initialize NeoPixel strip object (REQUIRED)
  strip.show();   // Initialize all pixels to 'off'
}

void loop() {
  // getFingerprintID();
  // writeRFID();
  // enrollFingerPrint();
  // sensorTest();

  normalProcess();
}

int getSlotIndex() {
  for (int i = 0; i < numSensor; i++) {
    if (assignedSlot[i] == 1) {
      return i;
    }
  }
  return -1;
}

bool startsWithAndEndsWith(String str, char startChar, char endChar) {
  if (str.length() < 2) {
    // String is too short to have both start and end characters
    return false;
  }
  return (str.charAt(0) == startChar) && (str.charAt(str.length() - 1) == endChar);
}


String extractBetween(String str, char startChar, char endChar) {
  if (str.length() < 2) {
    // String is too short to have both start and end characters
    return "-1";
  }
  if ((str.charAt(0) == startChar) && (str.charAt(str.length() - 1) == endChar)) {
    // Extract the substring between the start and end characters
    return str.substring(1, str.length() - 1);
  }
  // Return an empty string if the conditions are not met
  return "-1";
}

void normalProcess() {

  if (Serial.available()) {
    String reading = Serial.readString();
    Serial << reading << endl;
    reading.trim();
    if (reading == "0") {
      int slot = getSlotIndex();
      if (slot < 0) {
        Serial << "No Slot Available" << endl;
      } else {
        Serial << "*" << slot << "#" << endl;
        assignedSlot[slot] = 2;
      }
    } else if (startsWithAndEndsWith(reading, '@', '#')) {
      id = extractBetween(reading, '@', '#').toInt();
      if (id > 0) {
        while (!getFingerprintEnroll())
          ;
      }
    } else if (reading == "2") {
      int finger = 0;
      while (finger < 1) {
        finger = getFingerprintIDez();
        Serial << finger << endl;
        delay(100);
      }
      Serial << "done" << endl;
    }
  }
  if (myChrono.hasPassed(2000)) {
    myChrono.restart();
    for (int i = 0; i < numSensor; i++) {
      switch (assignedSlot[i]) {
        case 0:  //unavailable - may sasakyan
          if (getSensorVal(i)) {
            strip.setPixelColor(ledPins[i], strip.Color(0, 255, 0));
          } else {
            assignedSlot[i] = 1;
            strip.setPixelColor(ledPins[i], strip.Color(0, 0, 0));
            Serial << "(" << i << ")" << endl;
          }
          break;

        case 1:  //available
          if (getSensorVal(i)) {
            strip.setPixelColor(ledPins[i], strip.Color(255, 0, 0));
          } else {
            strip.setPixelColor(ledPins[i], strip.Color(0, 0, 0));
          }
          break;

        case 2:  //assigned
          if (getSensorVal(i)) {
            assignedSlot[i] = 0;
            strip.setPixelColor(ledPins[i], strip.Color(0, 255, 0));
          } else {
            strip.setPixelColor(ledPins[i], strip.Color(0, 0, 255));
          }
          break;
      }
    }
    strip.show();
  }
}

void sensorTest() {
  if (myChrono.hasPassed(2000)) {
    myChrono.restart();
    for (int i = 0; i < numSensor; i++) {
      if (getSensorVal(i)) {
        strip.setPixelColor(ledPins[i], strip.Color(0, 255, 0));
      } else {
        strip.setPixelColor(ledPins[i], strip.Color(255, 0, 0));
      }
    }
    strip.show();
  }
}

bool getSensorVal(int x) {
  return !digitalRead(sensorPins[x]);
}

void enrollFingerPrint() {
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  id = readnumber();
  if (id == 0) {  // ID #0 not allowed, try again!
    return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (!getFingerprintEnroll())
    ;
}



uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}



uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        // Serial.println("Image taken");

        break;
      case FINGERPRINT_NOFINGER:
        // Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        // Serial.println("Communication error");
        Serial << "!" << endl;
        break;
      case FINGERPRINT_IMAGEFAIL:
        // Serial.println("Imaging error");
        Serial << "!" << endl;
        break;
      default:
        // Serial.println("Unknown error");
        Serial << "!" << endl;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");

      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      Serial << "!" << endl;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      Serial << "!" << endl;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      Serial << "!" << endl;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      Serial << "!" << endl;
      return p;
    default:
      // Serial.println("Unknown error");
      Serial << "!" << endl;
      return p;
  }

  Serial.println("Remove finger");
  Serial << "##" << endl;
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        // Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        // Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        // Serial.println("Communication error");
        Serial << "!" << endl;
        break;
      case FINGERPRINT_IMAGEFAIL:
        // Serial.println("Imaging error");
        Serial << "!" << endl;
        break;
      default:
        // Serial.println("Unknown error");
        Serial << "!" << endl;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      // Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      // Serial.println("Image too messy");
      Serial << "!" << endl;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      // Serial.println("Communication error");
      Serial << "!" << endl;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      // Serial.println("Could not find fingerprint features");
      Serial << "!" << endl;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      // Serial.println("Could not find fingerprint features");
      Serial << "!" << endl;
      return p;
    default:
      // Serial.println("Unknown error");
      Serial << "!" << endl;
      return p;
  }



  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    // Serial.println("Prints matched!");
    Serial << "!" << endl;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    // Serial.println("Communication error");
    Serial << "!" << endl;
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    // Serial.println("Fingerprints did not match");
    Serial << "!" << endl;
    return p;
  } else {
    // Serial.println("Unknown error");
    Serial << "!" << endl;
    return p;
  }

  // Serial.print("ID ");
  // Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    // Serial.println("Communication error");
    Serial << "!" << endl;
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    // Serial.println("Could not store in that location");
    Serial << "!" << endl;
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    // Serial.println("Error writing to flash");
    Serial << "!" << endl;
    return p;
  } else {
    // Serial.println("Unknown error");
    Serial << "!" << endl;
    return p;
  }

  return true;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

void writeRFID() {
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("Card UID:"));  //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(F(" PICC type: "));  // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte len;

  Serial.setTimeout(20000L);  // wait until 20 seconds for input from serial
  // Ask personal data: Family name
  Serial.println(F("Type Family name, ending with #"));
  len = Serial.readBytesUntil('#', (char *)buffer, 30);  // read family name from serial
  for (byte i = len; i < 30; i++) buffer[i] = ' ';       // pad with spaces

  block = 1;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else Serial.println(F("PCD_Authenticate() success: "));

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else Serial.println(F("MIFARE_Write() success: "));

  block = 2;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else Serial.println(F("MIFARE_Write() success: "));

  // Ask personal data: First name
  Serial.println(F("Type First name, ending with #"));
  len = Serial.readBytesUntil('#', (char *)buffer, 20);  // read first name from serial
  for (byte i = len; i < 20; i++) buffer[i] = ' ';       // pad with spaces

  block = 4;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else Serial.println(F("MIFARE_Write() success: "));

  block = 5;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else Serial.println(F("MIFARE_Write() success: "));


  Serial.println(" ");
  mfrc522.PICC_HaltA();       // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}
