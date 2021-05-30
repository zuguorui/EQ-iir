#include "DynamicCompressor.h"

DynamicCompressor::DynamicCompressor(float thresholdDB, float dBBase, float compressRatio, float attackTime, float releaseTime, float makeUpGain, int sampleRate)
{
    this->thresholdDB = thresholdDB;
    this->dBBase = dBBase;
    this->compressRatio = compressRatio;
    this->attackTime = attackTime;
    this->releaseTime = releaseTime;
    this->sampleRate = sampleRate;
    this->makeUpGain = makeUpGain;

    this->a_attack = (float)exp(-1 / (sampleRate * 1.0f / 1000 * this->attackTime));
    this->a_release = (float)exp(-1 / (sampleRate * 1.0f / 1000 * this->releaseTime));
    yl_prev = 0.0;
}

DynamicCompressor::~DynamicCompressor()
{

}

void DynamicCompressor::setMakeGain(float makeUpGain)
{
    this->makeUpGain = makeUpGain;
}

float DynamicCompressor::getMakeUpGain()
{
    return this->makeUpGain;
}

void DynamicCompressor::process(double *L, double *R, int numSamples)
{
    for(int i = 0; i < numSamples; i++)
    {
        float x_g = (float)(abs(L[i]) + abs(R[i])) / 2;
        if(x_g < 0.0000001)
        {
            x_g = -120;
        }
        else
        {
            x_g = (float)(20 * log10(x_g / dBBase));
        }
        
        float y_g = 0;
        if(x_g <= thresholdDB)
        {
            y_g = x_g;
        }
        else
        {
            //cout << "x > threshouldDB" << endl;
            y_g = thresholdDB + (x_g - thresholdDB) / compressRatio;
        }
        float x_l = x_g - y_g;
        float y_l = 0;
        if(x_l > yl_prev)
        {
            y_l = a_attack * yl_prev + (1 - a_attack) * x_l;
        }
        else
        {
            y_l = a_release * yl_prev + (1 - a_release) * x_l;
        }

        yl_prev = y_l;
        float c = (float)pow(10, (makeUpGain - y_l) / 20);
        L[i] *= c;
        R[i] *= c;
    }
}
