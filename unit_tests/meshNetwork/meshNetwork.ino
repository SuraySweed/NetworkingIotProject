#include "painlessMesh.h"
#include <list>

#define   MESH_PREFIX     "Mesh_username"
#define   MESH_PASSWORD   "mesh_password"
#define   MESH_PORT       5555
#define THERE_IS_NO_ROOT -1

Scheduler userScheduler; 
painlessMesh mesh;

void sendmsg() ;
uint32_t getMeshTreeRootID();

//Task taskSendmsg( TASK_SECOND * 1 , TASK_FOREVER, &sendmsg );
Task taskSendmsg( TASK_SECOND * 1 , 3, &sendmsg );


/*
uint32_t getMeshTreeRootID() {
  if (mesh.asNodeTree().root) {
    return mesh.asNodeTree().nodeId;
  }
  for (auto node : mesh.asNodeTree().subs) {
    if (node.root) {
      return node.nodeId;
    }
  }
  return THERE_IS_NO_ROOT;
}
*/

void sendmsg() {
  String msg = "hey, my name is suray sweed, Node: ";
  msg += mesh.getNodeId();
  uint32_t root_id = getMeshTreeRootID();
  //uint32_t root_id = mesh.getNodeList().back();
  mesh.sendSingle(root_id, msg);
  taskSendmsg.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u: a message that contain: %s\n", from, msg.c_str());
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

uint32_t getMeshTreeRootID() {
  //Serial.printf("is root? %d\n", mesh.isRoot());
  if (mesh.isRoot()) {
    return mesh.getNodeId();
  }
  //Serial.printf("is subs is empty? %d\n", mesh.subs.empty());

  if (!mesh.subs.empty()) {
    for (auto node : mesh.subs) {
      if (node.get()) {
        if (node.get()->root) {
          return node.get()->nodeId;
        }
      }
      return THERE_IS_NO_ROOT;
    }
  }
  return THERE_IS_NO_ROOT;
}