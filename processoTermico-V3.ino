#include "Config.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
#define RESET asm ("jmp (0x0000)")

void setup() {
  pinMode(hallSensor, INPUT);
  Serial.begin(57600);
  loadConfig();
  delay(100);
  //state = 0;
  //String(state).toCharArray(moduleConfig.state, 2);
  Wire.begin();  
  Lamp.Pinout(5,6); // Selection of lamp pins as described in the class.
  Fan.Pinout(9,10); // Selection of cooler pins, as described in the class.
  inputString.reserve(200);   // reserve 200 bytes for the inputString.
  attachInterrupt(0, ctt, RISING);//Habilita a interrupção 0 no pino 2.
}

void loop() {

  // Updates manipulable variables when a new message arrives via serial
  serialInput();

  //Run one of the routines every T milliseconds
  float periodo = T * 1000.0;
  
  if (millis() - startSendSerial >= periodSendSerial){
    if (millis() - startMillis >= periodo){
      axis_x = axis_x + ((millis() - startMillis) / 1000.0);
      startMillis = millis();    
      switch (state) {
        case 1://ROTINA ENSAIO MALHA ABERTA
          y = sht20.readTemperature();
          U = MV;
          break;
        case 2://ROTINA BANG-BANG
          y = sht20.readTemperature();
          erro = ref - y;         
          
          //executes only at the first sampling instant
          if( firstSamplingTime == 0 ){
            firstSamplingTime = 1;
            if ( erro > 0 ){
              U = Umax;
            }else{
              U = Umin;
            } 
          //executes from the second sampling instant
          }else{
            if ( U == Umax ){

              if ( erro < -h ){
                  U = Umin;
              }else{
                  U = Umax;
              }
            }else{
              if ( erro > h ){
                U = Umax;
              }else{  
                U = Umin;
              }
            } 
          }
          break;
        case 3://ROTINA PID
          Tsec = T;       
          y = sht20.readTemperature();
          erro = ref - y;
          P = K * erro;
          I = I_ant + K*Tsec*(erro+erro_ant)/(2.0*Ti);
          D = D_ant*(2.0-p*Tsec)/(2.0+p*Tsec) + ((2.0*p*K*Td)/(2.0+p*Tsec))*(erro-erro_ant);
          U = P + I + D;
          erro_ant = erro;
          I_ant = I;
          D_ant = D;
          break;    
        default:
          String txtState =    "3";
          String txtMV    =  "6.0";
          String txtT     =  "0.1";
          String txtPert  =  "0.5";
          String txtRef   = "28.0";
          String txtHist  =  "0.5";
          String txtUmax  = "12.0";
          String txtUmin  =  "0.0";
          String txtK     =  "4.0";
          String txtTi    =  "1.0";
          String txtTd    =  "0.2";
          String txtPolo  = "100.0";
          txtState.toCharArray(moduleConfig.state, 2);
          txtMV.toCharArray(moduleConfig.MV, 10);
          txtT.toCharArray(moduleConfig.T, 10);
          txtPert.toCharArray(moduleConfig.perturbation, 10);
          txtRef.toCharArray(moduleConfig.ref, 10);
          txtHist.toCharArray(moduleConfig.h, 10);
          txtUmax.toCharArray(moduleConfig.Umax, 10);
          txtUmin.toCharArray(moduleConfig.Umin, 10);
          txtK.toCharArray(moduleConfig.K, 10);
          txtTi.toCharArray(moduleConfig.Ti, 10);
          txtTd.toCharArray(moduleConfig.Td, 10);
          txtPolo.toCharArray(moduleConfig.p, 10);
          saveConfig();
          delay(100);
          loadConfig();
          delay(100);
          break;    
      }
      //sets the value (range from 0 to 12): Maximum lamp voltage is 12 Volts
      if(U <  0.0) U =  0.0; 
      if(U > 12.0) U = 12.0;
      applyInTheProcess(U);      
      serialOutput();
    }
    
    //Voltage value to be applied to the cooler;
    if(perturbation != perturbationPrevious){
      perturbationPrevious = perturbation; 
      applyPerturbation();
    }

    if (millis() - startCountRPM >= 1000){
      // Desabilita as interrupções
      cli();
      rpm = (30000 / (millis() - startCountRPM)) * tachOut;
      tachOut = 0;
      // Habilita as interrupções
      sei();
      startCountRPM = millis();
      //Como estamos lendo a cada 1 segundo, seria tachOut*60 para determinar as rotações por minuto,
      //porém, o Efeito Hall ativa a saída duas vezes por rotação, deve-se dividir a contagem das rotações por 2.
    }
  }
}

void applyInTheProcess(float _U){
  //Convert Voltage into bits
  int fadeValue = (_U * 255.0) / 24.0;
  Lamp.ApplyIntensity(fadeValue);
}

void applyPerturbation(){
  //sets the value (range from 0 to 255): 24 Volt voltage source. Cooler has a voltage of 24 Volts.
  int fadeValueFan = (perturbation*255.0)/24.0;  
  if(fadeValueFan <  0) fadeValueFan =  0; 
  if(fadeValueFan > 255) fadeValueFan = 255;
  Fan.ApplyIntensity(255);
  delay(500);
  Fan.ApplyIntensity(fadeValueFan);
}

void serialEvent() {
  while (Serial.available()) {
    startSendSerial = millis(); 
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void ctt(){
  tachOut++;
}
