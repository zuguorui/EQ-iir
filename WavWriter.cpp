#include "WavWriter.h"

WavWriter::WavWriter()
{

}

WavWriter::~WavWriter()
{
    if(file != nullptr)
    {
        fclose(file);
    }
    wavHead.reset();
}

shared_ptr<WavWriter> WavWriter::open(string path, shared_ptr<WavHead> wavHead)
{
    if(wavHead == nullptr)
    {
        return nullptr;
    }
    FILE *file = fopen(path.c_str(), "wb");
    if(file == nullptr)
    {
        return nullptr;
    }

    shared_ptr<WavWriter> writer(new WavWriter());
    writer->wavHead = wavHead;
    writer->file = file;

    wavHead->writeToFile(file);

    return writer;
}

void WavWriter::write(void *buffer, int sizeBytes)
{
    if(file == nullptr || wavHead == nullptr)
    {
        return;
    }
    fwrite(buffer, 1, sizeBytes, file);
    fflush(file);
}
