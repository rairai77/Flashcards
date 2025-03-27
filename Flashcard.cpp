#include "Flashcard.h"

Flashcard::Flashcard() {
  // constructor
}
Flashcard::Flashcard(String _character,
                     String _pinyin,
                     String _definition,
                     float _weight,
                     bool _wrongLastTime)
  : character(_character),
    pinyin(_pinyin),
    definition(_definition),
    weight(_weight),
    wrongLastTime(_wrongLastTime) {}

void Flashcard::update(bool status) {  // True if right, False if wrong
  wrongLastTime = !status;
  if (status) {
    weight *= 0.95;
  } else {
    weight *= 1.1;
  }
}

String Flashcard::toStorageString() {
  return character + "|" + pinyin + "|" + definition + "|" + 
         String(weight) + "|" + String(wrongLastTime ? 1 : 0);
}

Flashcard Flashcard::fromStorageString(String data) {
  int d1 = data.indexOf('|');
  int d2 = data.indexOf('|', d1 + 1);
  int d3 = data.indexOf('|', d2 + 1);
  int d4 = data.indexOf('|', d3 + 1);

  return Flashcard(
    data.substring(0, d1), // character
    data.substring(d1+1, d2), // pinyin
    data.substring(d2+1, d3), // definition
    data.substring(d3+1, d4).toFloat(), // weight
    data.substring(d4+1).toInt() == 1 // wrongLastTime
  );
}