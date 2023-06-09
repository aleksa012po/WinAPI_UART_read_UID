#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <conio.h>
#include <fileapi.h>

#define UART_COM_PORT "COM18"
#define MAX_TAG_LENGTH 16                                                                                       // BOOTLOADER ACK (4 bytes) + RESPONSE (7 bytes) + DATA (4 bytes) + CHECKSUM byte (1 byte)
#define RFID_ACTIVATION_SEQUENCE_LENGTH 7

int main()
{
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    BOOL bStatus;
    DWORD dwBytesRead;
    DWORD dwBytesWritten;
    COMMTIMEOUTS timeout;
    unsigned char NFC_Tag[MAX_TAG_LENGTH];
    char GET_CARD_ID[RFID_ACTIVATION_SEQUENCE_LENGTH] = {0x55, 0x13, 0xAA, 0x00, 0x00, 0x00, 0xF3};             // GET_CARD_ID (0x13)
    char GET_DLOGIC_CARD_TYPE[RFID_ACTIVATION_SEQUENCE_LENGTH] = {0x55, 0x3C, 0xAA, 0x00, 0x00, 0x00, 0xCA};    // GET_DLOGIC_CARD_TYPE (0x3C)
    char USER_INTERFACE_SIGNAL[RFID_ACTIVATION_SEQUENCE_LENGTH] = {0x55, 0x26, 0xAA, 0x00, 0x01, 0x01, 0xE0};   // USER_INTERFACE_SIGNAL (0x26)

    // Open the serial port
    hSerial = CreateFile("\\\\.\\COM18", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        printf("Error opening %s\n", UART_COM_PORT);
        return 1;
    }
    else
        printf("----------------------------\nSuccessfully connected to %s\n", UART_COM_PORT);

    // Configure the serial port
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    bStatus = GetCommState(hSerial, &dcbSerialParams);
    if (bStatus == FALSE)
    {
        printf("Error setting up UART parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
    else
        printf("----------------------------\nSuccessfully setting up UART parameters\n----------------------------\n");

    GetCommTimeouts(hSerial, &timeout);
    timeout.ReadIntervalTimeout = 500;
    timeout.ReadTotalTimeoutConstant = 500;
    timeout.ReadTotalTimeoutMultiplier = 1;

    timeout.WriteTotalTimeoutConstant = 500;
    timeout.WriteTotalTimeoutMultiplier = 1;

    SetCommTimeouts(hSerial, &timeout);

    GetCommState(hSerial, &dcbSerialParams);

    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fRtsControl = RTS_CONTROL_ENABLE;               // RTS (Request to Send)
    bStatus = SetCommState(hSerial, &dcbSerialParams);
    if (bStatus == FALSE)
    {
        printf("Failed to set serial parameters.\n");
        CloseHandle(hSerial);
        return 1;
    }
    else
            printf("****************************\n*   Opening RFID program   *\n****************************\n");
            printf("\n");


    // Send the GET_CARD_ID or GET_CARD_ID_EX activation sequence to the RFID RC232
        printf("Enter the number of the command to send to the device: \n\n1) GET_CARD_ID (0x13) \n2) GET_DLOGIC_CARD_TYPE (0x3C)\n3) USER_INTERFACE_SIGNAL (0x26) \n4) EXIT \n");
        int commandChoice;
        printf("\n");
        printf("Please insert the right command: ");
        scanf("%d", &commandChoice);
        printf("\n");

        if (commandChoice == 1)
        {
            bStatus = WriteFile(hSerial, GET_CARD_ID, RFID_ACTIVATION_SEQUENCE_LENGTH, &dwBytesWritten, NULL);
        }
        else if (commandChoice == 2)
        {
            bStatus = WriteFile(hSerial, GET_DLOGIC_CARD_TYPE, RFID_ACTIVATION_SEQUENCE_LENGTH, &dwBytesWritten, NULL);
        }
        else if (commandChoice == 3)
        {
            bStatus = WriteFile(hSerial, USER_INTERFACE_SIGNAL, RFID_ACTIVATION_SEQUENCE_LENGTH, &dwBytesWritten, NULL);

            printf("BEEP\n");
        }
        else if (commandChoice == 4)
        {
            printf("Exiting the program...\n");
            CloseHandle(hSerial);
        }
        else
        {
            printf("Invalid command choice. Exiting program.\n");
            CloseHandle(hSerial);
            return 1;
        }

        if (bStatus == FALSE || dwBytesWritten != RFID_ACTIVATION_SEQUENCE_LENGTH)
        {
            printf("Failed to send the activation sequence to the RFID RC232.\n");
            CloseHandle(hSerial);
            return 1;
        }

        // Read the NFC tag from the serial port
        bStatus = ReadFile(hSerial, NFC_Tag, MAX_TAG_LENGTH, &dwBytesRead, NULL);
        if (bStatus == FALSE)
        {
            printf("Failed to read from serial port.\n");
            CloseHandle(hSerial);
            return 1;
        }

        // Only display the message if a tag has been successfully read
        if (dwBytesRead == MAX_TAG_LENGTH)
        {
            // Print the NFC tag
            printf("Card UID is: ");
            for (int i = 0; i < dwBytesRead; i++)
            {
                printf("%02X ", NFC_Tag[i]);
                if (i == 3 || i == 10 || i == 14)
                {
                    printf("| ");
                }
            }
            printf("\n\nWhere first 4 bytes are BOOTLOADER ACK: ");
            for (int i = 0; i < 4; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
            printf("Second 7 bytes are RESPONSE: ");
            for (int i = 4; i < 11; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
            printf("Third 4 bytes are DATA: ");
            for (int i = 11; i < 15; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
            printf("Fourth 1 byte is CHECKSUM byte: ");
            for (int i = 15; i < 16; i++)
            {
                printf("%02X ", NFC_Tag[i]);
            }
            printf("\n\n");
        }
            else
                {
                    printf("No tag detected.\n");
                }

        // Close the serial port
        CloseHandle(hSerial);

        return 0;
}
