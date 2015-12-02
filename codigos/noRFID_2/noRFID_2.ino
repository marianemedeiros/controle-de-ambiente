#include <MySensor.h>
#include <SPI.h>
#include <MFRC522.h>

//MySensor define
#define CHILD_ID_RFID 0   // Id of RFID sensor
#define CHILD_ID_WATT 2

//RFID define
#define SS_PIN 6
#define RST_PIN 5

//RGB define
#define PIN_RED 4
#define PIN_GREEN 3
#define PIN_BLUE 8




#define CHILD_ID_LIGHT 0
#define LIGHT_SENSOR_ANALOG_PIN 0

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MySensor gw;
MyMessage msg(CHILD_ID_RFID, V_IR_RECEIVE);
MyMessage msgRFID(CHILD_ID_WATT, V_VAR1);
//Sleep sleep;
int node_id = 2; //manual sensor_id

void setup() {

  // First make sure mfrc is releasing the SPI bus
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, LOW);

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_GREEN, LOW);
  //gw.begin();
  gw.begin(NULL, node_id); //manual sensor_id

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Light Sensor", "1.0");

  // Register all sensors to gateway (they will be created as child devices)
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);


  Serial.begin(115200); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");


}

void loop() {

  //gw.process(); // Process incomming messages

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card. PICC_HaltA() is automatically called.
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : "");
    Serial.print(mfrc522.uid.uidByte[i], HEX);

  }
  Serial.println();

  mfrc522.PICC_HaltA();

  char carray[mfrc522.uid.size * 2];
  uidToCharArray(mfrc522.uid.uidByte, carray);
  Serial.print("\ID: ");
  Serial.print(carray);

 fflush(stdin);
    if (strcmp (carray, "BC387565") == 0) {
          gw.send(msgRFID.setDestination(3).set(1));
          digitalWrite(PIN_GREEN, HIGH); 
          Serial.println("<<<<<< 1 >>>>>>");
    }
    else if (strcmp (carray, "33CA93F4") == 0){
          gw.send(msgRFID.setDestination(3).set(2));
          digitalWrite(PIN_GREEN, HIGH); 
          Serial.println("<<<<<< 2 >>>>>>");
    }
    else{
      digitalWrite(PIN_RED, HIGH); 
    }

    delay(1000);
    digitalWrite(PIN_GREEN, LOW);
    digitalWrite(PIN_RED, LOW); 
   

          
  //gw.send(msgRFID.setDestination(8).set(temperature));
  //gw.send(msg.set(carray));  // Send UID to gw
}

void uidToCharArray(byte uid[], char carr[]) {
  int j = 0;
  for (int i = 0; i < 4; i++) {
    byte hinibble = (uid[i] >> 4) & 0x0f;
    byte lonibble = uid[i] & 0x0f;
    carr[j++] = binToHexAscii(hinibble);
    carr[j++] = binToHexAscii(lonibble);
  }
  carr[j] = '\0';
}

char binToHexAscii(byte x)
{
  char hex[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  return hex[x & 0x0f];
}



