#ifndef REST_API_H_   /* Include guard */
#define REST_API_H_

#include <ESP8266WebServer.h>
#include "SystemLogic.h"

#define INDEX_HTML "Podlewaczka 2"

class RestApi {
  private:
    ESP8266WebServer* pServer;
    SystemLogic* pSystemLogic;
    SysConfig* pConfig;
    
    void handleRoot()
    {
      pServer->send_P(200, "text/html", INDEX_HTML);
    }

    void handleApi() {
      int channelNumber = getArg("ch");
      int channelValue = getArg("val");
      String command = getArgString("command");
      String commandVal = getArgString("arg");
      if (channelNumber != -1 && channelValue != -1) {
        pSystemLogic->setChannel(channelNumber, channelValue);
      }
      boolean success = true;
      if (command != "") {
        if(command == "startAuto") {
          pSystemLogic->startAuto();
        }
        if(command == "activate") {
          pSystemLogic->onActivate();
        }
        if(command == "deactivate") {
          pSystemLogic->onDeactivate();
        }
        if(command == "click") {
         pSystemLogic->onButtonPress(); 
        }
        if (command == "off") {
          pSystemLogic->takeOff();
        }
        if(command == "config" && commandVal != "") {
          success = pConfig->setConfig(commandVal);
        }
      }
      if (success) {
        pServer->send(200, "text/plain", "ok");  
      } else {
        pServer->send(500, "text/plain", "error");  
      }
    }

    String handleState() {
      pServer->send(200, "application/json", pSystemLogic->getStatus());
    }

    int getArg(String aName) {
      for (int i=0; i<pServer->args(); i++) {
        String a = pServer->argName(i);
        if (aName == a) {
          return pServer->arg(i).toInt();
        }
      }
      return -1;
    }

    String getArgString(String aName) {
      for (int i=0; i<pServer->args(); i++) {
        String a = pServer->argName(i);
        if (a == aName) {
          return pServer->arg(i);
        }
      }
      return "";
    }
    
    void handleNotFound() {
      String message = "File Not Found\n\n";
      message += "URI: ";
      message += pServer->uri();
      message += "\nMethod: ";
      message += (pServer->method() == HTTP_GET)?"GET":"POST";
      message += "\nArguments: ";
      message += pServer->args();
      message += "\n";
      for (uint8_t i=0; i<pServer->args(); i++) {
        message += " " + pServer->argName(i) + ": " + pServer->arg(i) + "\n";
      }
      pServer->send(404, "text/plain", message);
    }
  
  public:
    RestApi(ESP8266WebServer* pServer, SystemLogic* pSystemLogic, SysConfig* pConfig) {
      this->pServer = pServer;
      this->pSystemLogic = pSystemLogic;
      this->pConfig = pConfig;
    }

    void bind() {
      pServer->on("/", std::bind(&RestApi::handleRoot, this));
      pServer->onNotFound(std::bind(&RestApi::handleNotFound, this));
      pServer->on("/api", std::bind(&RestApi::handleApi, this));
      pServer->on("/state", std::bind(&RestApi::handleState, this));
      pServer->begin();
    }

  
};

#endif // REST_API_H_
