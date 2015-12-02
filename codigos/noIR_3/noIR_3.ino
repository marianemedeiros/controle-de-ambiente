#include <SPI.h>
#include <MySensor.h>
#include <IRremote.h>
#define CHILD_ID_LIGHT 0

IRsend irsend;

unsigned long SLEEP_TIME = 30000;
MySensor gw;

MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
int temperaturaIdeal = 0;

int nodeid = 3;
int TRAVA = 0;
int id=0;
int lastID = 99;
#define DESLIGA 0
#define ID1 1
#define ID2 2


/**
*   Essa função é responsável por enviar o sinal infravermelho para o ar-condicionado.
*   Recebe como parâmetro um vetor de inteiro que contém a mensagem que será enviada ao  
*     ar condicionado.
**/
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
  // registra os sensores no gateway
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);
  
}

void loop(){
  gw.process();

  // É enviado ao controlador a temperatura ideal da sala, ou seja, a temperatura que o usuário
  // deseja no ar-condicioando. Então só enviado a temperatura novamente quando mudar de usuário.
  if (id != lastID) {
      gw.send(msg.set(temperaturaIdeal));
      lastID = id;
  }

}

/*
*   Essa função é responsável por enviar a mensagem correta para o ar condicionado.
*   Recebe como parâmetro um ID para que seja possível relacionar com as configurações 
*     personalizadas do ar condicionado de cada usuário e envia essa mensagem para
*     a função sendraw;
*   Caso o ID seja '0' é enviado a mensagem de desligamento para o ar condicionado.
*
*/
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
          temperaturaIdeal = 0;
          sendraw(irSignal);
          break;
        }             
      }
  }
  else{
    // Se a trava estiver ativada é enviada a mensagem de desligamento para o ar-condicionado.
    unsigned int irSignal[] = {9000, 4500, 560, 560, 560, 560, 560};
    temperaturaIdeal=TRAVA;
    lastID = id;
    id = 0;
    sendraw(irSignal);
  }
 }


// Essa função é responsável por receber as mensagens vindas dos outros nós.
// V_VAR1 é a mensagem contendo o ID do usuário.
// V_VAR2 ativa e desativa a TRAVA.
void incomingMessage(const MyMessage &message){
  
  if (message.isAck()){
    Serial.println("This is an ack from gateway");
  }

  // ID do usuário.
  if (message.type == V_VAR1){
    lastID = id;
    id = message.getInt();
    if(lastID == id){
      gerenciaAr(0);
      lastID=0;
    }
    else{
      gerenciaAr(id);
      
      Serial.println("Var1");
      Serial.println(id);
    }
  }

// TRAVA
  if (message.type == V_VAR2){
    TRAVA = message.getInt();
    if(TRAVA == 0){
      // envia 0 para o controlador (significa que a trava foi desativada e o ar está desligado)
      gw.send(msg.set(0));      
    }
    else{
      gerenciaAr(0);
      // envia 1 para o controlador (significa que a trava foi ativada e o ar irá ser desligado)
      gw.send(msg.set(1));
    }
  }
  
}





