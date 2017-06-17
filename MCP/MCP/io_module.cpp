#include "io_module.h"

void IO::io_cycle()
{   
    while (true) {
        
        if(user_read.read())
            user_data_from_reg = io_register & 4095;
        
        if(user_write.read())
            io_register = user_data_to_reg.read();
        
        if(io_read.read())
            io_data_from_reg = io_register & 4095;
        
        if(io_write.read())
            io_register = io_data_to_reg.read();
        
        wait();
    }
}