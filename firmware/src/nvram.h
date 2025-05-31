#ifndef NVRAM_H
#define NVRAM_H

#include <Arduino.h>
#include <SPIFFS.h>
#include "types.h"
#include "debug.h"

// #define NVRAM_DEBUG
// #define NVRAM_FORMAT

#define NVRAM_SIZE 512

class NVRAM
{
private:
    uint m_address;
    uint8_t *m_dataPtr;
    uint m_length;

    const char *NVRAM_DATA_FILE_NAME = "/nvram.bin";
    const char *NVRAM_CRC_FILE_NAME = "/nvram_crc.bin";

    uint16_t calculateCRC(uint8_t *dataPtr, uint length);
    bool deleteNVRAMFile(const char *fileName);
    bool writeNVRAMFile(const char *fileName, uint8_t *dataPtr, uint length);
    bool readNVRAMFile(const char *fileName, uint8_t *dataPtr, uint length);

public:
    NVRAM(uint address, uint8_t *dataPtr, uint length);
    bool initNVRAM();
    bool readNVRAM();
    bool writeNVRAM();
    void clearNVRAM();
};

#endif