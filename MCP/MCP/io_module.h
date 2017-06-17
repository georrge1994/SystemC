#include "systemc.h"

#ifndef IO_H
#define IO_H

    SC_MODULE (IO)
    {
        int io_register;           
        
        sc_in_clk clk;                              // main clock
        sc_in<bool> sreset;                         // restart
        
        // to mc
        sc_in<bool> io_read;                        
        sc_in<bool> io_write;                       
        sc_in<sc_uint<12> > io_data_to_reg;        
        sc_out<sc_uint<12> > io_data_from_reg;    
   
        // from user
        sc_in<bool> user_read;                        
        sc_in<bool> user_write;                       
        sc_in<sc_uint<12> > user_data_to_reg;        
        sc_out<sc_uint<12> > user_data_from_reg;     
        
        void io_cycle();
        
        SC_CTOR(IO) :
                    clk("clk"),
                    sreset("sreset"),
                
                    io_read("io_read"),
                    io_write("io_write"),
                    io_data_to_reg("io_data_to_reg"),
                    io_data_from_reg("io_data_from_reg"),
                    user_read("user_read"),
                    user_write("user_write"),
                    user_data_to_reg("user_data_to_reg"),
                    user_data_from_reg("user_data_from_reg")
                
        {
            SC_CTHREAD(io_cycle, clk.pos());
            async_reset_signal_is(sreset,false);
        }
    };
    
#endif
