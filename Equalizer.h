#ifndef _EQUALIZER_H_
#define _EQUALIZER_H_


#include <iostream>
#include <stdlib.h>
#include <memory>
#include <stdint.h>
#include <cmath>
#include <vector>
#include "IIRFilter.h"
#include "DynamicCompressor.h"

using namespace std;

/**
 * 声道道均衡器，我们为每个声道都要建立一个均衡器
 * 
*/
class ChannelFilter
{
public:
    shared_ptr<IIRFilter> lowPassFilter;
    shared_ptr<IIRFilter> bandPassFilter;
    shared_ptr<IIRFilter> highPassFilter;

    ~ChannelFilter()
    {
        lowPassFilter.reset();
        bandPassFilter.reset();
        highPassFilter.reset();
    }
};

// 正常形式的低通滤波器系数
static const int LP_LEN = 6;
static double LP_B[LP_LEN] = {0.00112266916511878,	-0.00335804481007404,	0.00223539333478608,	0.00223539333478608,	-0.00335804481007404,	0.00112266916511878};
static double LP_A[LP_LEN] = {1, -4.89871336422332,	9.59996411771510,	-9.40745163011783,	4.60986753597724,	-0.903666623971535};

// 正常形式的带通滤波器系数
static const int BP_LEN = 11;
static double BP_B[BP_LEN] = {0.0147870348410641,	-0.0896713907921595,	0.239728170511116,	-0.357933513532618,	0.281432905082980,	-1.31335252371870e-17,	-0.281432905082980,	0.357933513532618,	-0.239728170511116,	0.0896713907921597,	-0.0147870348410641};
static double BP_A[BP_LEN] = {1, -8.24143236474598,	30.6837048158386,	-67.9994348029134,	99.3786343355872,	-100.107651076046,	70.4028934601109,	-34.1345683881885,	10.9193043553503,	-2.08073159058107,	0.179281256060903};

// 正常形式的高通滤波器系数
static const int HP_LEN = 6;
static double HP_B[HP_LEN] = {0.512554073443323,	-2.49431022188030,	4.92200130950305,	-4.92200130950306,	2.49431022188030,	-0.512554073443323};
static double HP_A[HP_LEN] = {1, -3.56799305163320,	5.31535809874652,	-4.09208228010496,	1.61958624530545,	-0.262711533863224};

// 级联形式的低通滤波器参数
static const int LP_SOS_ROW = 3;
static const int LP_SOS_COL = 6;
static const double LP_SOS[LP_SOS_ROW][LP_SOS_COL] = {
    {1,	1.00000000000000,	0,	1,	-0.963547075321816,	0},
    {1,	-1.99358039818411,	0.999999999999900,	1,	-1.95062963869978,	0.951737145504358},
    {1,	-1.99754549571959,	1.00000000000010,	1,	-1.98453665020172,	0.985412994665244}
};
static const double LP_G = 0.00112266916511878;

// 级联形式的带通滤波器参数
static const int BP_SOS_ROW = 5;
static const int BP_SOS_COL = 6;
static const double BP_SOS[BP_SOS_ROW][BP_SOS_COL] = {
    {1,	5.44743089525568e-08,	-0.999999945525691,	1,	-1.50598726858070,	0.518282819067758},
    {1,	-0.700072243318383,	1.00000000000000,	1,	-1.23158596537784,	0.456085198859544},
    {1,	-1.36460554640996,	0.999999999999997,	1,	-1.55756736890671,	0.799625176366307},
    {1,	-1.99986269254609,	1.00000008963480,	1,	-1.96057569962627,	0.961352149408521},
    {1,	-1.99964970605548,	0.999999964839528,	1,	-1.98571606225446,	0.986627313688261}
};
static const double BP_G = 0.0147870348410641;

// 级联形式的高通滤波器参数
static const int HP_SOS_ROW = 3;
static const int HP_SOS_COL = 6;
static const double HP_SOS[HP_SOS_ROW][HP_SOS_COL] = {
    {1,	-0.999999999999545,	0,	1,	-0.653466881779375,	0},
    {1,	-1.96268149824057,	1.00000000000073,	1,	-1.35970418742341,	0.512933994644893},
    {1,	-1.90375182287976,	0.999999999999727,	1,	-1.55482198243042,	0.783779815814256}
};
static const double HP_G = 0.512554073443323;


// 每次处理最大帧数
static const int SAMPLES_PER_BUFFER = 1024;


/**
 * 均衡器
*/
class Equalizer
{

public:
    /**
     * 构造函数
     * @param numChannel: 声道数
     * @param sampleRate: 采样率
     * 
    */
    Equalizer(int numChannel, int sampleRate);
    ~Equalizer();

    // 构造滤波器组
    void generateFilter();

    /**
     * 处理数据
     * @param data 数据buffer，处理结果会被存放在这里
     * @param numSample data中的音频帧数
    */
    void process(int16_t *data, int numSample);

    // 设置低中高频的增益，单位dB
    void setBassGain(double gain);
    void setMidGain(double gain);
    void setTrebleGain(double gain);

    // 设置总增益
    void setGain(double gain);



private:
    int numChannel;
    int sampleRate;
    double bassGain = 1.0;
    double midGain = 1.0;
    double trebleGain = 1.0;
    double gain = pow(10, 6.0 / 20);

    int inputSamples = 0;

    int processBandTimes = 0;

    // 每个通道的数据
    double **channelInputs;

    double lp_out[SAMPLES_PER_BUFFER];
    double bp_out[SAMPLES_PER_BUFFER];
    double hp_out[SAMPLES_PER_BUFFER];

    vector<shared_ptr<ChannelFilter>> channelFilters;
    // 动态压缩器，用以防止数据溢出导致的破音
    shared_ptr<DynamicCompressor> compressor;

    /**
     * 分离通道数据
    */
    void separateChannel(int16_t *input, int numSample);
    /**
     * 混合通道数据
    */
    void mixChannel(int16_t *output, int numSample);

    /**
     * 处理通道数据
    */
    void processChannel(double *channel, int numSample, shared_ptr<ChannelFilter> ChannelFilter);

};

#endif