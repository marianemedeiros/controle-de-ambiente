#include <SPI.h>
#include <MySensor.h>  

#define CHILD_ID_LIGHT 0
#define LIGHT_SENSOR_ANALOG_PIN 0

unsigned long SLEEP_TIME = 30000; //Tempo de espera entre as leituras (em milisegundos)
MySensor gw;
MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
int lastLightLevel;

void setup()  
{
  gw.begin();

  //Manda a informaçao de versão do esquemático para o gateway e o controlador
  gw.sendSketchInfo("Light Sensor", "1.0");

 // Registra todos os sensores no gateway (eles serão criados como dispositivos filhos)
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);
}

void loop()      
{     
  int lightLevel = (1023-analogRead(LIGHT_SENSOR_ANALOG_PIN))/10.23;
  Serial.println(lightLevel);
  if (lightLevel != lastLightLevel) {
      gw.send(msg.set(lightLevel));
      lastLightLevel = lightLevel;
  }
  gw.sleep(SLEEP_TIME);
}
