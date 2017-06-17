#include "Command_decoder.h"

void Command_decoder::decoding_command()
{
    int input_cell = data_cell.read();     // read from memory
    while(true){
        cmd =(input_cell >> 24) &  255;        // 8 2
        operand1 = (input_cell >> 12) & 4095;  // 12 
        operand2 = input_cell & 4095;          // 12
        wait();
    }
}