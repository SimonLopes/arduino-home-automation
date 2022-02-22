
// bibliotecas -----------------------------------------------------
#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <Hash.h>

// Define ========================================================

#define APP_CONFIG  "/config.json"

// conf ==========================================================
ESP8266WiFiMulti WiFiMulti;
#define USE_SERIAL Serial
SocketIOclient socketIO;
ESP8266WebServer server(80);

// Constantes e variaveis ========================================

unsigned long messageTimestamp = 0;
const uint32_t connectTimeoutMs = 5000;
unsigned long tempoAnterior = 0;
unsigned long millisTarefa1 = 0;
unsigned long programationSinalize = millis();
int aState = 0;
int count = 0;
int clickCount = 0;
int seconds = 0;
bool programationMode = false;

 //Dados Wifi
String ssid;
String pass;

 //Dados Acess Point
String ssidAP     = "Homefuture";
String passwordAP = "123456789";
byte DNS_PORT = 53;
DNSServer dnsServer;

 //Dados Socket
String DOMAIN = "homefuture.herokuapp.com";
int PORT = 443;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<html>
<head>
  <title>CONFIGURAR</title>
  <script>
  function onlyNumber(evt) {
    var theEvent = evt || window.event;
    if (theEvent.type === 'paste') {
        key = event.clipboardData.getData('text/plain');
    } else {
        var key = theEvent.keyCode || theEvent.which;
        key = String.fromCharCode(key);
    }
    var regex = /[0-9]|\./;
    if( !regex.test(key) ) {
      theEvent.returnValue = false;
      if(theEvent.preventDefault) theEvent.preventDefault();
    }
  }
  function validateForm() {
    var ssid_wifi = document.forms["form_config"]["SSID_wifi"].value;
    var pass_wifi = document.forms["form_config"]["pass_wifi"].value;
    var domain = document.forms["form_config"]["domain"].value;
    var port_domain = document.forms["form_config"]["port_domain"].value;
    var ssid_ap = document.forms["form_config"]["SSID_AP"].value;
    var pass_ap = document.forms["form_config"]["pass_AP"].value;
    
    var status = document.getElementById("statusDiv");
    if (ssid_wifi == "" || pass_wifi == "") {
      status.innerHTML = "<p id='status' style='color:red;'>Preencha os campos obrigatorios.</p>";
      return false;
    }
    else {
      status.innerHTML = "<p id='status' style='color:green;'>Enviando...</p>";
      return true;
    }
  }
  </script>
</head>
<body>
<h1><strong><span style="font-family:arial,helvetica,sans-serif;">Configurar Aparelho</span></strong></h1>
<form name="form_config" action="/settings" method="post">
  <p style="margin-left: 40px;">​Nome wifi (SSID)*</p>
  <p style="margin-left: 40px;"><input maxlength="50" name="SSID_wifi" size="150" type="text" style="padding: 15px 32px;font-size: 20px;" /></p>
  <p style="margin-left: 40px;">Senha wifi (PASS)*</p>
  <p style="margin-left: 40px;"><input maxlength="50" name="pass_wifi" size="150" type="password" minlength="8" style="padding: 15px 32px;font-size: 20px;" /></p>
  <p style="margin-left: 40px;">Dominio Servidor (ESCOLHIDO NA COMPRA) (DEIXE EM BRANCO PARA MANTER)</p>
  <p style="margin-left: 40px;"><input maxlength="300" name="domain" size="150" type="text" style="padding: 15px 32px;font-size: 20px;" /></p>
  <p style="margin-left: 40px;">PORTA SERVIDOR (443) (DEIXE EM BRANCO PARA MANTER)</p>
  <p style="margin-left: 40px;"><input maxlength="4" name="port_domain" size="150" type="text" onkeypress="onlyNumber(event)" style="padding: 15px 32px;font-size: 20px;" /></p>
  <p style="margin-left: 40px;">Nome ponto de acesso (DEIXE EM BRANCO PARA MANTER)</p>
  <p style="margin-left: 40px;"><input maxlength="50" name="SSID_AP" size="150" type="text" style="padding: 15px 32px;font-size: 20px;" /></p>
  <p style="margin-left: 40px;">Senha ponto de acesso (DEIXE EM BRANCO PARA MANTER)</p>
  <p style="margin-left: 40px;"><input maxlength="50" name="pass_AP" size="150" type="password" minlength="8" style="padding: 15px 32px;font-size: 20px;" /></p>
  <p style="margin-left: 40px;"><input style="background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px;" name="submit" type="submit" value="Salvar" /></p>
</form>
<div id="statusDiv"></div>
</body>
</html>
)rawliteral";


