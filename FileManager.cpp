#include "FileManager.h"

FileManager::FileManager() {
  // Constructor
}

bool FileManager::begin() {
  return SPIFFS.begin(true);
}

void FileManager::createSampleFile() {
  if (!SPIFFS.exists("/myfile.txt")) {
    File file = SPIFFS.open("/myfile.txt", "w");
    if (file) {
      file.println("This is a test file for M5Paper.");
      file.println("Hello from SPIFFS!");
      file.println("You can edit this file using data upload.");
      file.println("This file was created on first run.");
      file.close();
      Serial.println("Test file created");
    }
  }
}

String FileManager::readFile(const char* path, int maxChars) {
  File file = SPIFFS.open(path, "r");
  String content = "";
  
  if (!file) {
    Serial.println("Failed to open file for reading");
    return content;
  }
  
  Serial.println("Reading file: " + String(path));
  
  if (maxChars < 0) {
    // Read the entire file
    while (file.available()) {
      content += (char)file.read();
    }
  } else {
    // Read limited number of characters
    int count = 0;
    while (file.available() && count < maxChars) {
      content += (char)file.read();
      count++;
    }
  }
  
  file.close();
  return content;
}

bool FileManager::writeFile(const char* path, const String& content) {
  File file = SPIFFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }
  
  if (file.print(content)) {
    Serial.println("File written");
    file.close();
    return true;
  } else {
    Serial.println("Write failed");
    file.close();
    return false;
  }
}

bool FileManager::appendFile(const char* path, const String& content) {
  File file = SPIFFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return false;
  }
  
  if (file.print(content)) {
    Serial.println("Message appended");
    file.close();
    return true;
  } else {
    Serial.println("Append failed");
    file.close();
    return false;
  }
}

bool FileManager::deleteFile(const char* path) {
  if (SPIFFS.remove(path)) {
    Serial.println("File deleted");
    return true;
  } else {
    Serial.println("Delete failed");
    return false;
  }
}

void FileManager::listFiles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  
  Serial.println("Files in SPIFFS:");
  
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" - ");
    Serial.println(file.size());
    file = root.openNextFile();
  }
}