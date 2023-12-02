#include "chord.h"

HChord::HChord() {
    type = NONE;
    seventh = false;
    notes.fill(NULL);
}

void HChord::addNoteName(Note::ChordPosition p, NoteName n) {
    this->notenames.insert(p, n);
}

std::vector<Note *> HChord::noteFromChordPosition(Note::ChordPosition p) {
    std::vector<Note *> ret;
    for (Note *n : notes) {
        if (n->pos == p) {
            ret.push_back(n);
        }
    }

    return ret;
}

Note * HChord::noteFromVoice(int v) {
    return notes.at(v);
}

NoteName HChord::getRoot() {
    return notenames.at(Note::ROOT);
}
