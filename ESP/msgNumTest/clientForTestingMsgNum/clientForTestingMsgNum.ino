#include <painlessMesh.h>

#define MESH_PREFIX "MyMeshNetwork"
#define MESH_PASSWORD "MyPassword"
#define MESH_PORT 5555
#define THERE_IS_NO_ROOT -1

Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
  String ack = "thank you master!";
  mesh.sendBroadcast(ack);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New connection, nodeId = %u\n", nodeId);
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | MESH_STATUS  );

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
}

void loop() {
  mesh.update();

  // Send number of messages of size 10000 to the master node
  String msg;
  for (uint16_t i = 0; i < 1000; i++) {
    msg += "X"; // Add character 'X' to the message
  }
  static uint32_t messageCount = 0;
  uint32_t masterId;
  uint32_t numOfMessages = 100;
  if (mesh.getNodeList().size() > 0 && messageCount < numOfMessages) {
    String message = "Iteration 1, msg number " + String(messageCount) + ", " + msg;
    mesh.sendBroadcast(message);
    //mesh.sendSingle(masterId, message);
    Serial.printf("Iteration 1, sending msg number: %u\n", messageCount);
    messageCount++;
    delay(10);
  }
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
