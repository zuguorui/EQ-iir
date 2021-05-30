#include "WavReader.h"

WavReader::WavReader()
{
}

WavReader::~WavReader()
{
    if(file != nullptr)
    {
        fclose(file);
    }
    wavHead.reset();
}

shared_ptr<WavReader> WavReader::open(string path)
{
    FILE *file = fopen(path.c_str(), "rb");
    if(file == nullptr)
    {
        return nullptr;
    }
    shared_ptr<WavHead> head(new WavHead());
    bool succ = head->parseFromFile(file);
    if(!succ)
    {
        head.reset();
        fclose(file);
        return nullptr;
    }
    shared_ptr<WavReader> reader = shared_ptr<WavReader>(new WavReader());
    reader->file = file;
    reader->wavHead = head;
    return reader;
}

shared_ptr<WavHead> WavReader::getWavHead()
{
    return wavHead;
}

int WavReader::read(void *buffer, int numSamples)
{
    if(wavHead == nullptr || file == nullptr)
    {
        return -1;
    }

    int numBytes = numSamples * wavHead->bitsPerSample * wavHead->numChannels / 8;
    int readBytes = fread(buffer, 1, numBytes, file);

    if(readBytes <= 0)
    {
        return 0;
    }
    int readSamples = readBytes * 8 / (wavHead->bitsPerSample * wavHead->numChannels);
    return readSamples;

}