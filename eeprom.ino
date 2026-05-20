

void saveConfig(){
  for (unsigned int t = 0; t < sizeof(moduleConfig); t++) {
    EEPROM.write(CONFIG_START + t, *((char*)&moduleConfig + t));
  }
  //Saving the version ID to pull from EEPROM the next time you load
  EEPROM.write(VERSION_START + 0, CONFIG_VERSION[0]);
  EEPROM.write(VERSION_START + 1, CONFIG_VERSION[1]);
}

void loadConfig(){
  if (EEPROM.read(VERSION_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(VERSION_START + 1) == CONFIG_VERSION[1]) {
    for (unsigned int t = 0; t < sizeof(moduleConfig); t++)
      *((char*)&moduleConfig + t) = EEPROM.read(CONFIG_START + t);
  }
  state = atoi(moduleConfig.state);
  MV = atof(moduleConfig.MV);
  perturbation = atof(moduleConfig.perturbation);
  T = atof(moduleConfig.T);
  ref =  atof(moduleConfig.ref);
  h =  atof(moduleConfig.h);
  Umax =  atof(moduleConfig.Umax);
  Umin =  atof(moduleConfig.Umin);
  K =  atof(moduleConfig.K);
  Ti =  atof(moduleConfig.Ti);
  Td =  atof(moduleConfig.Td);
  p =  atof(moduleConfig.p);
}