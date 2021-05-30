#ifndef _WAV_WRITER_H_
#define _WAV_WRITER_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <memory>
#include "WavHead.h"

using namespace std;

class WavWriter{
public:
    static shared_ptr<WavWriter> open(string path, shared_ptr<WavHead> wavHead);
    ~WavWriter();
    void write(void *buffer, int sizeBytes);
    
private:
    WavWriter();

    FILE *file = nullptr;
    shared_ptr<WavHead> wavHead;
    
};

#endif