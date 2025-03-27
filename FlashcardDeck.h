#ifndef FLASHCARD_DECK_H
#define FLASHCARD_DECK_H

#include "Flashcard.h"

class FlashcardDeck {
private:
  Flashcard* cards;
  int currentIndex;
  int totalCards;
  float totalWeight;
  int capacity;

public:
  FlashcardDeck();
  ~FlashcardDeck();
  bool loadFromCSV(const char* filename);
  bool loadProgress(const char* filename);
  bool saveProgress(const char* filename);

  Flashcard& getCurrentCard();
  Flashcard& getNextCard();

  void updateCurrentCard(bool wasCorrect);

private:
  int selectWeightedCard();
  int selectWrongCard();
};

#endif  // FLASHCARD_DECK_H