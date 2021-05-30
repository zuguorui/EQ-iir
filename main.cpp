#include <iostream>
#include <memory>
#include <chrono>
#include "Equalizer.h"
#include "WavReader.h"
#include "WavHead.h"
#include "WavWriter.h"

#define __cplusplus 201103L

using namespace std;



int main()
{

    chrono::system_clock::time_point start = chrono::system_clock::now();
    shared_ptr<Equalizer> eq = shared_ptr<Equalizer>(new Equalizer(2, 48000));

    //eq->setTrebleGain(8);
    eq->setBassGain(5);
    //eq->setMidGain(-40);
    //eq->setGain(5);

    shared_ptr<WavReader> reader = WavReader::open("./src.wav");
    if(reader == nullptr)
    {
        cout << "Error, can't open src file" << endl;
        return -1;
    }

    shared_ptr<WavHead> head = reader->getWavHead();
    if(head->numChannels != 2 || head->sampleRate != 48000)
    {
        cout << "Error, src must be stereo and 48000Hz" << endl;
        return -1;
    }

    shared_ptr<WavWriter> writer = WavWriter::open("./dst.wav", head);
    if(writer == nullptr)
    {
        cout << "Error, can't open dst file" << endl;
        return -1;
    }

    int bufferLen = SAMPLES_PER_BUFFER * head->numChannels;
    int16_t *buffer = (int16_t *)malloc(bufferLen * sizeof(int16_t));

    int64_t processedSamples = 0;
    float processPercent = 0.0f;

    int loopCount = 0;
    while(true)
    {
        int readSamples = reader->read(buffer, SAMPLES_PER_BUFFER);
        if(readSamples <= 0)
        {
            break;
        }
        if(loopCount == 1000)
        {
            cout << "Debug point" << endl;
        }
        eq->process(buffer, readSamples);

        int readBytes = readSamples * head->numChannels * sizeof(int16_t);

        writer->write(buffer, readBytes);

        processedSamples += readSamples;
        float newPercent = processedSamples * 1.0f / head->totalSamples;
        if(newPercent - processPercent > 0.01)
        {
            printf("%.2f /%\n", newPercent * 100);
            processPercent = newPercent;
        }
        loopCount++;
    }

    reader.reset();
    writer.reset();
    cout << "complete" << endl;

    chrono::system_clock::time_point end = chrono::system_clock::now();

    chrono::milliseconds duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "use time " << duration.count() << "ms" << endl;

    

    return 0;

}