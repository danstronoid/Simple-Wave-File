/*
    Simple Wave File
    Author: Daniel Schwartz

    This is a simple API for working with wave audio files.
    Wave files are read into memory as audio data which can
    then be processed and written to a new wave file.
    This is by no means robust but a good exercise in working
    with audio data. It provides a simple way to experiment and
    try some wacky things.

    At the moment, this assumes a little endian system.
*/

#include <iostream>
#include <cmath>
#include "WaveFile.h"
using namespace std;

static const double PI = 3.141592653589793238463;

int main()
{
    // parameters for creating a new wave file
    // (length in samples, sample rate = 44100, channels = 2, bit depth = 16)
    WaveFile wave(44100 * 5, 44100, 1, 16);

    double freq{440};
    double amp{0.6};

    for (uint32_t i = 0; i < wave.length(); ++i){
        //AudioSample sample = wave.getSample(i);

        // *** Processing goes here *** //
        AudioSample sineWave = sin(2 * PI * i * (freq) / wave.sampleRate()) * amp;

        wave.setSample(i, sineWave);
    }

    wave.write("hello_sine.wav");

    wave.print();

    return 0;
}
