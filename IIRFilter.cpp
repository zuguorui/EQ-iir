#include "IIRFilter.h"
#include <assert.h>
#include <chrono>

IIRFilter::IIRFilter(double *b, double *a, int len, int maxSamplesPerBuffer)
{
    filterType = TYPE_NORMAL;
    this->coefficientCount = len;
    this->maxSamplesPerBuffer = maxSamplesPerBuffer;

    sosInput = (double *)malloc(maxSamplesPerBuffer * sizeof(double));
    sosOutput = (double *)malloc(maxSamplesPerBuffer * sizeof(double));

    filter_b = (double *)malloc(coefficientCount * sizeof(double));
    memcpy(filter_b, b, coefficientCount * sizeof(double));

    filter_a = (double *)malloc((coefficientCount - 1) * sizeof(double));
    memcpy(filter_a, a + 1, (coefficientCount - 1) * sizeof(double));

    for(int i = 0; i < coefficientCount; i++)
    {
        xs.push_back(0);
    }

    for(int i = 0; i < coefficientCount - 1; i++)
    {
        ys.push_back(0);
    }
}

IIRFilter::IIRFilter(double **sos, double g, int sosRows, int sosCols, int maxSamplesPerBuffer)
{
    filterType = TYPE_SOS;
    this->g = g;
    this->maxSamplesPerBuffer = maxSamplesPerBuffer;

    sosInput = (double *)malloc(maxSamplesPerBuffer * sizeof(double));
    sosOutput = (double *)malloc(maxSamplesPerBuffer * sizeof(double));

    int coefficientCount = sosCols / 2;

    for(int i = 0; i < sosRows; i++)
    {
        double *ba = sos[i];
        double *b = (double *)malloc(coefficientCount * sizeof(double));
        memcpy(b, ba, coefficientCount * sizeof(double));
    
        double *a = (double *)malloc(coefficientCount * sizeof(double));
        memcpy(a, ba + coefficientCount, coefficientCount * sizeof(double));

        shared_ptr<IIRFilter> filter(new IIRFilter(b, a, coefficientCount, maxSamplesPerBuffer));
        subFilters.push_back(filter);
    }
}

IIRFilter::~IIRFilter()
{
    if(sosInput != nullptr)
    {
        free(sosInput);
    }

    if(sosOutput != nullptr)
    {
        free(sosOutput);
    }
}

void IIRFilter::updateX(double x){
    xs.push_front(x);
    xs.pop_back();
}

void IIRFilter::updateY(double y)
{
    ys.push_front(y);
    ys.pop_back();
}

void IIRFilter::process(double *input, double *output, int samples)
{
    if(filterType == TYPE_NORMAL)
    {
        processNormal(input, output, samples);
    }
    else{
        processSOS(input, output, samples);
    }
}

void IIRFilter::processNormal(double *input, double *output, int samples)
{

    chrono::system_clock::time_point start = chrono::system_clock::now();

    for(int i = 0; i < samples; i++)
    {
        updateX(input[i]);
        double feedforwardSum = 0;
        int filterIndex = 0;
        for(double x : xs)
        {
            double f = filter_b[filterIndex];
            feedforwardSum += (f * x);
            filterIndex++;
        }

        filterIndex = 0;
        double feedbackSum = 0;
        for(double y : ys)
        {
            double f = filter_a[filterIndex];
            feedbackSum += (f * y);
            filterIndex++;
        }

        double d = feedforwardSum - feedbackSum;
        output[i] = d;
        updateY(d);
    }

    chrono::system_clock::time_point end = chrono::system_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end - start);
    //cout << "normal process " << samples << "samples use " << duration.count() << "us" << endl;
}


void IIRFilter::processSOS(double *input, double *output, int samples)
{

    assert(samples <= maxSamplesPerBuffer);

    chrono::system_clock::time_point start = chrono::system_clock::now();
    
    memcpy(sosInput, input, samples * sizeof(double));

    for(shared_ptr<IIRFilter> filter : subFilters)
    {
        filter->process(sosInput, sosOutput, samples);
        double *temp = sosInput;
        sosInput = sosOutput;
        sosOutput = temp;
    }

    for(int i = 0; i < samples; i++)
    {
        output[i] = sosInput[i] * g;
    }

    chrono::system_clock::time_point end = chrono::system_clock::now();
    chrono::microseconds duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "SOS process " << samples << "samples use " << duration.count() << "us" << endl;
}