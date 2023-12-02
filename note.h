#pragma once

#include <cmath>

#include "notename.h"

const int MAX_NOTE = 48;

class Note {
    private:
        const double C2_FREQ = 65.4;

    public:
        enum ChordPosition {
            ROOT,
            THIRD,
            FIFTH,
            SEVENTH,
            INVALID_POS
        };

        enum NoteStatus {
            NOT_CHECKED,
            INCORRECT,
            CORRECT
        };

        /* The value ranges from 0 (C2) to 48 (C4);
         * each increment corresponds to a half-step.
         * A value of -1 indicates the value has not
         * been set yet.
         */
        int value;

        int voice;

        ChordPosition pos;

        NoteStatus correct;

        Note(int v) {
            value = v;
            voice = -1;
            pos = INVALID_POS;
            correct = CORRECT;

            while (value < 0) {
                value = value + NoteName::NOTES_PER_OCTAVE;
            }

            while (value > MAX_NOTE) {
                value = value - NoteName::NOTES_PER_OCTAVE;
            }
        }

        Note(int octave, NoteName n) {
            value = (octave * NoteName::NOTES_PER_OCTAVE) + n.toInt();
            voice = -1;
            pos = INVALID_POS;
            correct = CORRECT;
        }

        Note *nearest(NoteName n) {
            int name = this->notename();
            int diff = name - n.toInt();
            if ((diff > -6) && (diff < 6)) {
                return new Note(value - diff);
            } else if (diff >= 6) {
                return new Note(value - diff + NoteName::NOTES_PER_OCTAVE);
            } else {
                return new Note(value - diff - NoteName::NOTES_PER_OCTAVE);
            }
        }

        int notename() {
            return (value % NoteName::NOTES_PER_OCTAVE);
        }

        double frequency() {
            int octave = value / NoteName::NOTES_PER_OCTAVE;
            int nn = value % NoteName::NOTES_PER_OCTAVE;
            double exp = octave + (double)nn / (double)NoteName::NOTES_PER_OCTAVE;
            return C2_FREQ * std::pow(2.0, exp);
        }
};
