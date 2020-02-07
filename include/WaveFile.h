#ifndef WAVEFILE_H
#define WAVEFILE_H

/*
    Simple Wave File
    Author: Daniel Schwartz

    -- WaveFile --

    The WaveFile class creates objects that hold all data
    pertaining to a particular wave file.  WaveFile objects can
    be created by opening and reading a wave file into memory, or
    by creating a new empty wave file.  The audio data in a wave
    file can be manipulated using the getSample() and setSample()
    methods.  After processing, a WaveFile object can be written to
    a new wave file using the write method.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <memory>
#include "WaveFileHeaders.h"
#include "AudioSample.h"

using namespace std;

class WaveFile {
private:
    // header info
    RiffHeader m_riffHeader;
    WaveFormatHeader m_formatHeader;
    WaveDataHeader m_dataHeader;

    // audio data
    unique_ptr<uint8_t[]> m_data;

    // the core attributes of an audio file
    uint32_t m_length;
    uint32_t m_sampleRate;
    uint16_t m_nChannels;
    uint16_t m_bitDepth;

public:
    WaveFile();
    WaveFile(uint32_t length, uint32_t sampleRate = 44100, uint16_t nChannels = 2, uint16_t bitDepth = 16);
    WaveFile(const WaveFile &other);
    WaveFile(string fileName);
    WaveFile& operator=(const WaveFile &other);

    // read and write, returns true if successful
    bool read(string inFileName);
    bool write(string outFileName);

    // get and set methods for an audio sample
    AudioSample getSample(uint32_t sample);
    void setSample(uint32_t sample, const AudioSample &audio);

    // print methods
    void print();
    void printHeaderInfo();

    // get methods
    uint32_t length() const { return m_length; }
    uint32_t sampleRate() const { return m_sampleRate; }
    uint16_t nChannels() const { return m_nChannels; }
    uint16_t bitDepth() const { return m_bitDepth; }

private:
    // used to recalculate header values
    void setHeaders();
};

#endif // WAVEFILE_H
