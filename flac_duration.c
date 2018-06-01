#include <stdio.h>
#include <stdint.h>

#define MAX_FILE_SIZE 100

// Document https://xiph.org/flac/format.html

int main()
{
    FILE *pFile;
    unsigned char buffer[MAX_FILE_SIZE];
    int n;
    int i = 0;

    pFile = fopen("Ready_For_It.flac", "rb");
    if(!pFile) {
        fprintf(stderr, "Cannot open file \n");
        return 1;
    }

    n = fread(buffer, MAX_FILE_SIZE, 1, pFile);
    if(n != 1 ) {
        fprintf(stderr, "Cannot read file \n");
        return 1;
    }

    // 4 first bytes --> must is fLaC
    if(buffer[i] != 'f' || buffer[i+1] != 'L' || buffer[i+2] != 'a' || buffer[i+3] != 'C'){
        fprintf(stderr, "This is not FLAC file \n");
        return 1;
    }
    i = 4;

    // Parse METADATA_BLOCK_HEADER
    int block_type;
    block_type = buffer[i] & 0x7f; //0x0111 1111
    if(block_type != 0) {
        fprintf(stderr, "This file doesnt contain stream info -> Cannot read duration \n");
        return 1;
    }
    // skip 4 bytes of metadata block header
    i += 4;

    // skip 10 bytes of block size and frame size
    i += 10;

    //get sample rate, 20 bits 
    uint32_t sample_rate;

    sample_rate = buffer[i];
    sample_rate = (sample_rate << 8) | buffer[i +1];
    sample_rate = (sample_rate << 4) | (buffer[i+2] >> 4) ;
    // skip 3 bytes including 20 bits of sample_rate, 3 bit of number chanels, 1 bit of bits per sample
    // still have 4 bits of bits per sample
    i += 3;

    //get total sample, 36 bits
    uint64_t total_sample;

    total_sample = buffer[i] & 0x0f; //skip 4 bits of bits per sample
    total_sample = (total_sample << 8) | buffer[i+1];
    total_sample = (total_sample << 8) | buffer[i+2];
    total_sample = (total_sample << 8) | buffer[i+3];
    total_sample = (total_sample << 8) | buffer[i+4];
    
    //finally, caculate duration
    float duration = (float) (total_sample/ sample_rate);

    fprintf(stdout, "sample rate = %u, total sample = %lu \n", sample_rate, total_sample);

    fprintf(stdout, "=================================================================== \n");
    fprintf(stdout, "================ Duration = %f \n", duration);
    fprintf(stdout, "=================================================================== \n");

    return 0;
}

