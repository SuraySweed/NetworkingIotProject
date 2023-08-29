#include <painlessMesh.h>
#include <HTTPClient.h>

#define MESH_PREFIX "MyMeshNetwork"
#define MESH_PASSWORD "MyPassword"
#define MESH_PORT 5555

const char* ssid = "Galaxy Note2073f7";
const char* password = "peae8479";
//const char* ssid = "sawid_2.4";
//const char* password = "wara4312671";
const char* serverUrl = "http://192.168.93.214:3000/receive-data"; // Replace with your server URL
bool hello_sended = false;
Scheduler userScheduler;
painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());

  // Send the message to the server immediately upon receiving it
  Serial.println("sending warning to server");
  sendMsgToServer(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New connection, nodeId = %u\n", nodeId);
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | MESH_STATUS);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

}

void loop() {
  mesh.update();
}

void sendMsgToServer(uint32_t nodeId, const String& msg) {
  
  WiFi.begin(ssid, password);
  do {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    //mesh.update();
  } while (WiFi.status() != WL_CONNECTED) ;
  
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to WiFi");
    HTTPClient http;
    http.begin(serverUrl);

    // Create a JSON payload with node ID and message data
    DynamicJsonDocument data(128);
    data["node_id"] = String(nodeId);
    data["msg_data"] = msg;

    String dataStr;
    serializeJson(data, dataStr);

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(dataStr);

    if (httpResponseCode > 0) {
      Serial.print("HTTP response code: ");
      Serial.println(httpResponseCode);
      hello_sended = true;
    } else {
      Serial.println("Error sending message to server");
    }

    http.end();
    //WiFi.disconnect();
    //Serial.println("Wi-Fi disconnected");
  }

}
