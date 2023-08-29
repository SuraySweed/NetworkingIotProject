#include <painlessMesh.h>
#include <HTTPClient.h>
#include "esp_camera.h" // Include the ESP32-CAM camera library
#include <base64.h> // Assuming you have a base64 library available
#include <Arduino.h> // Include the Arduino core library

#define MESH_PREFIX "MyMeshNetwork"
#define MESH_PASSWORD "MyPassword"
#define MESH_PORT 5555

const char* ssid = "Galaxy Note2073f7";
const char* password = "peae8479";
//const char* ssid = "sawid_2.4";
//const char* password = "wara4312671";
const char* serverUrl = "http://192.168.93.214:3000/upload-image"; // Replace with your server URL
bool hello_sended = false;
Scheduler userScheduler;
painlessMesh mesh;
camera_fb_t *receivedImage = NULL;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from %u: %s\n", from, msg.c_str());
  
  // Check if the received message is an image message
  if (msg.startsWith("IMAGE|")) {
    Serial.println("process image at master");
    processCSVImageData(from, msg.substring(6).c_str()); // Extract image data
  }
  else Serial.println("msg isnt an image");
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

// Function to process received image data
/*
void processReceivedImage(uint32_t from, String imageDataStr) {
  Serial.printf("Received image of size %u data from node %s\n", imageDataStr.length(),  String(from).c_str());

  // Split the comma-separated values and convert them to integers
  int imageSize = 0;
  String pixelValueStr;
  uint8_t pixelValue;
  receivedImage->buf = (uint8_t *) malloc(20);
  Serial.println("1");
  for (size_t i = 0; i < imageDataStr.length(); i++) {
    char c = imageDataStr.charAt(i);
    Serial.println("2");
    if (c == ',') {
      Serial.println("3/c");
      pixelValue = pixelValueStr.toInt();
      Serial.println("4 toint");
      receivedImage->buf[0] = pixelValue;
      Serial.println("5 buf");
      pixelValueStr = "";
    } else {
      Serial.println("6 not ,");
      pixelValueStr += c;
    }
  }
  
  Serial.println("**************************************");
  Serial.println("receivedImage = " + receivedImage->buf[0]);
  // Display or process the received image data as needed
  // For example, you can display the image using TFT displays or other methods
  sendImageToServer(receivedImage);

  // Release the image buffer
  if (receivedImage) {
    esp_camera_fb_return(receivedImage);
    receivedImage = NULL;
  }
}
*/

camera_fb_t* createCameraFrameFromCSV(const char* csvData) {
  // Assuming each value is between 0 and 255, and there are no spaces
  Serial.println("1 at createCameraFrameFromCSV");
  const char* delimiter = ",";
  
  // Allocate memory for the camera frame
  size_t bufferSize = 1500; // Assuming RGB image with 640x480 resolution
  camera_fb_t* frame = (camera_fb_t*)malloc(sizeof(camera_fb_t));
  Serial.println("2 after frame malloc");
  frame->buf = (uint8_t*)malloc(bufferSize);
  if(frame->buf != NULL) Serial.println("malloc failed for frame buf");
  Serial.println("3 after buf malloc");
  // Parse CSV data and populate pixel values
  char* token = strtok((char*)csvData, delimiter);
  Serial.println("4 after strtok");
  int pixelIndex = 0;
  while (token != NULL) {
    //Serial.println("5 in while");
    //Serial.printf("frame->buf len = %u\n", frame->len);
    if (frame != NULL) frame->buf[pixelIndex] = (uint8_t)atoi(token); // Convert CSV value to integer
    pixelIndex++;
    //Serial.println("6 buf++");
    token = strtok(NULL, delimiter);
  }
  Serial.println("7 after while");
  // Set relevant properties of the camera frame
  frame->len = bufferSize;
  Serial.println("8 frame len");
  frame->width = 30;
  Serial.println("9 frame width");
  frame->height = 50;
  Serial.println("10 frame hiegh");
  frame->format = PIXFORMAT_RGB888;
  Serial.println("11 frame format");
  return frame;
}

void processCSVImageData(uint32_t from, const char* csvData) {
  receivedImage = createCameraFrameFromCSV(csvData);
  Serial.println("12 return from create");
  if (receivedImage) {
    // Now you can use the receivedImage
    // Don't forget to free memory when done
    sendImageToServer(receivedImage);
    free(receivedImage->buf);
    free(receivedImage);
  } else {
    // Handle the case where creating a camera frame failed
  }
}

void sendImageToServer(camera_fb_t *image) {
  Serial.println("13 in send image to server");
  if (!image) {
    Serial.println("Invalid image data");
    return;
  }

  //String base64Image = base64::encode(image->buf, image->len);
  
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

    // Create a message with the image data
    String message;
    Serial.printf("image len = %u\n", image->len);
    for (size_t i = 0; i < image->len; i++) {
      message += String(image->buf[i]);
      if (i < image->len - 1) {
        message += ",";
      }
    }

    // Set the content type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send the image data to the server
    Serial.println("message to server is " + message);
    //String encodedString = base64::encode(image->buf, image->len);
    //String encodedString = base64_encode((const uint8_t*)message.c_str(), message.length());
    int httpResponseCode = http.POST("image=" + message);

    if (httpResponseCode > 0) {
      Serial.print("Image sent to server. HTTP response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Error sending image to server");
    }

    http.end();
  }

}

