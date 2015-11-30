/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik EKblad
 *
 * DESCRIPTION
 * Example sketch showing how to measue light level using a LM393 photo-resistor
 * http://www.mysensors.org/build/light
 */

#include <SPI.h>
#include <MySensor.h>
#include <IRremote.h>
#define CHILD_ID_LIGHT 0

IRsend irsend;

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)
MySensor gw;

MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
int temperaturaIdeal = 0;

int nodeid = 3;
int TRAVA = 0;

int lastID = 0;
#define DESLIGA 0
#define ID1 1
#define ID2 2


void sendraw(unsigned int *irSignal) {
  int i;
  for(i = 0; i<3; i++){
    int khz = 38; // 38kHz carrier frequency for the NEC protocol 
    irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.
    delay(5000);
  }
}

void setup(){
  gw.begin(incomingMessage, nodeid);
  
  gw.sendSketchInfo("Light Sensor", "1.0");
  // Register all sensors to gateway (they will be created as child devices)
    gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);

}

void loop(){
  gw.process();
}

void gerenciaAr(int idAtual){

 if(TRAVA == 0){
 switch (idAtual){
      case ID1:{
          unsigned int irSignal[] = {9000, 4500, 560, 560, 560, 560, 560};
          temperaturaIdeal = 22;
          sendraw(irSignal);
          break;
        }
      case ID2:{
          unsigned int irSignal[] = {9000, 4500, 560, 560, 560, 560, 560};
          sendraw(irSignal);
          temperaturaIdeal = 30;
          break;
        }
      case DESLIGA:{
          unsigned int irSignal[] = {9000, 4500, 560, 560, 560, 560, 560};
          temperaturaIdeal = 50;
          sendraw(irSignal);
          break;
        }             
      }
  }
  else{
    unsigned int irSignal[] = {9000, 4500, 560, 560, 560, 560, 560};
    temperaturaIdeal = 50;
    sendraw(irSignal);
  }
 }

void incomingMessage(const MyMessage &message){
  
  // We only expect one type of message from controller. But we better check anyway.
  if (message.isAck()){
    Serial.println("This is an ack from gateway");
  }
  //read: 7-0-8 s=1,c=1,t=25,pt=2,l=2:486
  if (message.type == V_VAR1){
    int id;
    id = message.getInt();
    if(lastID == id){
      gerenciaAr(0);
      lastID=0;
    }
    else{
      gerenciaAr(id);
      lastID = id;  
      gw.send(msg.set(temperaturaIdeal));
      delay(1000);
      Serial.println("Var1");
      Serial.println(id);
    }
  }

  if (message.type == V_VAR2){
    TRAVA = message.getInt();
    if(TRAVA == 0){
      gerenciaAr(lastID);
    }
    else{
      gerenciaAr(0);
    }
  }
  
}





