
#include "ProcessInputClass.h"
ProcessInput Lamp;
ProcessInput Fan; 

#include <Wire.h>
#include "DFRobot_SHT20.h"
DFRobot_SHT20 sht20;

int hallSensor = 2;
volatile uint32_t tachOut;//Váriavel que armazena as rotações.
unsigned long startCountRPM = 0; //tempo do instante de leitura do rpm
int rpm = 0;

int state = 3;
float axis_x = 0.0;

float MV                   =   6.0; //variável manipulada - Tensão da lâmpada (sinal de entrada do processo)
float perturbation         =   0.5; //variável manipulada - Tensão do cooler (fanVoltage); 
float perturbationPrevious =   0.0; //variável auxiliara - verifica variação na tensão do cooler; 
float T                    =   100; //período de amostragem (ms)
float Tsec                 =   0.0; //período de amostragem (s) 
unsigned long startMillis  =     0; //tempo do instante de início do período da amostragem
float y                    =   0.0; //valor do sensor de temperatura (tempInt_SHT20)
float ref                  =  28.0; //valor dreferência (ºC)
float erro                 =   0.0; //valor de dreferência (ref) menos o valor do sensor de temperatura (y)
float h                    =   0.5; //valor dhisterese (ºC)
float U                    =   0.0; //sinal de saída do controle 
float Umax                 =  12.0; //valor máximo do Relé
float Umin                 =   0.0; //valor mínimo do Relé
int firstSamplingTime      =     0; //variável auxiliar usada para rodar um trecho de código 1 vez somente
float K                    =   4.0; //ganho proporcional  (ação proporcional)
float Ti                   =   1.0; //tempo integral      (ação integral)
float Td                   =   0.2; //tempo derivativo    (ação derevativa)
float p                    = 100.0; //polo do filtro da ação derivativa (pólo limitador de ganho em alta frequência -> p = 100)
float erro_ant             =   0.0; //erro da amostragem do tempo T anterior
float I_ant                =   0.0; //sinal de controle Integral do tempo T anterior
float D_ant                =   0.0; //sinal de controle Derivativo do tempo T anterior
float P                    =   0.0; //sinal de controle Proporcional
float I                    =   0.0; //sinal de controle Integral
float D                    =   0.0; //sinal de controle Derivativo

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
unsigned long startSendSerial  =     0; //tempo do instante de início do recebimento de uma mensagem
unsigned long periodSendSerial =  2000; //período sem enviar mensagens
//-----------------------------------------------
//Parâmetros de configuração da EEPROM
//-----------------------------------------------
#define VERSION_START  500
#define CONFIG_START   6
#define CONFIG_VERSION "1a"

struct ConfigStruct{
  char state[2];
  char MV[10];
  char perturbation[10];
  char T[10];
  char ref[10];
  char h[10];
  char Umax[10];
  char Umin[10];
  char K[10];
  char Ti[10];
  char Td[10];
  char p[10];
} moduleConfig;
//-----------------------------------------------

//-----------------------------------------------