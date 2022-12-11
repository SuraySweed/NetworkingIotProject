#include "painlessMesh.h"

#define   MESH_PREFIX     "Mesh_username"
#define   MESH_PASSWORD   "mesh_password"
#define   MESH_PORT       5555

Scheduler userScheduler; 
painlessMesh mesh;

void sendmsg() ;

Task taskSendmsg( TASK_SECOND * 1 , TASK_FOREVER, &sendmsg );

void sendmsg() {
  String msg = "This is a testing message from Node2  ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendmsg.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
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
  mesh.setDebugMsgTypes( ERROR | STARTUP );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendmsg );
  taskSendmsg.enable();
}

void loop() {
  mesh.update();
}