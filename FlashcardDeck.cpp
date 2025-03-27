#include "FlashcardDeck.h"
#include <LittleFS.h>
#include <esp32-hal-psram.h>
#include "Flashcard.h"

FlashcardDeck::FlashcardDeck() {
  capacity = 1000;
  currentIndex = 0;
  totalCards = 0;
  totalWeight = 0;
  if (psramFound()) {
    Serial.println("Using PSRAM");
    cards = (Flashcard*)ps_malloc(capacity * sizeof(Flashcard));
  } else {
    Serial.println("PSRAM not available, using regular heap");
    cards = (Flashcard*)malloc(capacity * sizeof(Flashcard));
  }
}

FlashcardDeck::~FlashcardDeck() {
  free(cards);
}

bool FlashcardDeck::loadFromCSV(const char* filename) {
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open CSV file");
    return false;
  }
  file.readStringUntil('\n');  // skip header
  totalCards = 0;
  totalWeight = 0;
  while (file.available() && totalCards < capacity) {
    String line = file.readStringUntil('\n');
    if (line.length() <= 1) continue;

    int c1 = line.indexOf(',');
    int c2 = line.indexOf(',', c1 + 1);

    if (c1 > 0 && c2 > 0) {
      cards[totalCards] = Flashcard(
        line.substring(0, c1),       // char
        line.substring(c1 + 1, c2),  // pinyin
        line.substring(c2 + 1),      // definition
        1.0,
        false);
      totalCards++;
    }
  }
  file.close();
  loadProgress("/flashcard_progress.dat");
  return (totalCards > 0);
}

bool FlashcardDeck::loadProgress(const char* filename) {
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open progress file");
    return false;
  }
  if (file.available()) {
    currentIndex = file.readStringUntil('\n').toInt();
  }
  int lineCount = 0;

  while (file.available() && lineCount < totalCards) {
    String line = file.readStringUntil('\n');
    if (line.length() <= 1) continue;
    Flashcard card = Flashcard::fromStorageString(line);
    cards[lineCount].weight = card.weight;
    cards[lineCount].wrongLastTime = card.wrongLastTime;
    lineCount++;
  }

  file.close();
  return (lineCount > 0);
}

bool FlashcardDeck::saveProgress(const char* filename) {
  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println("Failed to open progress file");
    return false;
  }
  file.println(String(currentIndex));
  int savedCount = 0;
  for (int i = 0; i < totalCards; i++) {
    if (file.println(cards[i].toStorageString())) {
      savedCount++;
    } else {
      Serial.println("Error saving card " + String(i));
    }
  }
  file.close();
  Serial.println("Saved progress!");
  return (savedCount == totalCards);
}

Flashcard& FlashcardDeck::getCurrentCard() {
  return cards[currentIndex];
}

Flashcard& FlashcardDeck::getNextCard() {
  if (random(100) < 10) {
    selectWrongCard();
  } else {
    selectWeightedCard();
  }
  return getCurrentCard();
}

void FlashcardDeck::updateCurrentCard(bool wasCorrect) {
  cards[currentIndex].update(wasCorrect);
}
int FlashcardDeck::selectWrongCard() {
  int wrongCards[totalCards];
  int wrongCount = 0;
  for (int i = 0; i < totalCards; i++) {
    if (cards[i].wrongLastTime) {
      wrongCards[wrongCount] = i;
      wrongCount++;
    }
  }
  if (wrongCount > 0) {
    int randomIndex = random(wrongCount);
    currentIndex = wrongCards[randomIndex];
    return currentIndex;
  }

  return selectWeightedCard();
}
int FlashcardDeck::selectWeightedCard() {
  if (totalCards <= 1) {
    currentIndex = 0;
    return currentIndex;
  }
  float totalWeight = 0;
  for (int i = 0; i < totalCards; i++) {
    totalWeight += cards[i].weight;
  }
  float randomPoint = random(0, 1000) / 1000.0 * totalWeight;

  float weightSum = 0;
  for (int i = 0; i < totalCards; i++) {
    weightSum += cards[i].weight;
    if (weightSum >= randomPoint) {
      currentIndex = i;
      return currentIndex;
    }
  }

  currentIndex = random(totalCards);
  return currentIndex;
}