// eventos socket
void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            USE_SERIAL.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
        
            USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);
            socketIO.send(sIOtype_CONNECT, "/");
            
            break;
        case sIOtype_EVENT:
            {
              
              USE_SERIAL.printf("[IOc] get event: %s id: %d\n", payload);
              
              DynamicJsonDocument doc(1024);
              DeserializationError error = deserializeJson(doc, payload, length);
              
              if(error) {
                  USE_SERIAL.print(F("deserializeJson() failed: "));
                  USE_SERIAL.println(error.c_str());
                  return;
              }

              // Leitura e identificacao de acoes HOMEFUTURE ===========================
                  
              String eventName = doc[0];
              String eventData = doc[1];

                //recebe acao
              if(eventName == "home"){
                  DynamicJsonDocument doc(1024);
                  DeserializationError error = deserializeJson(doc, eventData);
                  if(error) {
                    USE_SERIAL.print(F("deserializeJson() failed: "));
                    USE_SERIAL.println(error.c_str());
                    return;
                  }
                  
                    // variaveis recebidas
                  String eventName = doc["name"];
                  String pinString = doc["pin"];
                  String modePin = doc["pinMode"];
                  String timeExecString = doc["timeExec"];
                  String typeEvent = doc["typeEvent"];

                   // acoes 
                  executeHomeAcao(eventName,pinString,modePin,timeExecString,typeEvent);


                  // autenticar
               }else if(eventName == "verify"){
                    DynamicJsonDocument doc(1024);
                    DeserializationError error = deserializeJson(doc, eventData);
                    if(error) {
                      USE_SERIAL.print(F("deserializeJson() failed: "));
                      USE_SERIAL.println(error.c_str());
                      return;
                    }
                
                    String responsePin = doc["responsePinNum"];
                    String responseType = doc["responseType"];
                    int responsePinNum = responsePin.toInt();

                    if(responseType == "SWITCH"){
                      pinMode(responsePinNum, INPUT_PULLUP);
                      bool statusVal;
                      aState = digitalRead(responsePinNum);

                      if(aState == LOW){
                        statusVal = false;
                      }else {
                        statusVal = true;
                      }
                      sendStatus(statusVal);

                    }else if(responseType == "PULSE"){
                      pinMode(responsePinNum, INPUT);
                      bool statusVal;
                      aState = digitalRead(responsePinNum);

                      if(aState == LOW){
                        statusVal = false;
                      }else {
                        statusVal = true;
                      }
                      sendStatus(statusVal);
                    }
                    
                    
               }
              
            }
            break;
        case sIOtype_ACK:
            USE_SERIAL.printf("[IOc] get ack: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_ERROR:
            USE_SERIAL.printf("[IOc] get error: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_EVENT:
            USE_SERIAL.printf("[IOc] get binary: %u\n", length);
            hexdump(payload, length);
            break;
        case sIOtype_BINARY_ACK:
            USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
            hexdump(payload, length);
            break;
    }
}

void setup() {
  
    USE_SERIAL.begin(115200);
    
    pinMode(0, HIGH);
    
    pinMode(2, OUTPUT); 
    digitalWrite(2, HIGH);
    pinMode(0, INPUT_PULLUP);
    
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
      USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
      USE_SERIAL.flush();
      delay(1000);
    }
    openConfig();
    connectWiFi();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  if(programationMode == false) {
    digitalWrite(2, HIGH);
    socketIO.loop();

    /// entra no modo programacao depois de 10 segundos com o botao pressionado
    if(digitalRead(0) == LOW){
      delay(1000);
      count++;    
      if(count == 10){
        programationMode = true;
        
        openProgramationMode();
        
        digitalWrite(2, LOW);
        delay(500);
        digitalWrite(2, HIGH);
      }
    } else {
      count = 0;
    }
  }

    /// conta clicks para sair do modo programacao
    if (programationMode == true && digitalRead(0) == LOW ){ // verifica se o botão direito foi acionado
      clickCount ++;
      seconds=0;
      while (digitalRead(0) == 0) {} // loop vazio - aguarda soltar o botão (evita a númeração constante
    }


    /// reseta os clicks dps de 4 segundos sem clicar
    if(programationMode == true && digitalRead(0) == HIGH){
      if ((millis() - tempoAnterior) >= 1000) {
        tempoAnterior = millis();
        seconds++;
        if(seconds >= 5){seconds=0;}
      }
      if(seconds >= 4){
        clickCount = 0;
      }
    }

    /// sinaliza que esta em modo porgramacao
    if(programationMode == true){
      if((millis() - programationSinalize) < 500){
        digitalWrite(2, HIGH);
      }else{
        digitalWrite(2, LOW);
      }
      if((millis() - programationSinalize) > 1000){
        programationSinalize = millis();
      }
    }

    /// desativa o modo programacao
    if(programationMode == true && clickCount == 5){
      clickCount = 0;
      Serial.println(clickCount);
      closeProgramationMode();
      digitalWrite(2, !digitalRead(2));
      delay(100);
      digitalWrite(2, !digitalRead(2));
      delay(100);
      digitalWrite(2, !digitalRead(2));
      delay(100);
      digitalWrite(2, !digitalRead(2));
      delay(100);
      digitalWrite(2, !digitalRead(2));
    }
    
    
}

