

void app_setup(){
  
  server.on("/m",[](){
    if(!server.hasArg("p")){
      senderr(ERR_NO_PIN);
      return;
    }
    if(!server.hasArg("m")){
      senderr(ERR_NO_MODE);
      return;
    }
    pinMode(atoi(server.arg("p").c_str()),atoi(server.arg("m").c_str()));
    sendok();
  });
  server.on("/w",[](){
    if(!server.hasArg("p")){
      senderr(ERR_NO_PIN);
      return;
    }
    if(!server.hasArg("m")){
      senderr(ERR_NO_MODE);
      return;
    }
    digitalWrite(atoi(server.arg("p").c_str()),atoi(server.arg("m").c_str()));
    sendok();
  });
  server.on("/r",[](){
    if(!server.hasArg("p")){
      senderr(ERR_NO_PIN);
      return;
    }
  server.send(202, "text/plain", String(digitalRead(atoi(server.arg("p").c_str())))); 
  });
  server.on("/p",[](){
    if(!server.hasArg("p")){
      senderr(ERR_NO_PIN);
      return;
    }
    bool pulsestate = 1;
    if(server.hasArg("m")){
      pulsestate = atoi(server.arg("m").c_str());
    }
    if(!server.hasArg("t")){
      //senderr("no mode");
      return;
    }
    digitalWrite(atoi(server.arg("p").c_str()),pulsestate);
    delay(atoi(server.arg("p").c_str()));
    digitalWrite(atoi(server.arg("p").c_str()),!pulsestate);
    //sendmsg("OK");
  });
  
}
void app_loop(){
}
