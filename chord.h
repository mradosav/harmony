#pragma once

#include <vector>
#include <array>

#include "note.h"
#include "notename.h"
#include "bagset.h"

const int NOTES_PER_CHORD = 4;

class HChord {
    private:
        BagSet<Note::ChordPosition, NoteName> notenames;
        std::array<Note *, NOTES_PER_CHORD> notes;

    public:
        enum ChordType {
            MAJOR,
            MINOR,
            DIMINISHED,
            NONE
        };

        ChordType type;
        bool seventh;

        HChord();

        void addNoteName(Note::ChordPosition p, NoteName n);

        bool resolve(HChord *prev, bool rootPosition=false);

        std::vector<Note *> noteFromChordPosition(Note::ChordPosition p);

        Note *noteFromVoice(int v);

        NoteName getRoot();
};
