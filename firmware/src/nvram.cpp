#include "nvram.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////

NVRAM::NVRAM(uint address, uint8_t *dataPtr, uint length)
{
    m_address = address;
    m_dataPtr = dataPtr;
    m_length = length;
}

///////////////////////////////////////////////////////////////////////////////
// Public methods
///////////////////////////////////////////////////////////////////////////////

bool NVRAM::initNVRAM()
{
#ifdef NVRAM_FORMAT
    // Format SPIFFS
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Formatting SPIFFS...");
    if (!SPIFFS.format())
    {
        DEBUG_PRINTLN("An error occurred while formatting SPIFFS");
        return false;
    }
    delay(1000);
#endif

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error has occurred while mounting SPIFFS");
        delay(1000);
        return false;
    }

    return true;
}

bool NVRAM::readNVRAM()
{
    // Read the NVRAM data
    if (!readNVRAMFile(NVRAM_DATA_FILE_NAME, m_dataPtr, m_length))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error occurred while reading the NVRAM data");
        delay(1000);
        return false;
    }

#ifdef NVRAM_DEBUG
    // Print the NVRAM data
    DEBUG_PRINTLN("NVRAM data:");
    for (int i = 0; i < m_length; ++i)
    {
        // Print each byte and their index
        DEBUG_PRINT("Index: ");
        DEBUG_PRINT(i);
        DEBUG_PRINT(" DATA: ");
        DEBUG_PRINT(m_dataPtr[i]);
        DEBUG_PRINTLN();
    }
    DEBUG_PRINTLN();
#endif

    // Read the NVRAM CRC
    uint8_t crcData[2];
    if (!readNVRAMFile(NVRAM_CRC_FILE_NAME, crcData, 2))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error occurred while reading the NVRAM CRC");
        delay(1000);
        return false;
    }

    // Calculate the CRC of the NVRAM data
    uint16_t crc = calculateCRC(m_dataPtr, m_length);

#ifdef NVRAM_DEBUG
    // Print the NVRAM CRC
    DEBUG_PRINT("NVRAM CRC: ");
    DEBUG_PRINTLN(crc);
    // Print the calculated CRC
    DEBUG_PRINT("Calculated CRC: ");
    DEBUG_PRINTLN(crcData[0] << 8 | crcData[1]);
#endif

    // Compare the calculated CRC with the read CRC
    if (crc != (crcData[0] << 8 | crcData[1]))
    {
        DEBUG_PRINTLN("The NVRAM data is corrupted");
        return false;
    }

    return true;
}

bool NVRAM::writeNVRAM()
{
    // Write the NVRAM data
    if (!writeNVRAMFile(NVRAM_DATA_FILE_NAME, m_dataPtr, m_length))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error occurred while writing the NVRAM data");
        delay(1000);
        return false;
    }

    // Calculate the CRC of the NVRAM data
    uint16_t crc = calculateCRC(m_dataPtr, m_length);

    // Write the NVRAM CRC
    uint8_t crcData[2] = {static_cast<uint8_t>(crc >> 8), static_cast<uint8_t>(crc & 0xFF)};
    if (!writeNVRAMFile(NVRAM_CRC_FILE_NAME, crcData, 2))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error occurred while writing the NVRAM CRC");
        delay(1000);
        return false;
    }

#ifdef NVRAM_DEBUG
    // Read entire data from NVRAM and compare with the dataPtr byte by byte
    uint8_t dataPtrRead[NVRAM_SIZE];
    if (!readNVRAMFile(NVRAM_DATA_FILE_NAME, dataPtrRead, m_length))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error occurred while reading the NVRAM data");
        delay(1000);
        return false;
    }
    // Compare the dataPtr with the dataPtrRead
    DEBUG_PRINTLN("Comparing the NVRAM data with the dataPtr");
    for (int i = 0; i < m_length; ++i)
    {
        // Print each byte and their index
        DEBUG_PRINT("Index: ");
        DEBUG_PRINT(i);
        DEBUG_PRINT(": DATA: ");
        DEBUG_PRINT(m_dataPtr[i]);
        DEBUG_PRINT(" NVRAM: ");
        DEBUG_PRINTLN(dataPtrRead[i]);
        // Compare each byte
        if (m_dataPtr[i] != dataPtrRead[i])
        {
            // If the bytes are different, print an error message and index
            DEBUG_PRINTLN("The NVRAM data is corrupted");
            DEBUG_PRINTLN(i);
            return false;
        }
    }

    // Read the NVRAM CRC and compare with the calculated CRC
    uint8_t crcDataRead[2];
    if (!readNVRAMFile(NVRAM_CRC_FILE_NAME, crcDataRead, 2))
    {
        DEBUG_PRINTLN();
        DEBUG_PRINTLN("An error occurred while reading the NVRAM CRC");
        delay(1000);
        return false;
    }
    // Compare the calculated CRC with the read CRC
    DEBUG_PRINTLN()
    DEBUG_PRINTLN("Calculated CRC: ");
    DEBUG_PRINTLN(crc);
    DEBUG_PRINTLN("NVRAM CRC: ");
    DEBUG_PRINTLN(crcDataRead[0] << 8 | crcDataRead[1]);
    if (crc != (crcDataRead[0] << 8 | crcDataRead[1]))
    {
        DEBUG_PRINTLN("The NVRAM data is corrupted");
        delay(1000);
        return false;
    }
#endif

    return true;
}

void NVRAM::clearNVRAM()
{
    deleteNVRAMFile(NVRAM_DATA_FILE_NAME);
    deleteNVRAMFile(NVRAM_CRC_FILE_NAME);
}

///////////////////////////////////////////////////////////////////////////////
// Private methods
///////////////////////////////////////////////////////////////////////////////

uint16_t NVRAM::calculateCRC(uint8_t *dataPtr, uint length)
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < length; ++i)
    {
        crc ^= dataPtr[i];
        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

bool NVRAM::deleteNVRAMFile(const char *fileName)
{
    if (SPIFFS.exists(fileName))
    {
        if (!SPIFFS.remove(fileName))
        {
            DEBUG_PRINTLN();
            DEBUG_PRINTLN("An error occurred while deleting the file");
            delay(1000);
            return false;
        }
    }
    return true;
}

bool NVRAM::writeNVRAMFile(const char *fileName, uint8_t *dataPtr, uint length)
{
    // Open file for writing
    File file = SPIFFS.open(fileName, FILE_WRITE);
    if (!file)
    {
        DEBUG_PRINTLN("Failed to open file for writing");
        return false;
    }

    // Write the NVRAM data
    for (int i = 0; i < length; ++i)
    {
        file.write(dataPtr[i]);
    }
    file.close();

    return true;
}

bool NVRAM::readNVRAMFile(const char *fileName, uint8_t *dataPtr, uint length)
{
    // Open file for reading
    File file = SPIFFS.open(fileName);
    if (!file)
    {
        DEBUG_PRINTLN("Failed to open file for reading");
        return false;
    }

    // Read the data into the m_dataPtr array
    for (int i = 0; i < length; ++i)
    {
        dataPtr[i] = file.read();
    }
    file.close();

    return true;
}