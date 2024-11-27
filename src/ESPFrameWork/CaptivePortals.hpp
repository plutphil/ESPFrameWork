void setportals(){
  server.on("/generate_204", []() {
    logdbg("Android captive portal");
    
    server.sendHeader("Location", "/");
    
    server.send(200, "text/plain", "HI Andro");
  });  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", []() {

    logdbg("Microsoft captive portal");
    server.sendHeader("Location", "/");
    server.send(200, "text/plain", "HI MS");

  });        // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/success.txt",[](){
    logdbg("Firefox");
    server.sendHeader("Location", "/");
    server.send(200, "text/plain", "success");
  });
  server.on("/canonical.html",[](){
    logdbg("Firefox2");
    server.sendHeader("Location", "/");
    server.send(200, "text/html", "success");
  });
}
