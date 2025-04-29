/**
 * ESP32-S3 Mesh Sensor Node (ESP1)
 * Reads an analog value from an ADC pin and sends it over the ESP-MESH network.
 */

 #include <Arduino.h>
 #include "painlessMesh.h" // https://github.com/gmag11/painlessMesh
 
 // --- Configuration ---
 #define   MESH_PREFIX     "MyMeshNetwork" // Name of your mesh network
 #define   MESH_PASSWORD   "MySecurePassword" // Password for your mesh network
 #define   MESH_PORT       5555 // Default painlessMesh port
 
 // ADC Configuration
 const int adcPin = 4; // GPIO4 for ADC reading (Check ESP32-S3 pinout for ADC capable pins)
 // Note: ESP32-S3 ADC pins might vary. Common ones are GPIO 1-10.
 // Adjust ADC attenuation if needed for your voltage range (e.g., 0-3.3V)
 // #include <driver/adc.h> // Required if using adc1_config_width or adc1_config_channel_atten
 // Default attenuation usually covers 0-1.1V. For 0-3.3V use ADC_ATTEN_DB_11
 
 // --- Global Variables ---
 Scheduler     userScheduler; // Scheduler for tasks
 painlessMesh  mesh;
 Task          taskSendData(TASK_SECOND * 2, TASK_FOREVER, &sendData); // Task to send data every 2 seconds
 
 // --- Function Declarations ---
 void sendData();
 void receivedCallback(uint32_t from, String &msg);
 void newConnectionCallback(uint32_t nodeId);
 void changedConnectionCallback();
 void nodeTimeAdjustedCallback(int32_t offset);
 
 // --- Setup ---
 void setup() {
   Serial.begin(115200);
   Serial.println();
   Serial.println("ESP1: Sensor Node Initializing...");
 
   // Optional: Configure ADC width and attenuation if needed
   // adc1_config_width(ADC_WIDTH_BIT_12); // Set 12-bit resolution
   // adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11); // Configure GPIO4 (Channel 3) for 0-3.3V range
 
   // Initialize the mesh
   mesh.setDebugMsgTypes(ERROR); // Set mesh logging level (can use INFO, DEBUG, etc.)
   mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
 
   // Register callbacks
   mesh.onReceive(&receivedCallback);
   mesh.onNewConnection(&newConnectionCallback);
   mesh.onChangedConnections(&changedConnectionCallback);
   mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
 
   // Add the task to the scheduler
   userScheduler.addTask(taskSendData);
   taskSendData.enable();
 
   Serial.println("ESP1: Initialization Complete. Sending data...");
 }
 
 // --- Loop ---
 void loop() {
   mesh.update(); // Keep the mesh network running
   // User scheduler automatically runs tasks
 }
 
 // --- Task Implementation ---
 void sendData() {
   // Read ADC value
   int adcValue = analogRead(adcPin);
 
   // Create message string
   String msg = "ADC Value: ";
   msg += adcValue;
 
   // Broadcast the message to all nodes in the mesh
   mesh.sendBroadcast(msg);
 
   Serial.print("Sent data: ");
   Serial.println(msg);
   Serial.printf("My Node ID: %u\n", mesh.getNodeId()); // Print own node ID
 }
 
 // --- Mesh Callbacks ---
 void receivedCallback(uint32_t from, String &msg) {
   // This node primarily sends, but good practice to have a receiver callback
   Serial.printf("Received message from %u: %s\n", from, msg.c_str());
   // Could add logic here if ESP1 needs to react to messages
 }
 
 void newConnectionCallback(uint32_t nodeId) {
   Serial.printf("--> New Connection, nodeId = %u\n", nodeId);
   Serial.printf("    Total connections: %d\n", mesh.getNodeList().size());
 }
 
 void changedConnectionCallback() {
   Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
   Serial.printf("Total connections: %d\n", mesh.getNodeList().size());
 }
 
 void nodeTimeAdjustedCallback(int32_t offset) {
   Serial.printf("Adjusted time %ims. Offset = %d\n", mesh.getNodeTime(), offset);
 }