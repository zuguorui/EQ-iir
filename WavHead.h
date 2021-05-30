#ifndef _WAV_HEAD_H_
#define _WAV_HEAD_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

using namespace std;

#define STANDARD_WAVE_HEAD_LEN 44

class WavHead
{
public:
    WavHead();
    ~WavHead();

    bool parseFromFile(FILE *file);

    bool writeToFile(FILE *file);

    string chunkID; // 4 bytes
    int32_t chunkSize;
    string format; // 4 bytes
    string subChunk1ID; // 4 bytes
    int32_t subChunk1Size;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t sampleRate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
    string subChunk2ID; // 4 bytes
    int32_t subChunk2Size;

    int32_t headTotalSize = 0;
    int32_t totalSamples = 0;

};
#endif