#include "FileManager.h"

FileManager::FileManager() {
  // Constructor
}

bool FileManager::begin() {
  return LittleFS.begin(true);
}

void FileManager::createEmptyFile() {
  if (!LittleFS.exists("/flashcards.txt")) {
    File file = LittleFS.open("/flaschards.txt", "w");
    if (file) {
      file.println("0");
      file.close();
      Serial.println("Dummy file created");
    }
  }
}

String FileManager::readFile(const char* path, int maxLines) {
  File file = LittleFS.open(path, "r");
  String content = "";
  
  if (!file) {
    Serial.println("Failed to open file for reading");
    return content;
  }
  
  Serial.println("Reading file: " + String(path));
  
  if (maxLines < 0) {
    // Read the entire file
    while (file.available()) {
      content += (char)file.read();
    }
  } else {
    // Read limited number of characters
    int count = 0;
    while (file.available() && count < maxLines) {
      char new_char = (char) file.read();
      content += new_char;
      if(new_char=='\n'){
        count++;
      }
    }
  }
  
  file.close();
  return content;
}

bool FileManager::writeFile(const char* path, const String& content) {
  File file = LittleFS.open(path, "w");
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
  File file = LittleFS.open(path, "a");
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
  if (LittleFS.remove(path)) {
    Serial.println("File deleted");
    return true;
  } else {
    Serial.println("Delete failed");
    return false;
  }
}

void FileManager::listFiles() {
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  
  Serial.println("Files in LittleFS:");
  
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" - ");
    Serial.println(file.size());
    file = root.openNextFile();
  }
}