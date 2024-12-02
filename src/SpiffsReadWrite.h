#ifndef SpiffsReadWrite
#define SpiffsReadWrite

#include <Arduino.h>
#include <esp_spi_flash.h>
#include <esp_spiffs.h>

class SPIFFS_READ_WRITE
{
  public:
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
    uint32_t partition_address = partition->address;
    uint32_t partition_size = partition->size;

    SPIFFS_READ_WRITE()
    {
      if (partition == NULL)
      {
        Serial.println("No SPIFFS partition found! Maybe not a Esp32?");
        while (1)
          delay(1);
      }
    }

    bool read(uint32_t address, uint32_t *data, uint32_t len)
    {
      if ( address > partition_size / 4U )
      {
        Serial.printf("Address outside SPIFFS partition range (max of %u words) \n", partition_size / 4U);
        return false;
      }

      uint32_t real_address = (address * 4) + partition_address;

#ifdef DEBUG
      Serial.print("real_address:");
      Serial.println(real_address);
#endif

      esp_err_t err = spi_flash_read(real_address, data, len);
      if (err != ESP_OK)
        return false;

      return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    bool write(uint32_t address, uint32_t *data, uint32_t len)
    {
      if ( address > partition_size / 4U )
      {
        Serial.printf("Address outside SPIFFS partition range (max of %u words) \n", partition_size / 4U);
        return false;
      }

      esp_err_t err;
      uint32_t real_address = (address * 4) + partition_address;
      uint32_t data_to_check[len / 4U];
      uint32_t aligned_address_begin = real_address & ~(0xFFF);
      uint32_t flash_n_blocks = ((real_address + len) >> 12) - (real_address >> 12) + 1;
      uint32_t erase = false;

#ifdef DEBUG
      Serial.print("flash_n_blocks: ");
      Serial.println(flash_n_blocks);
#endif

      err = spi_flash_read(real_address, data_to_check, len);
      if (err != ESP_OK)
      {
        Serial.println("spi_flash_read fail");
        return false;
      }

      // Compare if the data arrays are the same
      for (size_t i = 0; i < len / 4U; i++)
        if (data[i] != data_to_check[i])
          erase = true;

      if (erase)
      {
        err = spi_flash_write(real_address, data, len);
        if (err != ESP_OK)
        {
          Serial.println("spi_flash_write fail");
          return false;
        }
      }
      else
      {
#ifdef DEBUG
        Serial.println("Data alreary writen!");
#endif
        return true;
      }

      err = spi_flash_read(real_address, data_to_check, len);
      if (err != ESP_OK)
      {
        Serial.println("spi_flash_read fail");
        return false;
      }

      // Compare if the data arrays are the same
      erase = false;
      for (size_t i = 0; i < len / 4U; i++)
        if (data[i] != data_to_check[i])
          erase = true;

      if (erase)
      {
        uint32_t flash_4kb_block[1024 * flash_n_blocks] = {0};
        err = spi_flash_read(aligned_address_begin, flash_4kb_block, 4096 * flash_n_blocks);
        if (err != ESP_OK)
        {
          Serial.println("spi_flash_read fail");
          return false;
        }

#ifdef DEBUG
        Serial.print("data: ");
        for (uint32_t i = 0; i < len / 4U; i++)
          Serial.printf(" %u,", data[i]);
        Serial.println();

        Serial.print("data_to_check: ");
        for (uint32_t i = 0; i < sizeof(data_to_check) / 4U; i++)
          Serial.printf(" %u,", data_to_check[i]);
        Serial.println();

        Serial.print("erase:");
        Serial.println(erase);
#endif

        err = spi_flash_erase_range(aligned_address_begin, SPI_FLASH_SEC_SIZE * flash_n_blocks);
        if (err != ESP_OK)
        {
          Serial.println("spi_flash_erase_range fail");
          return false;
        }

#ifdef DEBUG
        Serial.print("flash_4kb_block 1: ");
        for (uint32_t i = 0; i < sizeof(flash_4kb_block) / 4U; i++)
          Serial.printf(" %u,", flash_4kb_block[i]);
        Serial.println();
#endif

        memcpy(&flash_4kb_block[address % (1024 * flash_n_blocks)], data, len);

#ifdef DEBUG
        Serial.print("flash_4kb_block 2: ");
        for (uint32_t i = 0; i < sizeof(flash_4kb_block) / 4U; i++)
          Serial.printf(" %u,", flash_4kb_block[i]);
        Serial.println();
#endif

        err = spi_flash_write(aligned_address_begin, flash_4kb_block, 4096 * flash_n_blocks);
        if (err != ESP_OK)
        {
          Serial.println("spi_flash_write fail");
          return false;
        }
      }

      return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    bool format()
    {
      esp_err_t err = spi_flash_erase_range(partition_address, partition_size);
      if (err != ESP_OK)
      {
        Serial.println("spi_flash_erase_range fail");
        return false;
      }
      return true;
    }

}; // SPIFFS_READ_WRITE

SPIFFS_READ_WRITE Spiffs;

#endif
