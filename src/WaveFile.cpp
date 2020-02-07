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

#include "WaveFile.h"
#include "util.h"

WaveFile::WaveFile():
    m_riffHeader{}, m_formatHeader{}, m_dataHeader{}, m_data{nullptr},
    m_length{}, m_sampleRate{}, m_nChannels{}, m_bitDepth{}
{
}

// constructor for creating a new wave file
WaveFile::WaveFile(uint32_t length, uint32_t sampleRate, uint16_t nChannels, uint16_t bitDepth):
    m_riffHeader{}, m_formatHeader{}, m_dataHeader{}, m_data{nullptr},
    m_length(length), m_sampleRate(sampleRate), m_nChannels(nChannels), m_bitDepth(bitDepth)
{
    setHeaders();
    m_data.reset(new uint8_t[m_dataHeader.subChunk2Size]{});
}

// the copy constructor performs a deep copy
WaveFile::WaveFile(const WaveFile &other): WaveFile()
{
    m_riffHeader = other.m_riffHeader;
    m_formatHeader = other.m_formatHeader;
    m_dataHeader = other.m_dataHeader;

    m_length = other.m_length;
    m_sampleRate = other.m_sampleRate;
    m_nChannels = other.m_nChannels;
    m_bitDepth = other.m_bitDepth;

    m_data.reset(new uint8_t[m_dataHeader.subChunk2Size]{});
    for (uint32_t i = 0; i < m_dataHeader.subChunk2Size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

// constructor for directly reading a wave file into memory
WaveFile::WaveFile(string fileName): WaveFile()
{
    read(fileName);
}

// the overloaded assignment operator performs a deep copy
WaveFile& WaveFile::operator=(const WaveFile &other) {
    if (this == &other) {
        return *this;
    }

    m_riffHeader = other.m_riffHeader;
    m_formatHeader = other.m_formatHeader;
    m_dataHeader = other.m_dataHeader;

    m_length = other.m_length;
    m_sampleRate = other.m_sampleRate;
    m_nChannels = other.m_nChannels;
    m_bitDepth = other.m_bitDepth;

    m_data.reset(new uint8_t[m_dataHeader.subChunk2Size]{});
    for (uint32_t i = 0; i < m_dataHeader.subChunk2Size; ++i) {
        m_data[i] = other.m_data[i];
    }

    return *this;
}

// reads a standard PCM wave file into memory
// returns true if successful
bool WaveFile::read(string inFileName) {
    ifstream inFile;
    inFile.open(inFileName, ios::binary);

    if (!inFile) {
        cout << "Cannot open file: " << inFileName << endl;
        return false;
    }

    cout << "Reading from file: " << inFileName << endl;

    inFile.read(reinterpret_cast<char *>(&m_riffHeader), sizeof(RiffHeader));

    // first check to ensure a valid wave file
    if (!wordCompare(m_riffHeader.chunkID, "RIFF")) {
        cout << "File format error: missing RIFF header" << endl;
        inFile.close();
        return false;
    }

    // parse through the extra parameters until the format is reached
    do {
        inFile.ignore(numeric_limits<streamsize>::max(), 'f');
        if (inFile.eof()) {
            cout << "File format error: missing format header" << endl;
            inFile.close();
            return false;
        }
        inFile.seekg(-1, inFile.cur);
        inFile.read(reinterpret_cast<char *>(&m_formatHeader), sizeof(WaveFormatHeader));
    } while (!wordCompare(m_formatHeader.subChunk1ID, "fmt "));

    // audioFormat 1 is a standard PCM wave, currently this is the only kind of
    // wave file supported, but I would like to add support for extensible waves and
    // 32bit float waves in the future
    if (m_formatHeader.audioFormat != 1) {
        cout << "Incompatible wave format:" << m_formatHeader.audioFormat << endl;
        inFile.close();
        return false;
    }

    // parse through the extra parameters until the data is reached
    do {
        inFile.ignore(numeric_limits<streamsize>::max(), 'd');
        if (inFile.eof()) {
            cout << "File format error: missing data header" << endl;
            inFile.close();
            return false;
        }
        inFile.seekg(-1, inFile.cur);
        inFile.read(reinterpret_cast<char *>(&m_dataHeader), sizeof(WaveDataHeader));
    } while (!wordCompare(m_dataHeader.subChunk2ID, "data"));

    m_data.reset(new uint8_t[m_dataHeader.subChunk2Size]);
    inFile.read(reinterpret_cast<char *>(m_data.get()), m_dataHeader.subChunk2Size);

    inFile.close();

    if (!inFile) {
        cout << "Error closing file: " << inFileName << endl;
        return false;
    }

    // if there were extra parameters thrown away, then recalculate the size
    if (m_formatHeader.subChunk1Size != sizeof(WaveFormatHeader)) {
        m_formatHeader.subChunk1Size = sizeof(WaveFormatHeader)
            - sizeof(m_formatHeader.subChunk1ID) - sizeof(m_formatHeader.subChunk1Size);
        m_riffHeader.chunkSize = m_dataHeader.subChunk2Size + WAVE_HEADER_SIZE
            - sizeof(m_riffHeader.chunkID) - sizeof(m_riffHeader.chunkSize);
    }

    // calculate the total number of samples and store all core attributes
    // in a more easily accessible place
    m_length = m_dataHeader.subChunk2Size / m_formatHeader.numChannels
            / (m_formatHeader.bitsPerSample / 8);
    m_nChannels = m_formatHeader.numChannels;
    m_sampleRate = m_formatHeader.sampleRate;
    m_bitDepth = m_formatHeader.bitsPerSample;

    return true;
}

// writes the WaveFile object to a new wave file
// returns true if successful
bool WaveFile::write(string outFileName) {
    ofstream outFile;
    outFile.open(outFileName, ios::binary);

    if (!outFile) {
        cout << "Cannot create file: " << outFileName << endl;
        return false;
    }

    cout << "Writing to file: " << outFileName << endl;

    outFile.write(reinterpret_cast<char *>(&m_riffHeader), sizeof(RiffHeader));
    outFile.write(reinterpret_cast<char *>(&m_formatHeader), sizeof(WaveFormatHeader));
    outFile.write(reinterpret_cast<char *>(&m_dataHeader), sizeof(WaveDataHeader));
    outFile.write(reinterpret_cast<char *>(m_data.get()), m_dataHeader.subChunk2Size);
    outFile.close();

    if (!outFile) {
        cout << "Error closing file: " << outFileName << endl;
        return false;
    }

    return true;
}

// Returns an AudioSample given a sample number within the wave file.
// This function perform the conversion from an uint8_t[] to
// a double.
AudioSample WaveFile::getSample(uint32_t sample) {
    // if the sample is beyond the length of the file, return empty audio data
    if (sample > m_length) {
        //cout << "Sample exceeds file length" << endl;
        return AudioSample();
    }

    double left{};
    double right{};
    uint32_t index = sample * m_nChannels * (m_bitDepth / 8);

    switch (m_bitDepth) {
        case 8:
        {
            left = m_data[index] / static_cast<double>(numeric_limits<uint8_t>::max());

            if (m_nChannels == 2) {
                right = m_data[index + 1] / static_cast<double>(numeric_limits<uint8_t>::max());
            }
            break;
        }
        case 16:
        {
            int16_t left_16bit = m_data[index] | (m_data[index + 1] << 8);
            left = left_16bit / static_cast<double>(numeric_limits<int16_t>::max());

            if (m_nChannels == 2) {
                int16_t right_16bit = m_data[index + 2] | (m_data[index + 3] << 8);
                right = right_16bit / static_cast<double>(numeric_limits<int16_t>::max());
            }
            break;
        }
        case 24:
        {
            int32_t left_24bit = (m_data[index] << 8) | (m_data[index + 1] << 16) | (m_data[index + 2] << 24);
            left = left_24bit / static_cast<double>(numeric_limits<int32_t>::max());

            if (m_nChannels == 2) {
                int32_t right_24bit = (m_data[index + 3] << 8) | (m_data[index + 4] << 16) | (m_data[index + 5] << 24);
                right = right_24bit / static_cast<double>(numeric_limits<int32_t>::max());

            }
            break;
        }
        case 32:
        {
            int32_t left_32bit = m_data[index] | (m_data[index + 1] << 8) | (m_data[index + 2] << 16) | (m_data[index + 3] << 24);
            left = left_32bit / static_cast<double>(numeric_limits<int32_t>::max());

            if (m_nChannels == 2) {
                int32_t right_32bit = m_data[index + 4] | (m_data[index + 5] << 8) | (m_data[index + 6] << 16) | (m_data[index + 7] << 24);
                right = right_32bit / static_cast<double>(numeric_limits<int32_t>::max());
            }
            break;
        }
        default:
            cout << "Invalid bit depth" << endl;
            return AudioSample(0, 0);
    }

    return AudioSample(left, right);
}

// Sets a sample within the wave file to the AudioSample passed in.
// This function perform the conversion back from a double
// to an uint8_t[].
void WaveFile::setSample(uint32_t sample, const AudioSample &audio) {
    if (sample > m_length) {
        cout << "Sample exceeds file length" << endl;
        return;
    }

    uint32_t index = sample * m_nChannels * (m_bitDepth / 8);

    switch (m_bitDepth) {
        case 8:
        {
            m_data[index] = static_cast<uint8_t>(audio.left * numeric_limits<uint8_t>::max());

            if (m_nChannels == 2) {
                 m_data[index + 1] = static_cast<uint8_t>(audio.right * numeric_limits<uint8_t>::max());
            }
            break;
        }
        case 16:
        {
            int16_t left_16bit = static_cast<int16_t>(audio.left * numeric_limits<int16_t>::max());
            m_data[index] = left_16bit;
            m_data[index + 1] = left_16bit >> 8;

            if (m_nChannels == 2) {
                int16_t right_16bit = static_cast<int16_t>(audio.right * numeric_limits<int16_t>::max());
                m_data[index + 2] = right_16bit;
                m_data[index + 3] = right_16bit >> 8;
            }
            break;
        }
        case 24:
        {
            int32_t left_24bit = static_cast<int32_t>(audio.left * numeric_limits<int32_t>::max());
            m_data[index] = left_24bit >> 8;
            m_data[index + 1] = left_24bit >> 16;
            m_data[index + 2] = left_24bit >> 24;

            if (m_nChannels == 2) {
                int32_t right_24bit = static_cast<int32_t>(audio.right * numeric_limits<int32_t>::max());
                m_data[index + 3] = right_24bit >> 8;
                m_data[index + 4] = right_24bit >> 16;
                m_data[index + 5] = right_24bit >> 24;
            }
            break;
        }
        case 32:
        {
            int32_t left_32bit = static_cast<int32_t>(audio.left * numeric_limits<int32_t>::max());
            m_data[index] = left_32bit;
            m_data[index + 1] = left_32bit >> 8;
            m_data[index + 2] = left_32bit >> 16;
            m_data[index + 3] = left_32bit >> 24;

            if (m_nChannels == 2) {
                int32_t right_32bit = static_cast<int32_t>(audio.right * numeric_limits<int32_t>::max());
                m_data[index + 4] = right_32bit;
                m_data[index + 5] = right_32bit >> 8;
                m_data[index + 6] = right_32bit >> 16;
                m_data[index + 7] = right_32bit >> 24;
            }
            break;
        }
        default:
            cout << "Invalid bit depth" << endl;
            return;
    }
}

// this print function displays only the core information about an audio file
void WaveFile::print() {
    cout << "Length: " << m_length << " samples" << endl;
    cout << m_sampleRate << "Hz" << endl;
    if (m_nChannels == 1) {
        cout << "Mono" << endl;
    } else {
        cout << "Stereo" << endl;
    }
    cout << m_bitDepth << "-bit" << endl;
}

// this print function is used to display all of the header information
// useful for debugging
void WaveFile::printHeaderInfo() {
    //riff header
    printWord(m_riffHeader.chunkID);
    cout << "Chunk size: " << m_riffHeader.chunkSize << endl;
    printWord(m_riffHeader.format);

    //format header
    printWord(m_formatHeader.subChunk1ID);
    cout << "SubChunk1 size: " << m_formatHeader.subChunk1Size << endl;
    cout << "Audio format: " << m_formatHeader.audioFormat << endl;
    cout << "Channels: " << m_formatHeader.numChannels << endl;
    cout << "Sample rate: " << m_formatHeader.sampleRate << endl;
    cout << "Byte rate: " << m_formatHeader.byteRate << endl;
    cout << "Block align: " << m_formatHeader.blockAlign << endl;
    cout << "Bit depth: " << m_formatHeader.bitsPerSample << endl;

    //data header
    printWord(m_dataHeader.subChunk2ID);
    cout << "SubChunk2 size: " << m_dataHeader.subChunk2Size << endl;

    cout << "Contains: " << m_length << " samples" << endl;
}

// recalculates header values
void WaveFile::setHeaders() {
    m_formatHeader.subChunk1Size = sizeof(WaveFormatHeader)
        - sizeof(m_formatHeader.subChunk1ID) - sizeof(m_formatHeader.subChunk1Size);
    m_formatHeader.numChannels = m_nChannels;
    m_formatHeader.sampleRate = m_sampleRate;
    m_formatHeader.byteRate = m_sampleRate * m_nChannels * (m_bitDepth / 8);
    m_formatHeader.blockAlign = m_nChannels * (m_bitDepth / 8);
    m_formatHeader.bitsPerSample = m_bitDepth;
    m_dataHeader.subChunk2Size = m_length * m_nChannels * (m_bitDepth / 8);
    m_riffHeader.chunkSize = m_dataHeader.subChunk2Size + WAVE_HEADER_SIZE
        - sizeof(m_riffHeader.chunkID) - sizeof(m_riffHeader.chunkSize);
}
