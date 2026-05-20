/*
    {"state":1,"t":0.80,"pertub":1.50,"mv":6.00}

    {"state":2,"t":0.80,"pertub":1.50,"umax":12.00,"umin":0.00,"ref":28.00,"h":0.50}

    {"state":3,"t":0.80,"pertub":1.50,"ref":28.00,"h":0.50,"k":4.00,"ti":1.00,"p":100.00,"td":0.20} 
*/

void serialInput(){
  if (stringComplete) {
    JsonDocument obj;
    deserializeJson(obj, inputString);

    state = obj["state"];
    T = obj["t"];
    perturbation = obj["pertub"];
    String(state).toCharArray(moduleConfig.state, 2);
    String(T).toCharArray(moduleConfig.T, 10);
    String(perturbation).toCharArray(moduleConfig.perturbation, 10);

    if (state == 1){       //msg = "state","t","pertb","mv"
      MV = obj["MV"];
      String(MV).toCharArray(moduleConfig.MV, 10);
    }
    if (state == 2){ // msg = "state","t","pertb","Uma","Umi","ref","h"
      Umax = obj["Uma"];
      Umin = obj["Umi"];
      ref = obj["Ref"];
      h = obj["h"];
      String(Umax).toCharArray(moduleConfig.Umax, 10);
      String(Umin).toCharArray(moduleConfig.Umin, 10);
      String(ref).toCharArray(moduleConfig.ref, 10);
      String(h).toCharArray(moduleConfig.h, 10);
    }
    if (state == 3){ // msg = "state","t","pertb:","ref","h","k","ti","p","td"
      ref = obj["Ref"];
      h = obj["h"];
      K = obj["k"];
      Ti = obj["ti"];
      p = obj["polo"];
      Td = obj["td"];
      String(ref).toCharArray(moduleConfig.ref, 10);
      String(h).toCharArray(moduleConfig.h, 10);
      String(K).toCharArray(moduleConfig.K, 10);
      String(Ti).toCharArray(moduleConfig.Ti, 10);
      String(p).toCharArray(moduleConfig.p, 10);
      String(Td).toCharArray(moduleConfig.Td, 10);
    }
    saveConfig();
    loadConfig();
    inputString = "";
    stringComplete = false;
    erro_ant = 0.0;
    I_ant = 0.0;
    D_ant = 0.0;
  }
}

void serialOutput(){
  JsonDocument outputDoc;
  outputDoc["time"] = axis_x;
  outputDoc["state"] = state;
  outputDoc["t"] = T;
  outputDoc["pertub"] = perturbation;
  outputDoc["MV"] = MV;
  outputDoc["Umax"] = Umax;
  outputDoc["Umin"] = Umin;
  outputDoc["Ref"] = ref;
  outputDoc["h"] = h;
  outputDoc["k"] = K;
  outputDoc["ti"] = Ti;
  outputDoc["polo"] = p;
  outputDoc["td"] = Td;
  float PID = P + I + D;
  outputDoc["PID"] = PID;    
  outputDoc["U"] = U;
  outputDoc["y"] = y;
  serializeJson(outputDoc, Serial);
  Serial.println();
}