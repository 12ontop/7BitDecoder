// Cpp7BitDecoder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>

// Define constants
const size_t FRAME_SIZE = 8;    // Number of bytes per frame (change if needed)
const size_t BUFFER_SIZE = 256; // Adjust based on available memory and expected data size
const size_t MSBSET = 128;      // Value if just MSB set

// Buffers
std::vector<uint8_t> BuffIn(BUFFER_SIZE);
std::vector<uint8_t> BuffOut(BUFFER_SIZE);
size_t InputIndex = 0;
size_t OutputIndex = 0;

// Function to process encoded data
void ProcessRawData() 
{
    size_t frame_index = 0;
    std::vector<uint8_t> temp_buffer(FRAME_SIZE);
    bool is_frame_started = false;

    for (size_t ii = 0; ii < InputIndex; ++ii) 
    {
        if (BuffIn[ii] == MSBSET)    //if just MSB is set, this is new frame
        { 
            if (is_frame_started) 
            {
                // Process the current frame before starting a new one
                for (size_t jj = 0; jj < frame_index; ++jj) 
                {
                    BuffOut[OutputIndex++] = temp_buffer[jj];
                }
            }
            // Start a new frame
            frame_index = 0;
            is_frame_started = true;
        }

        if (is_frame_started && frame_index < FRAME_SIZE) 
        {
            // Store the lower 7 bits of the current byte in the temp buffer
            temp_buffer[frame_index++] = BuffIn[ii] & 0x7F;
        }
    }

    // Process the last frame if it was started
    if (is_frame_started) 
    {
        for (size_t jj = 0; jj < frame_index; ++jj) 
        {
            BuffOut[OutputIndex++] = temp_buffer[jj];
        }
    }
}

// Function to simulate incoming data
void SimulateRawData(const std::vector<uint8_t>& data) 
{
    for (uint8_t byte : data) 
    {
        if (InputIndex < BUFFER_SIZE) 
        {
            BuffIn[InputIndex++] = byte;
        }
    }
}

// Function to print the output buffer
void PrintOutputBuffer() 
{
    std::cout << "Output Data:\n";
    for (size_t ii = 0; ii < OutputIndex; ++ii) 
    {
        //std::cout << "0x" << std::hex << static_cast<int>(output_buffer[i]) << " ";
        std::cout << static_cast<int>(BuffOut[ii]) << " ";
    }
    std::cout << "\n"; // Switch back to decimal output
}

// Test function
void RunTest() 
{
    // Example test data (adjust as needed)
    std::vector<uint8_t> test_data = {
        0x80, 0x01, 0x02, 0x03, 0x80, 0x05, 0x06, 0x07, 0x08, 0x80, 0x11, 0x12
    };

    // Simulate incoming data
    SimulateRawData(test_data);

    // Process the encoded data
    ProcessRawData();

    // Print the output buffer
    PrintOutputBuffer();

    // Reset indices for next test
    InputIndex = 0;
    OutputIndex = 0;
}

int main() 
{
    // Run the test
    RunTest();

    return 0;
}