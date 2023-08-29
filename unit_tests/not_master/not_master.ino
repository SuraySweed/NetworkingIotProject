#include "painlessMesh.h"

#define   MESH_PREFIX     "Mesh_username"
#define   MESH_PASSWORD   "mesh_password"
#define   MESH_PORT       5555

Scheduler userScheduler; 
painlessMesh mesh;

void sendmsg() ;

Task taskSendmsg( TASK_SECOND * 1 , 3, &sendmsg );

bool isRecievedMsg = false;
//uint32_t dest_id = 0;

void sendmsg() {
  String msg = "hey all i recieved a msg but im not the root, my id is: ";
  msg += mesh.getNodeId();

  mesh.sendBroadcast(msg);
  Serial.printf("send successful\n");

  
  
  taskSendmsg.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u: a message that contain: %s\n", from, msg.c_str());
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
  mesh.setDebugMsgTypes( ERROR | STARTUP );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  Serial.printf("I am root?: %d \n", mesh.isRoot());
  userScheduler.addTask( taskSendmsg );
  //taskSendmsg.enable();
}

void loop() {
  mesh.update();
}