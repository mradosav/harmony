#pragma once

#include "chord.h"

class Scale {
    private:
        int root;

    public:
        static const int NOTES_PER_SCALE = 7;

        static constexpr int majorScaleMap[7] = {0, 2, 4, 5, 7, 9, 11};

        Scale();

        NoteName noteFromDegree(int deg);

        HChord *chordFromDegree(int deg);
};
