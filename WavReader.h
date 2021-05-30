#ifndef _WAV_READER_H_
#define _WAV_READER_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include "WavHead.h"

using namespace std;

class WavReader
{

public:
    
    ~WavReader();

    static shared_ptr<WavReader> open(string path);

    shared_ptr<WavHead> getWavHead();

    int read(void *buffer, int numSamples);

private:
    WavReader();
    shared_ptr<WavHead> wavHead;

    FILE *file = nullptr;
};




#endif