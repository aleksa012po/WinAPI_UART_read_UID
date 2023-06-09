RFID RC232
This program is designed to read the NFC tags using RFID RC232.

Getting Started
The following instructions will help you get a copy of the project up and running on your local machine.

Prerequisites
- Windows Operating System
- Microsoft Visual Studio
- RFID RC232 device connected to COM13 port

Installing
1. Clone this repository.
2. Open RFID_RC232.sln file in Microsoft Visual Studio.
3. Compile and run the application.

Running the Program
When you run this program, it will open a connection between your computer and an RFID RC232 device connected to COM13 port.
The program uses UART communication protocol for serial communication with the device.
After opening a connection with the device, it sends commands like GET_CARD_ID or GET_CARD_ID_EX activation sequence to
read NFC tags from its memory by reading bytes from UART buffer.
Then, it reads data in chunks of 16 bytes (where first 4 are BOOTLOADER ACK, second 7 are RESPONSE, third 4 are DATA & 
fourth one is CHECKSUM byte) sent back by RFID reader and displays them as output on console window.

Built With
- C programming language

Authors
aleksa012po 
