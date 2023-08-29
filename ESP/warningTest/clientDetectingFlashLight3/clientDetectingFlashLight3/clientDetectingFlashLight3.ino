#include <painlessMesh.h>
#include "esp_camera.h" // Include the ESP32-CAM camera library


// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM

#include "camera_pins.h"


#define MESH_PREFIX "MyMeshNetwork"
#define MESH_PASSWORD "MyPassword"
#define MESH_PORT 5555
#define THERE_IS_NO_ROOT -1
#define FLASH_THRESHOLD 2800000
#define FLASH_DIFF 500000

Scheduler userScheduler;
painlessMesh mesh;
camera_fb_t *fb = NULL;
int oldDiff = 0;
uint32_t masterNodeID = 0;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
  //mesh.sendBroadcast(msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New connection, nodeId = %u\n", nodeId);
}

void sendMessage(const String& message) {
  mesh.sendBroadcast(message);
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | MESH_STATUS);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);

  // Camera setup
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera initialization failed with error 0x%x", err);
    return;
  }
  sendMessage("HELLO MASTER");
}

void loop() {
  mesh.update();

  // Capture a frame from the camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Process the camera frame to detect a flash-like event
  int diff = 0;
  
  for (int i = 0; i < fb->len; i += 2) {
    diff += abs((int)fb->buf[i] - (int)fb->buf[i + 2]);
  }

  Serial.printf("diff = %d\n", diff);
  Serial.printf("oldDiff = %d\n", oldDiff);
  // Add your logic here to trigger a warning message based on the flash detection
  bool triggerWarning = (diff - oldDiff > FLASH_DIFF); // Adjust FLASH_THRESHOLD as needed
  
  if (triggerWarning) {
    String message = "Warning! Flash detected! node 3 ";
    Serial.println("sending warning to master");
    sendMessage(message); // Send the warning message to all nodes in the network
    Serial.println(message);
  }
  oldDiff = diff; 
  esp_camera_fb_return(fb); // Release the camera frame buffer

  delay(1500); // Delay between camera captures (adjust as needed)
}
