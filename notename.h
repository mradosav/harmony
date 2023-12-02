#pragma once

#include <string>

class NoteName {
    private:
       int val;

    public:
      static const int NOTES_PER_OCTAVE = 12;

      NoteName(int v) {
          val = v % NOTES_PER_OCTAVE;
      }

      int toInt() {
          return val;
      }

      std::string toString() {
          switch(val) {
              case 0: return "C";
              case 1: return "C#";
              case 2: return "D";
              case 3: return "Eb";
              case 4: return "E";
              case 5: return "F";
              case 6: return "F#";
              case 7: return "G";
              case 8: return "Ab";
              case 9: return "A";
              case 10: return "Bb";
              case 11: return "B";
              default: return "";
          }
      }

      bool operator== (NoteName test) {
          return (this->val == test.val);
      }
};
