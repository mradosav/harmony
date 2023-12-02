#define _USE_MATH_DEFINES 1
#include <math.h>
#include <stdint.h>
#include <windows.h>
#include <mmeapi.h>
#include <mmreg.h>
#include <vector>

#include "note.h"

//modified from https://stackoverflow.com/questions/5814869/playing-an-arbitrary-sound-on-windows

bool play(std::vector<std::vector<Note *>> notes) {
    int samples = 44100;
    int length = 1;    //number of seconds the wave will play

    int16_t *buffer;

    WAVEFORMATEX wf = {0};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.cbSize = 0;
    wf.nSamplesPerSec = samples;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = sizeof(buffer[0]);
    wf.nAvgBytesPerSec = samples * sizeof(buffer[0]);

    int chordSize = samples * length;
    int bufferSize = chordSize * notes.size();

    buffer = (int16_t *)calloc(bufferSize, sizeof(*buffer));
    if (buffer == NULL) {
        return false;
    }

    int mult = (2 << 14) / notes.at(0).size();
    for (int i = 0; i < notes.size(); i++) {
        std::vector<double> freqs;
        for (Note *n : notes.at(i)) {
            freqs.push_back(n->frequency());
        }

        for (int j = 0; j < chordSize; j++) {
            double arg = 2.0 * M_PI * (double)j / (double)samples;
            int16_t val = 0;
            for (double f : freqs) {
                val += (int16_t)(sin(f * arg) * mult);
            }
            buffer[i * chordSize + j] = val;
        }
    }

    HWAVEOUT hWavOut = NULL;
    MMRESULT mmresult = waveOutOpen(&hWavOut, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL);
    if (mmresult != MMSYSERR_NOERROR) {
        free(buffer);
        return false;
    }

    timeBeginPeriod(length * notes.size());

    WAVEHDR whdr = {0};
    whdr.dwBufferLength = (unsigned long)(bufferSize * sizeof(buffer[0]));
    whdr.lpData = (LPSTR)buffer;
    whdr.dwFlags = 0;
    mmresult = waveOutPrepareHeader(hWavOut, &whdr, sizeof(whdr));
    if (mmresult != MMSYSERR_NOERROR) {
        timeEndPeriod(length);
        free(buffer);
        return false;
    }

    unsigned long start = GetTickCount();
    mmresult = waveOutWrite(hWavOut, &whdr, sizeof(whdr));
    Sleep((unsigned long)1000 * length * notes.size() - GetTickCount() + start);

    waveOutUnprepareHeader(hWavOut, &whdr, sizeof(whdr));
    timeEndPeriod(length * notes.size());
    free(buffer);

    return true;
}
