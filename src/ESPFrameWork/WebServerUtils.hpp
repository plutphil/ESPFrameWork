#define TEXTHTML "text/html"
#define TEXTPLAIN "text/plain"
#define HTTP_OK 200
#define HTTP_NOT_FOUND 404 
#define HTTP_ERR 500
#define ERR_NO_PIN 88
#define ERR_NO_MODE 88

//void senderr(String msg){
  //server.send(202, "text/html", "<meta http-equiv=\"REFRESH\" content=\"0;url=/main\"><h1 style=\"color:red;\">"+msg+"</h1>");  
//}
/*void sendmsg(String msg){
  server.send(202, "text/html", "<meta http-equiv=\"REFRESH\" content=\"0;url=/main\"><h1 style=\"color:green;\">"+msg+"</h1>");  
}*/
void sendok(){
  server.send(HTTP_OK,TEXTPLAIN,"");
}
void sendmsg(String msg,int code=HTTP_OK){
  logdbg("msg "+msg);
  server.send(code,TEXTPLAIN,msg);
}
void senderr(int i=0){
  server.send(HTTP_ERR,TEXTPLAIN,"");
}
