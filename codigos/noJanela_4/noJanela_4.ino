#include <MySigningNone.h>
#include <MyTransportNRF24.h>
#include <MyTransportRFM69.h>
#include <MyHwATMega328.h>
#include <MySensor.h>
#include <SPI.h>

#define RELAY_1  3  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 1 // Total number of attached relays
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

#define CHILD_ID_WATT 1
MyMessage msgTrava(CHILD_ID_WATT, V_VAR2);

// NRFRF24L01 radio driver (set low transmit power by default) 
MyTransportNRF24 radio(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);  
//MyTransportRFM69 radio;
// Message signing driver (none default)
//MySigningNone signer;
// Select AtMega328 hardware profile
MyHwATMega328 hw;
// Construct MySensors library
MySensor gw(radio, hw);

int node_id = 4;
void setup()  
{   
  // Initialize library and add callback for incoming messages
  gw.begin(incomingMessage, node_id, true);
  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Relay", "1.0");

  // Fetch relay status
  for (int sensor=1, pin=RELAY_1; sensor<=NUMBER_OF_RELAYS;sensor++, pin++) {
    // Register all sensors to gw (they will be created as child devices)
    gw.present(sensor, S_LIGHT);
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);   
    // Set relay to last known state (using eeprom storage) 
    digitalWrite(pin, gw.loadState(sensor)?RELAY_ON:RELAY_OFF);
  }
}


void loop() 
{
  // Alway process incoming messages whenever possible
  gw.process();
}

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) {
     // Change relay state
     if(message.getBool()){
      digitalWrite(message.sensor-1+RELAY_1, RELAY_ON);
      gw.send(msgTrava.setDestination(3).set(1));
     }
     else{
      digitalWrite(message.sensor-1+RELAY_1, RELAY_OFF);
      gw.send(msgTrava.setDestination(3).set(0));
     }
//     digitalWrite(message.sensor-1+RELAY_1, message.getBool()?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     gw.saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}

