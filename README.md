# SpiffsReadWrite
This is a library for the ESP32 internal flash storage, that have multiple partitions, including a very useful one: 

The SPIFFS partition

This partition can be used to store user data. Traditionally, access to it is done via the SPIFFS/LittleFS filesystem, but this library enables EEPROM-like access to the storage, reducing overhead and read time for certain applications.

It runs on ESP32 devices using the Arduino Core:

![Example](https://github.com/JonasGMorsch/SpiffsReadWrite/blob/main/example.png)

## Installing this Library

https://www.arduino.cc/en/Guide/Libraries

## How to Use

Go to [Example](https://github.com/JonasGMorsch/SpiffsReadWrite/blob/main/examples/SpiffsReadWrite/SpiffsReadWrite.ino) or:

1 - ```#include <SpiffsReadWrite.h>```

2 - call ```Spiffs.write(address,data,sizeof(data));``` to write to flash

3 - call ```Spiffs.read(address,data,sizeof(data));``` to read flash

4 - call ```Spiffs.format();``` to format SPIFFS partition

**Be Aware:**  Flash memory is a wear-prone component; avoid writing to the same 4kB block unnecessarily.

## Increassing SPIFFS partition
If you don't plan to use OTA to upload code, you can modify the ESP32 partition scheme:

![partion](https://github.com/JonasGMorsch/SpiffsReadWrite/blob/main/partitions.png)
