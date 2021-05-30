#include "WavHead.h"

WavHead::WavHead()
{

}

WavHead::~WavHead()
{

}

bool WavHead::parseFromFile(FILE *file)
{
    if(file == nullptr)
    {
        return false;
    }

    int64_t originPos = ftell(file);
    fseek(file, 0, SEEK_SET);


    char fourBytes[4];

    fread(fourBytes, 1, 4, file);
    this->chunkID = string(fourBytes, 4);

    if(this->chunkID.compare("RIFF") != 0)
    {
        cout << "Error, the wave file should begin with \"RIFF\"" << endl;
        fseek(file, originPos, SEEK_SET);
        return false;
    }

    fread(&(this->chunkSize), 1, 4, file);

    fread(fourBytes, 1, 4, file);
    this->format = string(fourBytes, 4);
    if(this->format.compare("WAVE") != 0)
    {
        cout << "Error, this file format is not wave" << endl;
        fseek(file, originPos, SEEK_SET);
        return false;
    }

    
    fread(fourBytes, 1, 4, file);
    this->subChunk1ID = string(fourBytes, 4);

    fread(&(this->subChunk1Size), 1, 4, file);

    // fmt
    int readFmtSize = 0;
    fread(&(this->audioFormat), 1, 2, file);
    fread(&(this->numChannels), 1, 2, file);
    readFmtSize += 4;

    fread(&(this->sampleRate), 1, 4, file);
    readFmtSize += 4;


    fread(&(this->byteRate), 1, 4, file);
    readFmtSize += 4;

    fread(&(this->blockAlign), 1, 2, file);
    fread(&(this->bitsPerSample), 1, 2, file);
    readFmtSize += 4;

    if(readFmtSize != this->subChunk1Size)
    {
        int skipSize = this->subChunk1Size - readFmtSize;
        fseek(file, skipSize, SEEK_CUR);
    }

    while(true)
    {
        fread(fourBytes, 1, 4, file);
        string subChunkName = string(fourBytes, 4);

        int subChunkSize = 0;
        fread(&subChunkSize, 1, 4, file);

        if(subChunkName.compare("data") == 0)
        {
            this->subChunk2ID = subChunkName;
            this->subChunk2Size = subChunkSize;
            break;
        }
        else
        {
            fseek(file, subChunkSize, SEEK_CUR);
        }
    }

    int64_t currentPos = ftell(file);
    this->headTotalSize = currentPos;
    this->totalSamples = this->subChunk2Size / 2 / this->numChannels;
    this->chunkSize = this->subChunk2Size + STANDARD_WAVE_HEAD_LEN - 8;

    //fseek(file, originPos, SEEK_SET);

    return true;
}

bool WavHead::writeToFile(FILE *file)
{
    fwrite(chunkID.c_str(), 1, 4, file);
    fwrite(&(chunkSize), 1, 4, file);
    fwrite(format.c_str(), 1, 4, file);
    fwrite(subChunk1ID.c_str(), 1, 4, file);
    fwrite(&(subChunk1Size), 1, 4, file);
    fwrite(&(audioFormat), 1, 2, file);
    fwrite(&(numChannels), 1, 2, file);
    fwrite(&(sampleRate), 1, 4, file);
    fwrite(&(byteRate), 1, 4, file);
    fwrite(&(blockAlign), 1, 2, file);
    fwrite(&(bitsPerSample), 1, 2, file);
    fwrite(subChunk2ID.c_str(), 1, 4, file);
    fwrite(&(subChunk2Size), 1, 4, file);
    return true;
}