# CAN-BUS

DBC file Adapted from https://github.com/commaai/opendbc/tree/master/generator/honda


This project aims to log as much filtered packets as possible to SD card. For a full logging this project is not suitable as Ardurino cannot achive high throughput writes on memory card by a limited bandwidth SPI interface. You would need a dedicated SPI port to write in full speed. MCP2515 is fully capable to capture each and every packet at 500kbps but that speed can only be achived by continous pooling of RXB0 and RXB1 registers on MCP2515. It is also possible to log all filtered packets and write to a serial port at 115Kbps if the bus is overloaded with a lot of traffic and skip writing to SD memory card.

One device that can log all packets is Canalyst II which is a chinese USB to CAN BUS interface. It has 2 CAN BUS ports.

In my setup i am using Ardurino Uno, Sunflower CAN Shield v3 and CAN Logging Shield. You would need to disconnect pin 10 on CAN Shield and connect pin 10 to pin 9 on ardurino UNO as pin 10 is also used by logging shield.

Some of the cool things you can get from this project is log the gear shift information and also log individual wheel speeds on Hondas. For other parameters you can use ELM327 side by side on same CAN bus and beaware that ELM327 does polling on CAN BUS insted of asyncronous listening to all raw data that this project can do.


STL files adapted from https://www.thingiverse.com/thing:994827 to extend the concept and create an enclosure for this project.
