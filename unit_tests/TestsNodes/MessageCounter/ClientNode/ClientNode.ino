#include <painlessMesh.h>

#define MESH_PREFIX "MyMeshNetwork"
#define MESH_PASSWORD "MyPassword"
#define MESH_PORT 5555
#define THERE_IS_NO_ROOT -1

Scheduler userScheduler;
painlessMesh mesh;

static uint8_t iterationNumber = 0;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
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

  // Send 10 messages to the master node
  static uint8_t messageCount = 0;
  uint32_t masterId;
  if (mesh.getNodeList().size() > 0 && messageCount < 255) {
    String message = "Hello from node num1, msg #" + String(messageCount) + ", iteration #" + String(iterationNumber);
    mesh.sendBroadcast(message);
    //mesh.sendSingle(masterId, message);
    Serial.printf("sending msg: %s\n", message.c_str());
    messageCount++;
    delay(10);

    if (messageCount == 254) {
      messageCount = 0;
      iterationNumber++;
      delay(30000);
    }
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