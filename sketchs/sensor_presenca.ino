#include <MySensor.h>  
#include <SPI.h>

unsigned long SLEEP_TIME = 120000; //Tempo de espera entre os relatos (em milisegundos)
#define DIGITAL_INPUT_SENSOR 3 //Entrada digital anexada ao sensor de moviemento (apenas 2 e 3 geram uma interrupção)
#define INTERRUPT DIGITAL_INPUT_SENSOR-2 //Geralmente no arduino uno/nano, interrupção = pino-2;
#define CHILD_ID 1   //Id do sensor filho

MySensor gw;
// Initialize motion message
MyMessage msg(CHILD_ID, V_TRIPPED);

void setup()  
{  
  gw.begin();

  //Manda a informaçao de versão do esquemático para o gateway e o controlador
  gw.sendSketchInfo("Motion Sensor", "1.0");

  pinMode(DIGITAL_INPUT_SENSOR, INPUT); //define o pino digital do sensor de movimento como  entrada
 // Registra todos os sensores no gateway (eles serão criados como dispositivos filhos)
  gw.present(CHILD_ID, S_MOTION);
 
}

void loop()     
{     
  //Le o valor de movimento digital
  boolean tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH;
        
  Serial.println(tripped);
  gw.send(msg.set(tripped?"1":"0"));  //Manda o valor disparado para o gateway 
 
  // Dorme até gerar uma interrupção. Manda as atualizações a cada dois minutos
  gw.sleep(INTERRUPT,CHANGE, SLEEP_TIME);
}
