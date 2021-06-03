#include "Equalizer.h"

#define PI 3.1415926

Equalizer::Equalizer(int numChannel, int sampleRate)
{
    this->numChannel = numChannel;
    this->sampleRate = sampleRate;

    generateFilter();

    channelInputs = (double **)malloc(numChannel * sizeof(double *));
    for(int i = 0; i < numChannel; i++)
    {
        channelInputs[i] = (double *)malloc(SAMPLES_PER_BUFFER * sizeof(double));
    }


    compressor = shared_ptr<DynamicCompressor>(new DynamicCompressor(-7, 1, 10, 10, 500, 0, 48000));
    cout << "gain = " << gain << endl;
}

Equalizer::~Equalizer()
{
    if(channelInputs != nullptr)
    {
        for(int i = 0; i < numChannel; i++)
        {
            free(channelInputs[i]);
        }
        free(channelInputs);
    }
}

/*
构造滤波器，尽管左右通道的滤波器参数一样，但是由于滤波器存在缓存的关系，因此
仍然要独立开来。
*/
void Equalizer::generateFilter()
{
    for(int i = 0; i < numChannel; i++)
    {
        channelFilters.push_back(shared_ptr<ChannelFilter>(new ChannelFilter()));
    }

    // 直接形式的滤波器
    for(int i = 0; i < numChannel; i++)
    {
        shared_ptr<ChannelFilter> channelFilter = channelFilters.at(i);
        channelFilter->lowPassFilter = shared_ptr<IIRFilter>(new IIRFilter(LP_B, LP_A, LP_LEN, SAMPLES_PER_BUFFER));
        channelFilter->bandPassFilter = shared_ptr<IIRFilter>(new IIRFilter(BP_B, BP_A, BP_LEN, SAMPLES_PER_BUFFER));
        channelFilter->highPassFilter = shared_ptr<IIRFilter>(new IIRFilter(HP_B, HP_A, HP_LEN, SAMPLES_PER_BUFFER));
    }
    
    // sos形式的滤波器
    // double *paramLP[LP_SOS_ROW];
    // for(int i = 0; i < LP_SOS_ROW; i++)
    // {
    //     paramLP[i] = (double *)malloc(LP_SOS_COL * sizeof(double));
    //     memcpy(paramLP[i], LP_SOS[i], LP_SOS_COL * sizeof(double));
    // }

    // for(int i = 0; i < numChannel; i++)
    // {
    //     shared_ptr<ChannelFilter> channelFilter = channelFilters.at(i);
    //     channelFilter->lowPassFilter = shared_ptr<IIRFilter>(new IIRFilter(paramLP, LP_G, LP_SOS_ROW, LP_SOS_COL, SAMPLES_PER_BUFFER));
    // }

    // double *paramBP[BP_SOS_ROW];
    // for(int i = 0; i < BP_SOS_ROW; i++)
    // {
    //     paramBP[i] = (double *)malloc(BP_SOS_COL * sizeof(double));
    //     memcpy(paramBP[i], BP_SOS[i], BP_SOS_COL * sizeof(double));
    // }

    // for(int i = 0; i < numChannel; i++)
    // {
    //     shared_ptr<ChannelFilter> channelFilter = channelFilters.at(i);
    //     channelFilter->bandPassFilter = shared_ptr<IIRFilter>(new IIRFilter(paramBP, BP_G, BP_SOS_ROW, BP_SOS_COL, SAMPLES_PER_BUFFER));
    // }


    // double *paramHP[HP_SOS_ROW];
    // for(int i = 0; i < HP_SOS_ROW; i++)
    // {
    //     paramHP[i] = (double *)malloc(HP_SOS_COL * sizeof(double));
    //     memcpy(paramHP[i], HP_SOS[i], HP_SOS_COL * sizeof(double));
    // }

    // for(int i = 0; i < numChannel; i++)
    // {
    //     shared_ptr<ChannelFilter> channelFilter = channelFilters.at(i);
    //     channelFilter->highPassFilter = shared_ptr<IIRFilter>(new IIRFilter(paramHP, HP_G, HP_SOS_ROW, HP_SOS_COL, SAMPLES_PER_BUFFER));
    // }


    // for(int i = 0; i < LP_SOS_ROW; i++)
    // {
    //     free(paramLP[i]);
    // }

    // for(int i = 0; i < BP_SOS_ROW; i++)
    // {
    //     free(paramBP[i]);
    // }

    // for(int i = 0; i < HP_SOS_ROW; i++)
    // {
    //     free(paramHP[i]);
    // }



}

void Equalizer::process(int16_t *data, int numSample)
{
    separateChannel(data, numSample);

    for(int i = 0; i < numChannel; i++)
    {
        double *channel = channelInputs[i];
        processChannel(channel, numSample, channelFilters.at(i));
    }

    compressor->process(channelInputs, numChannel, numSample);

    mixChannel(data, numSample);
}

void Equalizer::processChannel(double *channel, int numSample, shared_ptr<ChannelFilter> ChannelFilter)
{
    

    ChannelFilter->lowPassFilter->process(channel, lp_out, numSample);
    ChannelFilter->bandPassFilter->process(channel, bp_out, numSample);
    ChannelFilter->highPassFilter->process(channel, hp_out, numSample);

    for(int i = 0; i < numSample; i++)
    {
        double d = lp_out[i] * bassGain + bp_out[i] * midGain + hp_out[i] * trebleGain;
        channel[i] = d;
    }


    
}


void Equalizer::separateChannel(int16_t *input, int numSample)
{
    for(int i = 0; i < numChannel; i++)
    {
        double *channel = channelInputs[i];
        for(int j = 0; j < numSample; j++)
        {
            channel[j] = input[numChannel * j + i] * gain / INT16_MAX;
        }
    }

}

void Equalizer::mixChannel(int16_t *output, int numSample)
{
    for(int i = 0; i < numChannel; i++)
    {
        double *channel = channelInputs[i];
        for(int j = 0; j < numSample; j++)
        {
            double data = channel[j];
            if(data > 1)
            {
                data = 1;
            }
            else if(data < -1)
            {
                data = -1;
            }
            output[numChannel * j + i] = (int16_t)(data * INT16_MAX);
        }
    }
}

void Equalizer::setBassGain(double gain)
{
    bassGain = pow(10, gain / 20);
}

void Equalizer::setMidGain(double gain)
{
    midGain = pow(10, gain / 20);
}
void Equalizer::setTrebleGain(double gain)
{
    trebleGain = pow(10, gain / 20);
}

void Equalizer::setGain(double gian)
{
    this->gain = pow(10, gain / 20);
}
