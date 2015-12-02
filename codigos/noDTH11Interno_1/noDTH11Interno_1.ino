#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>  

#define CHILD_ID_HUM 0 //ID do sensor filho (umidade)
#define CHILD_ID_TEMP 1 //ID do sensor filho (temperatura)
#define HUMIDITY_SENSOR_DIGITAL_PIN 3
unsigned long SLEEP_TIME = 30000;

MySensor gw;
DHT dht;
float lastTemp;
float lastHum;
boolean metric = true; 
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);

int node_id = 1;
void setup()  
{ 
  // Inicia a biblioteca mysensor com o ID do dispositivo.
  gw.begin(NULL, node_id);
  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN); 

  // Manda a informaçao de versão do esquemático para o gateway e o controlador
  gw.sendSketchInfo("Humidity", "1.0");

  // Registra todos os sensores no gateway (eles serão criados como dispositivos filhos)
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
  
  metric = gw.getConfig().isMetric;
}

void loop()      
{  
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT");
  } else if (temperature != lastTemp) {
    lastTemp = temperature;
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    gw.send(msgTemp.set(temperature, 1));
    Serial.print("T: ");
    Serial.println(temperature);
  }
  
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      lastHum = humidity;
      gw.send(msgHum.set(humidity, 1));
      Serial.print("H: ");
      Serial.println(humidity);
  }

  gw.sleep(SLEEP_TIME); //Dorme um pouco
}


