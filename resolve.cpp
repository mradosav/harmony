#include "chord.h"
#include "bag.h"

Note *nearest(Note *previous, BagSet<Note::ChordPosition, NoteName> notenames, Bag<Note::ChordPosition> &positions) {
    Note::ChordPosition bestPos = Note::INVALID_POS;
    int bestDistance = MAX_NOTE;
    Note *ret = NULL;

    for (Note::ChordPosition pos : positions) {
        Note *test = previous->nearest(notenames.at(pos));
        int dist = 0;
        if (test->value > previous->value) {
            dist = test->value - previous->value;
        } else {
            dist = previous->value - test->value;
        }
        if (dist < bestDistance) {
            bestDistance = dist;
            bestPos = pos;
            ret = test;
        }
    }
    positions.remove(bestPos);
    ret->pos = bestPos;
    return ret;
}

bool HChord::resolve(HChord *prev, bool rootPosition) {
    Bag<int> voices;
    Bag<Note::ChordPosition> positions;

    for (int i = 0; i < NOTES_PER_CHORD; i++) {
        voices.insert(i);
    }
    positions.insert(Note::ROOT);
    positions.insert(Note::THIRD);
    positions.insert(Note::FIFTH);
    if (this->seventh) {
        positions.insert(Note::SEVENTH);
    }
    if (rootPosition) {
        NoteName rootName = notenames.at(Note::ROOT);
        Note *newRoot;
        if (prev) {
             Note *prevBass = prev->noteFromVoice(0);
             newRoot = prevBass->nearest(rootName);
        } else {
            newRoot = new Note(0, rootName);
        }
        newRoot->pos = Note::ROOT;
        newRoot->voice = 0;
        this->notes.at(0) = newRoot;
        voices.remove(0);
        positions.remove(Note::ROOT);
    }

    while (!voices.empty()) {
        int voice = voices.pickRandom();
        Note *newNote;
        if ((prev) && (!positions.empty())) {
            Note *prevNote = prev->noteFromVoice(voice);
            newNote = nearest(prevNote, notenames, positions);
        } else {
            Note::ChordPosition pos = positions.pickRandom();
            NoteName name = notenames.at(pos);
            positions.remove(pos);
            newNote = new Note(voice, name);
            newNote->pos = pos;
        }
        newNote->voice = voice;
        this->notes.at(voice) = newNote;
        voices.remove(voice);
    }

    return true;
}
