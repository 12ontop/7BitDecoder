#include <stdint.h>
#include <stdio.h>

#define FRAME_SIZE  7           // Number of bytes per frame
#define BUFFER_SIZE 256         // Dependent on hardware
#define MSBSETVALUE 128         // Value for just MSB set

uint8_t BuffIn[BUFFER_SIZE];
uint8_t BuffOut[BUFFER_SIZE];
size_t  InputIndex = 0;
size_t  OutputIndex = 0;

// Function to process raw data
void ProcessRawData(int DataSize) 
{
    size_t  FrameIndex = 0;
    uint8_t BuffTemp[FRAME_SIZE] = { 0 };
    int     IsFrameStarted = 0;
    int     Mask = (1 << DataSize) - 1;

    for (size_t ii = 0; ii < InputIndex; ++ii) 
    {
        if (BuffIn[ii] == MSBSETVALUE)  // if just MSB set, new frame
        { 
            if (IsFrameStarted) 
            {
                // write current frame to output buffer before starting a new one
                for (size_t jj = 0; jj < FrameIndex; ++jj) 
                {
                    BuffOut[OutputIndex++] = BuffTemp[jj];
                }
            }
            // Start a new frame
            FrameIndex = 0;
            IsFrameStarted = 1;
        }
        else if (IsFrameStarted && FrameIndex < FRAME_SIZE) 
        {
            // If not a frame marker, store lower DataSize bits of current byte in temp buffer
            BuffTemp[FrameIndex++] = BuffIn[ii] & Mask;
        }
    }

    // Process the last frame if it was started
    if (IsFrameStarted) 
    {
        for (size_t jj = 0; jj < FrameIndex; ++jj) 
        {
            BuffOut[OutputIndex++] = BuffTemp[jj];
        }
    }
}

void SimulateRawData(uint8_t* data, size_t length) 
{
    for (size_t ii = 0; ii < length; ++ii) 
    {
        BuffIn[InputIndex++] = data[ii];
    }
}

void PrintOutputBuffer(void) 
{
    int FrameCount = 1;
    int BytesPerFrameCount = 1;

    printf("\nOutput Data:\n");
    printf("\nFrame %d ", FrameCount);
    for (size_t ii = 0; ii < OutputIndex; ++ii) 
    {
        printf("0x%02X ", BuffOut[ii]);
        if (BytesPerFrameCount++ == FRAME_SIZE)
        {
            printf("\nFrame %d ", ++FrameCount);
            BytesPerFrameCount = 1;
        }
    }
    printf("\n");
}

void RunTest(void) 
{
    int DataSize = 7;       // Bits per Byte

    // Example test data
    uint8_t BuffRawData[] = {
        0x80, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x80, 0x02, 0x04, 0x06, 0x08, 0x10, 0x12, 0x14,
        0x80, 0x03, 0x06, 0x09, 0x12, 0x15, 0x18, 0x21,
        0x80, 0x04, 0x08
    };
    size_t RawDataLength = sizeof(BuffRawData) / sizeof(BuffRawData[0]);

    SimulateRawData(BuffRawData, RawDataLength);
   
    ProcessRawData(DataSize);

    printf("Input Data:\n");
    for (int ii = 0; ii < RawDataLength; ii++)
        printf("0x%02X ", BuffRawData[ii]);
        
    PrintOutputBuffer();

    // Reset indices for next test
    InputIndex = 0;
    OutputIndex = 0;
}


// UART receive interrupt handler (example, actual implementation depends on your MCU)
void UART_Receive_Handler(uint8_t received_byte) {
    // Store the received byte in the input buffer
    if (InputIndex < BUFFER_SIZE) 
    {
        BuffIn[InputIndex++] = received_byte;
    }
}
// Example function to initialize UART (details depend on your MCU)
void UART_Init(void) {
    // Initialize UART peripheral with desired settings
    // ...
}

// Example interrupt handler for receiving UART data (details depend on your MCU)
void UART_IRQHandler(void) 
{
    /*if (UART_Receive_Interrupt_Flag) {
        uint8_t received_byte = UART_Read_Received_Byte();
        UART_Receive_Handler(received_byte);
        // Clear the interrupt flag
        UART_Clear_Receive_Interrupt_Flag();
    }*/
}

int main(void) 
{

    RunTest();

    // Initialize UART (or other communication interfaces as needed)
    UART_Init();

    // Main loop
    /*while (1)
    {
        // Process the data in the input buffer
        ProcessRawData();

        // Do something with processed data

        // Reset the input and output indices for the next cycle
        input_index = 0;
        output_index = 0;
    }*/

    return 0;
}


