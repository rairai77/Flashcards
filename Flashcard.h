#ifndef FLASHCARD_H
#define FLASHCARD_H

#include <Arduino.h>

class Flashcard {
public:
  String character;
  String pinyin;
  String definition;
  float weight;
  bool wrongLastTime;

  Flashcard();
  Flashcard(String _character,
            String _pinyin,
            String _definition,
            float _weight,
            bool _wrongLastTime);
  
  void update(bool status);
  static Flashcard fromStorageString(String data);
  String toStorageString();
};

#endif  // FLASHCARD_H