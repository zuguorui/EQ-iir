#ifndef _DYNAMIC_COMPRESSOR_H_
#define _DYNAMIC_COMPRESSOR_H_

#include <iostream>
#include <stdlib.h>
#include <cmath>

using namespace std;

class DynamicCompressor
{
public:
    DynamicCompressor(float thresholdDB, float dBBase, float compressRatio, float attackTime, float releaseTime, float makeUpGain, int sampleRate);
    ~DynamicCompressor();

    void setMakeGain(float makeUpGain);
    float getMakeUpGain();
    void process(double *L, double *R, int numSamples);

private:
    
    float thresholdDB;
    float dBBase;
    float attackTime;
    float compressRatio;
    float releaseTime;
    float makeUpGain;
    int sampleRate;
    

    float yl_prev = 0.0;
    float a_attack = 0;
    float a_release = 0;


};



#endif
