void replyServerError(String msg) {
  DBG_OUTPUT_PORT.println(msg);
  server.send(500, "text/plain", msg + "\r\n");
}
void setup_webftp(){
  server.on("/rm", []() {
    String path = "";
    if (server.hasArg("p")) {
      path = server.arg("p");
      if (path != "/" && LittleFS.exists(path)) {
        LittleFS.remove(path);
        server.sendHeader("Location", "/ls");
        server.send(200, "text/plain", "success");
        return;
      }
    }
    server.send(200, "text/plain", "fail");
  });
  server.on("/ls", []() {
    String path = "";
    if (server.hasArg("p")) {
      path = server.arg("p");
      if (path != "/" && !LittleFS.exists(path)) {
        path = "";
      }
    }

    Dir dir = LittleFS.openDir(path);

    File f = LittleFS.open(path, "r");
    if (f) {
      if (!server.chunkedResponseModeStart(200, "text/plain")) {
        server.send(505, F("text/html"), F("HTTP1.1 required"));
        return;
      }
      while (f.available()) {
        server.sendContent(f.readString());
      }
      f.close();
      server.chunkedResponseFinalize();
      return;
    }
    path.clear();
    if (!server.chunkedResponseModeStart(200, "text/html")) {
      server.send(505, F("text/html"), F("HTTP1.1 required"));
      return;
    }
    String output ="";

    FSInfo fs_info;
    LittleFS.info(fs_info);
    output+=String("<h3>")+fs_info.usedBytes+" / "+fs_info.totalBytes+" B  "+String(fs_info.usedBytes*100.f/fs_info.totalBytes)+" % </h3>";
   server.sendContent(output);
   while (dir.next()) {
      output = "<a href=\"?p=";
      output += dir.fileName();
      output += "\">";
      if (dir.isDirectory()) {
        output += "dir";
      } else {
        output += dir.fileSize();
      }
      output += " ";
      if (dir.fileName()[0] == '/') {
        output += &(dir.fileName()[1]);
      } else {
        output += dir.fileName();
      }
      output += "</a>";
      output += "<a href=\"rm?p=";
      output += dir.fileName();
      output += "\">";
      output += "RM";
      output += "</a>";
      output += "<br>";
      server.sendContent(output);
    }

    server.sendContent("<form enctype=\"multipart/form-data\" action=\"up\" method=\"post\"><input type=\"file\" name=\"f\"><input type=\"submit\"></form>");
    server.sendContent("UPLOAD");
    server.chunkedResponseFinalize();
  });  
  
  server.on("/up",  HTTP_POST, [](){
    server.send(200, "text/plain", "");
    }, []() {
    Serial.println("got here");
    HTTPUpload& upload = server.upload();
    
    Serial.println("got here2");
   if (upload.status == UPLOAD_FILE_START) {
      String filename = upload.filename;
      // Make sure paths always start with "/"
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      DBG_OUTPUT_PORT.println(String("handleFileUpload Name: ") + filename);
      uploadFile = LittleFS.open(filename, "w");
      if (!uploadFile) {
        return replyServerError(F("CREATE FAILED"));
      }
      DBG_OUTPUT_PORT.println(String("Upload: START, filename: ") + filename);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (uploadFile) {
        size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
        if (bytesWritten != upload.currentSize) {
          return replyServerError(F("WRITE FAILED"));
        }
      }
      DBG_OUTPUT_PORT.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      if (uploadFile) {
        uploadFile.close();
      }
      DBG_OUTPUT_PORT.println(String("Upload: END, Size: ") + upload.totalSize);
    }
  });
}