void executeHomeAcao(String eventName,String pinString,String modePin,String timeExecString,String typeEvent){
    int pin = pinString.toInt();
    int timeExec = timeExecString.toInt();

    if(modePin == "OUTPUT"){
      pinMode(pin, OUTPUT);
      
      if(typeEvent == "PULSE"){
        
        digitalWrite(pin, LOW);
        delay(timeExec);
        digitalWrite(pin, HIGH);
          
      }else if(typeEvent == "SWITCH"){
        digitalWrite(pin, !digitalRead(pin));
      }
                      
    }else if(modePin == "INPUT"){
    }else if (modePin == "INPUT_PULLUP"){
    }
}

void sendStatus(bool Status){
     DynamicJsonDocument doc(1024);
     JsonArray array = doc.to<JsonArray>();
     array.add("status");
     JsonObject param1 = array.createNestedObject();
     param1["status"] = Status;
     String output;
     serializeJson(doc, output);
                             
     socketIO.sendEVENT(output);
}


void openProgramationMode() {
  createAP();
}

void closeProgramationMode() {
  WiFi.softAPdisconnect(true);
  programationMode = false;
  openConfig();
  connectWiFi();
}

void createAP(){
  
  WiFi.softAP(ssidAP.c_str(), passwordAP.c_str());
  
  delay(100);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  server.onNotFound([]() {
    server.send(200, "text/html", index_html);    
  });
  server.on("/settings", receiveSettings);
  
  server.begin();
  Serial.print("HTTP server started");
}

void receiveSettings(){
  String postBody = server.arg("plain");
  Serial.println(postBody);
  
  String ssid_wifi = server.arg("SSID_wifi");
  String pass_wifi = server.arg("pass_wifi");
  String domain = server.arg("domain");
  int port_domain = server.arg("port_domain").toInt();
  String ssid_ap = server.arg("SSID_AP");
  String pass_ap = server.arg("pass_AP");
  
  
  saveSettings(ssid_wifi, pass_wifi, domain, port_domain, ssid_ap, pass_ap);
}

void saveSettings(String ssid_wifi,String pass_wifi,String domain,int port_domain,String ssid_ap,String pass_ap){

  SPIFFS.begin();
  DynamicJsonDocument jsonBuffer(1024);
  File configFile = SPIFFS.open("/config.json", "w+");

  if(!configFile){
    Serial.println("Erro ao abrir arquivo!");
  }
 
  JsonObject configJson = jsonBuffer.to<JsonObject>();

  configJson["ssid_wifi"] = ssid_wifi;
  configJson["pass_wifi"] = pass_wifi;
  configJson["domain"] = (domain == "") ? DOMAIN : domain;
  configJson["port_domain"] = (!port_domain) ? PORT : port_domain;
  configJson["ssid_ap"] = (ssid_ap == "") ? ssidAP : ssid_ap;
  configJson["pass_ap"] = (pass_ap == "") ? passwordAP : pass_ap;

  serializeJson(configJson, configFile);

  configFile.close();
  
  closeProgramationMode();
  
}

void openConfig(){
  SPIFFS.begin();
  if (!SPIFFS.exists("/config.json")) {
    Serial.println("Failed to open file config");
    saveSettings("empty", "empty", "empty.herokuapp.com", 443, "HomeFutureAP", "12345678");
  }
    File configFile = SPIFFS.open("/config.json", "r");  
    if(!configFile){
      Serial.println("Failed read Document");
    }
    DynamicJsonDocument configFileJson(1024);
    DeserializationError err = deserializeJson(configFileJson, configFile);
    if(err) {
      Serial.println("Failed create jsonDocument");
      return;
    }
    Serial.println("++++++++++++++++++++++++++++++");
    String ssid_wifi = configFileJson["ssid_wifi"];
    String pass_wifi = configFileJson["pass_wifi"];
    String domain = configFileJson["domain"];
    int port_domain = configFileJson["port_domain"];
    String ssid_ap = configFileJson["ssid_ap"];
    String pass_ap = configFileJson["pass_ap"];

    ssid = ssid_wifi;
    pass = pass_wifi;
    ssidAP     = ssid_ap;
    passwordAP = pass_ap;
    DOMAIN = domain;
    PORT = port_domain;
    
    Serial.println(ssid);
    serializeJson(configFileJson, Serial);
    configFile.close();
  
 
  
}

void connectWiFi()
{
  //WiFi.mode(WIFI_STA);
  if(ssid == "" || pass == ""){
    programationMode = true;
    openProgramationMode();
    return;
  }
  WiFiMulti.addAP(ssid.c_str(), pass.c_str());
  USE_SERIAL.println(WiFi.status());
  while (WiFiMulti.run(connectTimeoutMs) != WL_CONNECTED)
  {
    USE_SERIAL.print(".");
    delay(1000);
  }

  USE_SERIAL.print("");
  USE_SERIAL.println("WiFi connected");
  USE_SERIAL.print("IP Address : ");
  USE_SERIAL.println(WiFi.localIP());

  socketIO.beginSSL(DOMAIN, PORT, "/socket.io/?EIO=4");
  socketIO.onEvent(socketIOEvent);
}
