#include <random>

#include "progression.h"

Progression::Progression() {
    HChord *first = scale.chordFromDegree(0);
    first->resolve(NULL, true);
    chords.push_back(first);
    generateNextChord();
}

void Progression::generateNextChord() {
    bool test = false;
    std::random_device rd;
    std::mt19937 gen(rd());
    //std::uniform_int_distribution<> dist(0, Scale::NOTES_PER_SCALE - 1);
    std::uniform_int_distribution<> dist(0, 5); //don't allow dim chords yet
    HChord *next = NULL;
    while (!test) {
        free(next);
        int degree = dist(gen);
        if (scale.noteFromDegree(degree) == chords.back()->getRoot()) {
            continue;
        } else if ((chords.size() > 1) && (scale.noteFromDegree(degree) ==
                    chords.at(chords.size() - 2)->getRoot())) {
            continue;
        }
        next = scale.chordFromDegree(degree);
        test = next->resolve(chords.back(), true);
    }
    chords.push_back(next);
}

std::vector<std::vector<Note *>> Progression::getProgression() {
    std::vector<std::vector<Note *>> ret;
    for (int i = 0; i < chords.size(); i++) {
        HChord *c = chords.at(i);
        std::vector<Note *> add;
        for (int i = 0; i < NOTES_PER_CHORD; i++) {
            Note *addn = c->noteFromVoice(i);
            add.push_back(addn);
        }
        ret.push_back(add);
    }

    return ret;
}
