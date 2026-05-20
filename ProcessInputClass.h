
class ProcessInput {  
  int spd = 255, pin1, pin2;
  
  public:  
  
    void Pinout(int in1, int in2){
      pin1 = in1;
      pin2 = in2;
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
    }   
    
    void ApplyIntensity(int in1){
      spd = in1;
      analogWrite(pin1, spd);
      digitalWrite(pin2, LOW);
    } 
    
    void TurnOn(){
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
    }  
    
    void TurnOff(){
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
    }

};