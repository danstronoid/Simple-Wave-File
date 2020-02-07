#ifndef WAVEFILEHEADERS_H_INCLUDED
#define WAVEFILEHEADERS_H_INCLUDED

/*
    Simple Wave File
    Author: Daniel Schwartz

    -- WaveFileHeaders --

    These three structs hold the header information for a wave file.
    The default values here are meant to be overwritten with the exception
    of the char[]s which remain constant in every wave file.
*/

#include <cstdint>

// The default values for these headers are for a 44100Hz, 16-bit, stereo, PCM Wave
struct RiffHeader {
    /* RIFF header */
    char     chunkID[4]{'R', 'I', 'F', 'F'};            //"RIFF"
    uint32_t chunkSize{36};                             // 36 + subChunk2Size
    char     format[4]{'W', 'A', 'V', 'E'};             // "WAVE"
};

struct WaveFormatHeader {
    /* Format */
    char     subChunk1ID[4]{'f', 'm', 't', ' '};        // "fmt "
    uint32_t subChunk1Size{16};                         // 16 for PCM
    uint16_t audioFormat{1};                            // PCM = 1
    uint16_t numChannels{2};                            // number of channels, 1 = mono, 2 = stereo
    uint32_t sampleRate{44100};                         // sample rate in Hz
    uint32_t byteRate{176400};                          // sampleRate * numChannels * bitsPerSample/8
    uint16_t blockAlign{4};                             // numChannels * bitsPerSample/8
    uint16_t bitsPerSample{16};                         // bit depth, 8, 16, 24 etc

    // format may contain other info
};

struct WaveDataHeader {
    /* Data */
    char     subChunk2ID[4]{'d', 'a', 't', 'a'};        // "data"
    uint32_t subChunk2Size{};                           // numSamples * numChannels * bitsPerSample/8

    // data comes after this header
};

// total size of all of the headers
static constexpr int WAVE_HEADER_SIZE = sizeof(RiffHeader) + sizeof(WaveFormatHeader)
    + sizeof(WaveDataHeader);

#endif // WAVEFILEHEADERS_H_INCLUDED
