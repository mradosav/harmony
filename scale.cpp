#include <random>

#include "scale.h"

Scale::Scale() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rootDist(0, NoteName::NOTES_PER_OCTAVE);
    root = rootDist(gen);
}

NoteName Scale::noteFromDegree(int deg) {
    return NoteName(root + majorScaleMap[deg % NOTES_PER_SCALE]);
}

HChord *Scale::chordFromDegree(int deg) {
    HChord *ret = new HChord();
    NoteName root = noteFromDegree(deg);
    ret->addNoteName(Note::ROOT, root);
    NoteName third = noteFromDegree(deg + 2);
    ret->addNoteName(Note::THIRD, third);
    NoteName fifth = noteFromDegree(deg + 4);
    ret->addNoteName(Note::FIFTH, fifth);

    switch(deg) {
        case 0:
        case 3:
        case 4:
            ret->type = HChord::MAJOR;
            break;
        case 1:
        case 2:
        case 5:
            ret->type = HChord::MINOR;
            break;
        case 6:
            ret->type = HChord::DIMINISHED;
            break;
    }
    return ret;
}
