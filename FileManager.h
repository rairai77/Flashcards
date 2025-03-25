#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <FS.h>
#include <SPIFFS.h>

class FileManager {
public:
  FileManager();
  
  // Initialize the file system
  bool begin();
  
  // Create a sample file if it doesn't exist
  void createSampleFile();
  
  // Read file content
  String readFile(const char* path, int maxChars = -1);
  
  // Write to a file
  bool writeFile(const char* path, const String& content);
  
  // Append to a file
  bool appendFile(const char* path, const String& content);
  
  // Delete a file
  bool deleteFile(const char* path);
  
  // List files in directory
  void listFiles();
};

#endif // FILE_MANAGER_H