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

//const char* ssid = "Suray Sweed";
//const char* password = "12345678";
const char* ssid = "Galaxy Note2073f7";
const char* password = "peae8479";
const char* serverUrl = "http://192.168.249.28:3000/receive-data"; // Replace with your server URL

std::vector<String> msgVector; // Vector to store received messages

Scheduler userScheduler;
painlessMesh mesh;

unsigned long lastSendTime = 0; // Last time messages were sent to the server
unsigned long sendInterval = 60000; // Send interval of 1 minute (in milliseconds)
uint32_t g_from;
int sendIntervalPassed = 0;

void receivedCallback(uint32_t from, String &msg) {
  if(sendIntervalPassed == 0) msg = msg.substring(0,msg.length()-1000);
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
  msgVector.push_back(msg); 
  g_from = from;
  if(sendIntervalPassed == 1 && msgVector.size() != 1)
  {
    Serial.printf("sending messages to server\n");
    sendMsgToServer(g_from);
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

  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= sendInterval && sendIntervalPassed == 0) {
    lastSendTime = currentTime;
    String myMsg = "master got the messages from mesh client!";
    Serial.println("after 30 sec of listening:)");
    mesh.sendBroadcast(myMsg);
    sendIntervalPassed = 1;
    //printReceivedMessages();
  }
  

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
    mesh.update();
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

    http.end();
    sendIntervalPassed = 0;
    msgVector.clear();
  }
}