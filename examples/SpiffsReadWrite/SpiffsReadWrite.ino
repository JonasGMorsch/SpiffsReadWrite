//#define DEBUG
#include <SpiffsReadWrite.h>

void setup()
{
  Serial.begin(2000000);
  delay(2000); // Allow some time for the Serial Monitor to open
  Serial.println();

  Serial.printf("SPIFFS partition start address: %u \n", Spiffs.partition_address);
  Serial.printf("SPIFFS partition size: %ukB or %u words \n", Spiffs.partition_size/1024, Spiffs.partition_size / 4U);

  //Spiffs.format(); // Uncomment to format the entire SPIFFS partition

  uint32_t address_to_rw = 4095; // It can be any value between 0 and partition_size in words

  //   Write to flash
  uint32_t write_data[] = {123123, 456456};
  Spiffs.write(address_to_rw, write_data, sizeof(write_data));

  //   Read to flash
  Serial.printf("From address %u, read 10 itens of SPIFFS partition:\n", address_to_rw);
  uint32_t read_data[10];
  Spiffs.read(address_to_rw, read_data, sizeof(read_data)); // Read the first 10 items starting from address_to_rw
  for (uint32_t i = 0; i < sizeof(read_data) / 4U; i++)
    if ( read_data[i] == UINT32_MAX )
      Serial.print("x,");
    else
      Serial.printf("%u,", read_data[i]);
  Serial.println();
}

void loop() 
{
  
}
