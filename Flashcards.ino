#include <epdiy.h>
#include <M5GFX.h>
#include "TouchButton.h"
#include "FileManager.h"

M5GFX display;

// Create a button
TouchButton myButton(100, 300, 200, 60, "READ FILE");

// Text area to display file contents
int textX = 50;
int textY = 100;
int textWidth = 540;
int textHeight = 180;

// File manager instance
FileManager fileManager;

void setup(void) {
  // Initialize serial (for debugging)
  Serial.begin(115200);
  delay(500);
  
  // Initialize the file manager
  if (!fileManager.begin()) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted successfully");
    fileManager.createEmptyFile();
  }
  
  // Initialize the display
  display.init();
  display.clear();
  display.setFont(&fonts::Font4);

  if (!display.touch()) {
    display.setTextDatum(textdatum_t::middle_center);
    display.drawString("Touch not found.", display.width() / 2, display.height() / 2);
  }

  display.setEpdMode(epd_mode_t::epd_fastest);
  display.startWrite();

  // Draw title
  display.setTextColor(TFT_BLACK);
  display.setTextDatum(textdatum_t::top_center);
  display.drawString("M5Paper File Reader", display.width() / 2, 30);
  
  // Draw text area
  display.drawRect(textX, textY, textWidth, textHeight, TFT_BLACK);
  
  // Draw button
  myButton.draw(display);
  display.display();
}

void readAndDisplayFile() {
  // Clear text area
  display.fillRect(textX, textY, textWidth, textHeight, TFT_WHITE);
  display.drawRect(textX, textY, textWidth, textHeight, TFT_BLACK);
  
  // Read file content
  String fileContent = fileManager.readFile("/flashcards.txt", 3); // Read first line
  
  if (fileContent.length() == 0) {
    display.setTextColor(TFT_BLACK);
    display.setCursor(textX + 10, textY + 20);
    display.println("Error: File not found or empty!");
    return;
  }
  
  // Display the content
  display.setTextColor(TFT_BLACK);
  display.setCursor(textX + 10, textY + 20);
  
  int lineHeight = 20;
  int currentY = textY + 20;
  int lastBreak = 0;
  
  for (int i = 0; i < fileContent.length(); i++) {
    if (fileContent[i] == '\n') {
      display.setCursor(textX + 10, currentY);
      display.println(fileContent.substring(lastBreak, i));
      lastBreak = i + 1;
      currentY += lineHeight;
    }
  }
  
  // Print the last line if needed
  if (lastBreak < fileContent.length()) {
    display.setCursor(textX + 10, currentY);
    display.println(fileContent.substring(lastBreak));
  }
}

void loop(void) {
  lgfx::touch_point_t tp[3];
  int nums = display.getTouchRaw(tp, 3);
  if (nums) {
    display.convertRawXY(tp, nums);
    if (myButton.isTouched(tp, nums)) {
      // Visual feedback - show button press
      myButton.draw(display, true);
      display.display();
      
      // Read and display file contents
      readAndDisplayFile();
      display.display();

      // Reset button appearance after a delay
      vTaskDelay(500 / portTICK_PERIOD_MS);
      myButton.draw(display);
      display.display();
    }
  } else {
    myButton.reset_touch();
  }

  vTaskDelay(10);
}