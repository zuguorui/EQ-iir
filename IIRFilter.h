#ifndef _IIR_FILTER_H_
#define _IIR_FILTER_H_

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <list>
#include <memory>

using namespace std;


/**
 * IIR滤波器
 * 
*/
class IIRFilter{
public:
    // 滤波器类型，TYPE_NORMAL代表基本形态的滤波器，TYPE_SOS代表级联形式的滤波器，它内部包含多个基本滤波器
    static const int TYPE_NORMAL = 0;
    static const int TYPE_SOS = 1;

    /**
     * 基本形态滤波器的构造函数
     * @param b: 滤波器中x的系数
     * @param a: 滤波器中y的系数
     * @param len: a和b的长度
     * @param maxCountPerBuffer: 每次处理的最长buffer大小，用来创建缓存用，避免频繁申请和释放内存
    */
    IIRFilter(double *b, double *a, int len, int maxCountPerBuffer);

    /**
     * sos级联形式滤波器的构造函数
     * @param sos: sos矩阵
     * @param g: 滤波器总增益系数
     * @param sosRows: sos矩阵行数，相当于级联滤波器的个数
     * @param sosCols: sos矩阵列数
     * @param maxCountPerBuffer: 每次处理的最长buffer大小，用来创建缓存用，避免频繁申请和释放内存
     * 
    */
    IIRFilter(double **sos, double g, int sosRows, int sosCols, int maxCountPerBuffer);
    ~IIRFilter();

    /**
     * 获取滤波器类型，是基本型还是级联型
    */
    int getType();

    /**
     * 处理数据，双声道
     * @param input: 输入buffer
     * @param output: 输出buffer
     * @param samples: buffer有多少帧，对于双声道数据，一帧包含左右声道两个double
     * 
    */
    void process(double *input, double *output, int len);

private:
    // 更新滤波器中的x缓存
    void updateX(double x);
    // 更新滤波器的y缓存
    void updateY(double y);

    void processNormal(double *input, double *output, int len);
    void processSOS(double *input, double *output, int len);

    // 滤波器中的系数组a
    double *filter_a = nullptr;

    // 滤波器中的系数组b
    double *filter_b = nullptr;

    // 滤波器中的系数个数
    int coefficientCount;

    // x和y缓存，因为经常要将新值push进去，所以采用list实现
    list<double> xs;
    list<double> ys;

    int maxCountPerBuffer;

    // 级联形式的缓存
    double *sosInput = nullptr;
    double *sosOutput = nullptr;

    // 级联形式下，级联滤波器里包含多个低阶滤波器
    list<shared_ptr<IIRFilter>> subFilters; 

    // 级联滤波器的总增益系数
    double g = 1;

    int filterType = TYPE_NORMAL;

};

#endif