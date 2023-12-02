#pragma once

#include <vector>
#include "scale.h"

class Progression {
    private:
        Scale scale;
        std::vector<HChord *> chords;

    public:
        Progression();

        void generateNextChord();

        std::vector<std::vector<Note *>> getProgression();
};
