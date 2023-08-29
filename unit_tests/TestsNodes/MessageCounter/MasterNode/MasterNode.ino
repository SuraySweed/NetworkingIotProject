#include "painlessMesh.h"
#include <HTTPClient.h>
#include <iostream>
#include <string.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <vector>

#define MESH_PREFIX "MyMeshNetwork"
#define MESH_PASSWORD "MyPassword"
#define MESH_PORT 5555

const char* ssid = "Suray Sweed";
const char* password = "12345678";

//const char* ssid = "Sireen19";
//const char* password = "25071966";
const char* serverUrl = "http://172.20.10.13:3000/receive-data"; // Replace with your server URL

std::vector<String> msgVector; // Vector to store received messages

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
  msgVector.push_back(msg); 
  if (msgVector.size() == 255) {
    // Print all received messages
    Serial.println("sending messages to server:\n");
    sendMsgToServer(from);
  }
  //sendMsgToServer(msg, from);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New connection, nodeId = %u\n", nodeId);
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | MESH_STATUS );

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  
  mesh.onNewConnection(&newConnectionCallback);
  // Check if all messages have been received

}

void loop() {
  mesh.update();
}


void printReceivedMessages() {
  Serial.println("in print Received Messages:");
  for (const auto& msg : msgVector) {
    Serial.println(msg);
  }
}


void sendMsgToServer( uint32_t nodeId)
{
  //while(!taskSendmsg.enable());
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    DynamicJsonDocument data(128);
    data["node_id"] = String(nodeId);
    
    for (auto& msgToServer : msgVector) {
      data["msg_data"] = msgToServer.c_str();
      String dataStr;
      serializeJson(data, dataStr);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(dataStr);
      while (httpResponseCode <= 0)
      {
        httpResponseCode = http.POST(dataStr);
        Serial.println("Error sending message");
      }
      Serial.print("HTTP response code: ");
      Serial.println(httpResponseCode);
    }
    msgVector.clear();
    http.end();
  }
}