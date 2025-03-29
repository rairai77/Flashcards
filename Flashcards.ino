#include <epdiy.h>
#include <M5GFX.h>
#include <LittleFS.h>
#include "esp_sleep.h"
#include "TouchButton.h"
#include "FlashcardDeck.h"
#include "Flashcard.h"

M5GFX display;

// Create a button
TouchButton flipButton(100, 400, 200, 60, "Flip Card");
TouchButton correctButton(50, 500, 150, 60, "Correct");
TouchButton wrongButton(250, 500, 150, 60, "Wrong");
TouchButton nextButton(350, 400, 150, 60, "Next");

// Text area to display file contents
int cardX = 25;
int cardY = 100;
int cardWidth = 490;
int cardHeight = 300;

FlashcardDeck* deck = NULL;
bool showAnswer = false;

void setup(void) {
  // Initialize serial (for debugging)
  Serial.begin(115200);
  delay(500);
  // Initialize the file manager
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  } else {
    Serial.println("LittleFS mounted!");
  }

  Serial.println("Initializing Display");
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

  Serial.println("Creating flashcards");
  deck = new FlashcardDeck();
  Serial.println("Loading flashcards");
  if (deck->loadFromCSV("/flashcards.csv")) {
    Serial.println("Loaded cards");
  } else {
    Serial.println("Failed to load cards");
  }

  drawScreen();
}


void drawScreen() {
  display.setTextColor(TFT_BLACK);
  display.fillScreen(TFT_WHITE);

  display.drawRect(cardX, cardY, cardWidth, cardHeight, TFT_BLACK);

  Flashcard& card = deck->getCurrentCard();
  display.setTextDatum(textdatum_t::middle_center);
  // display.setFont(&fonts::Font7);
  display.setFont(&fonts::efontCN_24);
  display.setTextSize(2.0);
  display.drawString(card.character, display.width() / 2, cardY + 60);
  display.setFont(&fonts::FreeSerif9pt7b);
  display.setTextSize(1.0);
  if (showAnswer) {
    display.setFont(&fonts::DejaVu18);
    display.drawString(card.pinyin, display.width() / 2, cardY + 120);
    display.drawString(card.definition, display.width() / 2, cardY + 150);
    correctButton.draw(display);
    wrongButton.draw(display);
  } else {
    flipButton.draw(display);
    nextButton.draw(display);
  }

  display.display();
}

void handleCardResponse(bool wasCorrect) {
  deck->updateCurrentCard(wasCorrect);
  deck->getNextCard();
  showAnswer = false;
  drawScreen();
}

void shutdownSequence() {
  deck->saveProgress("/flashcard_progress.dat");
  display.setBrightness(0);
  display.powerSaveOn();
  Serial.flush();
  esp_deep_sleep_start();
}

void loop(void) {
  static unsigned long lastActivityTime = millis();

  lgfx::touch_point_t tp[3];
  int nums = display.getTouchRaw(tp, 3);
  if (nums) {
    display.convertRawXY(tp, nums);
    lastActivityTime = millis();
    if (!showAnswer) {
      if (flipButton.isTouched(tp, nums)) {
        showAnswer = true;
        drawScreen();
      } else if (nextButton.isTouched(tp, nums)) {
        deck->getNextCard();
        showAnswer = false;
        drawScreen();
      }
    } else {
      if (correctButton.isTouched(tp, nums)) {
        handleCardResponse(true);
      } else if (wrongButton.isTouched(tp, nums)) {
        handleCardResponse(false);
      }
    }
  } else {
    flipButton.reset_touch();
    correctButton.reset_touch();
    wrongButton.reset_touch();
    nextButton.reset_touch();
    if(millis() - lastActivityTime > 30000) {
      shutdownSequence();
    }
  }
  vTaskDelay(10);
}