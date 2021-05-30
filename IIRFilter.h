#ifndef _IIR_FILTER_H_
#define _IIR_FILTER_H_

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <list>
#include <memory>

using namespace std;

class IIRFilter{
public:
    static const int TYPE_NORMAL = 0;
    static const int TYPE_SOS = 1;

    IIRFilter(double *b, double *a, int len, int maxSamplesPerBuffer);
    IIRFilter(double **sos, double g, int sosRows, int sosCols, int maxSamplesPerBuffer);
    ~IIRFilter();

    int getType();

    void process(double *input, double *output, int samples);

private:
    void updateX(double x);
    void updateY(double y);

    void processNormal(double *input, double *output, int len);
    void processSOS(double *input, double *output, int len);
    double *filter_a = nullptr;
    double *filter_b = nullptr;
    int coefficientCount;

    list<double> xs;
    list<double> ys;

    int maxSamplesPerBuffer;

    double *sosInput = nullptr;
    double *sosOutput = nullptr;

    list<shared_ptr<IIRFilter>> subFilters; 

    double g = 1;

    int filterType = TYPE_NORMAL;

};

#endif