// root code

#include "painlessMesh.h"

#define   MESH_PREFIX     "Mesh_username"
#define   MESH_PASSWORD   "mesh_password"
#define   MESH_PORT       5555


//const char* ssid = "HOTBOX 4-4E78-5GHz";
//const char* password = "DV29QJWG3KN9";
const char* ssid = "Suray Sweed";
const char* password = "12345678";
//const char* host = "192.168.181.1";
//const char* host = WiFi.localIP().toString().c_str(); // pc ip
const uint16_t PORT = 8820; 
String message = "";

Scheduler userScheduler; 
painlessMesh mesh;

// functions declerations
void sendmsg() ;

Task taskSendmsg( TASK_SECOND * 1 , 3, &sendmsg);


void sendmsg() {
  String msg = "hey suray i recieved your msg couse im the root , my id is: ";
  msg += mesh.getNodeId();
  
  uint32_t dest_id = mesh.getNodeList().back();
  mesh.sendSingle(dest_id, msg);
  Serial.printf("send successful\n");
  
  taskSendmsg.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u: a message that contain: %s\n", from, msg.c_str());
  message = msg;
  taskSendmsg.enable();

}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  
  // mesh network part
  mesh.setDebugMsgTypes( ERROR | STARTUP );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // set a root
  Serial.printf("I am root?: %d \n", mesh.isRoot());
  mesh.setRoot();
  Serial.printf("I am root?: %d\n", mesh.isRoot());

  userScheduler.addTask( taskSendmsg );
  //taskSendmsg.enable();

}

void loop() {
  mesh.update();
}