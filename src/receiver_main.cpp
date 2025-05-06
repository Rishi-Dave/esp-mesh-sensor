/**
 * ESP32-S3 Mesh Gateway Node (ESP2)
 * Receives data from the ESP-MESH network and prints it to the Serial Monitor.
 */

#include <Arduino.h>
#include "painlessMesh.h" // https://github.com/gmag11/painlessMesh

// --- Configuration ---
// IMPORTANT: Use the EXACT SAME credentials as ESP1
#define   MESH_PREFIX     "MyMeshNetwork" // Name of your mesh network
#define   MESH_PASSWORD   "MySecurePassword" // Password for your mesh network
#define   MESH_PORT       5555 // Default painlessMesh port

// --- Global Variables ---
Scheduler     userScheduler; 
painlessMesh  mesh;

// --- Function Declarations ---
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

// --- Setup ---
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP2: Gateway Node Initializing...");

  // Initialize the mesh
  mesh.setDebugMsgTypes(ERROR); // Set mesh logging level
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

  // Register callbacks (Crucially, the receivedCallback)
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  Serial.println("ESP2: Initialization Complete. Waiting for data...");
}

// --- Loop ---
void loop() {
  mesh.update(); // Keep the mesh network running
}

// --- Mesh Callbacks ---
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Data received from Node %u: %s\n", from, msg.c_str());
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
