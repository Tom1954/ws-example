#include <WiFi.h>
#include <WebSocketsServer.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "WebPage.h"

unsigned long timeF;

// SSID and password
const char* ssid = "Linksys";
const char* password = "Albatross25!";

// Stepper motor output pins
int runStepper = 22;
int directionStepper = 21;
int enableStepper = 17;

// Declear a structure for the initial state
struct InitialState {
  String button1;
  String button2;
  String button3;
  int sensitivity;
} state;

WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);


void setup() {
  Serial.begin(115200);

  //  IPAddress Ip(192, 168, 0, 180);
  //  IPAddress Gateway(192, 168, 0, 1);
  //  IPAddress Subnet(255, 255, 255, 0);
  //  WiFi.config(Ip, Gateway, Subnet);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  state.button1 = "on";
  state.button2 = "off";
  state.button3 = "on";
  state.sensitivity = 33;

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();





  pinMode(runStepper, OUTPUT);
  pinMode(directionStepper, OUTPUT);
  pinMode(enableStepper, OUTPUT);

  digitalWrite(runStepper, LOW);
  digitalWrite(directionStepper, LOW);
  digitalWrite(enableStepper, LOW);
}

void loop() {

  webSocket.loop();
  server.handleClient();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  IPAddress ip;
  switch (type) {
    case WStype_ERROR:
      Serial.printf("Error");
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u]Disconnected!\n", num);
      delay(200);
      break;
    case WStype_BIN:
      Serial.printf("bin");
      break;
    case WStype_FRAGMENT_TEXT_START:
      Serial.printf("FRAGMENT_TEXT_START");
      break;
    case WStype_FRAGMENT_BIN_START:
      Serial.printf("FRAGMENT_BIN_START");
      break;
    case WStype_FRAGMENT:
      break;
    case WStype_FRAGMENT_FIN:
      break;
    case WStype_CONNECTED:
      ip = webSocket.remoteIP(num);
      Serial.printf("[%u]Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      break;
    case WStype_TEXT:
      String payloadStr = String((char*) payload);
      //    Serial.println(payloadStr);

      if (payloadStr.indexOf("button1") != -1) {
        if (payloadStr.indexOf("on") != -1) {
          state.button1 = "on";

          DynamicJsonBuffer jBuffer;
          JsonObject& root = jBuffer.createObject();

          root["type"] = "button1";
          root["value"] = "on";
          String databufon;
          root.printTo(databufon);
          webSocket.broadcastTXT(databufon.c_str(), databufon.length());

          runProgram1();

          root["value"] = "off";
          String databufoff;
          root.printTo(databufoff);
          webSocket.broadcastTXT(databufoff.c_str(), databufoff.length());

        } else {
          Serial.println("off button 2");
        }
      }

      if (payloadStr.indexOf("button2") != -1) {
        Serial.println("Yes button 2");
      }

      if (payloadStr.indexOf("button3") != -1) {
        Serial.println("Yes button 3");
      }

      if (payloadStr.indexOf("reset") != -1) {
        Serial.println("Yes reset");
      }

      if (payloadStr.indexOf("sensitivity") != -1) {
        Serial.println("Yes sensitivity");
      }
      break;
  }
}

void runProgram1() {
  timeF = millis();
  Serial.println(timeF);
  int delayValue = 100;
  Serial.println("Begin runFunction");
  delay(1000);
  digitalWrite(directionStepper, HIGH);
  for (int i = 0; i <= 40; i++) {
    timeF = millis();
    Serial.println(timeF);
    delayValue = delayValue - 2;
    Serial.println(i);
    Serial.println(delayValue);
    digitalWrite(runStepper, HIGH);
    delay(delayValue);
    digitalWrite(runStepper, LOW);
    delay(delayValue);
  }
  for (int i = 0; i <= 120; i++) {
    Serial.println(i);
    Serial.println(delayValue);
    digitalWrite(runStepper, HIGH);
    delay(delayValue);
    digitalWrite(runStepper, LOW);
    delay(delayValue);
  }
  for (int i = 0; i <= 40; i++) {
    delayValue = delayValue + 2;
    Serial.println(i);
    Serial.println(delayValue);
    digitalWrite(runStepper, HIGH);
    delay(delayValue);
    digitalWrite(runStepper, LOW);
    delay(delayValue);
  }
  Serial.println("Finished runFunction");
  timeF = millis();
  Serial.println(timeF);
}

void backFunction() {
  digitalWrite(directionStepper, LOW);
  for (int i = 0; i > 2; i++) {
    digitalWrite(runStepper, HIGH);
    delay(10);
    digitalWrite(runStepper, LOW);
    delay(10);
  }
}

void fwdFunction() {
  digitalWrite(directionStepper, HIGH);
  for (int i = 0; i > 2; i++) {
    digitalWrite(runStepper, HIGH);
    delay(10);
    digitalWrite(runStepper, LOW);
    delay(10);
  }
}

/*


  // Function which will update the `state` variable given an action.
  // Example action:
  //  { type: "button1", value "off" };
  // will set `state.button1` to "off".
  void updateState(action) {
    switch (action.type) {
        // Example: { type: "button1", value: "on" }
        case "button1": {
            state.button1 = action.value;
            break;
        }
        // Example: { type: "button2", value: "off" }
        case "button2": {
            state.button2 = action.value;
            break;
        }
        // Example: { type: "button2", value: "off" }
        case "button3": {
            state.button2 = action.value;
            break;
        // Example: { type: "sensitivity", value: 10 }
        case "sensitivity": {
            state.sensitivity = action.value;
            break;
        }
        // Example: { type: "reset"}
        case "reset": {
            state = Object.assign({}, INITIAL_STATE);
            break;
        }
    }
  }

  // This will send the given state to all the clients.
  void broadcastState(state) {
    connections.forEach(connection => {
        // We "stringify" the current state as JSON, this ensures
        // we just send a string to the client.
        connection.send(JSON.stringify({
            type: "state",
            state: state
        }));
    })
  }

*/
