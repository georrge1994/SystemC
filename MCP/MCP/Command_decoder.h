#include "systemc.h"

#ifndef Command_decoder_h
#define	Command_decoder_h

    SC_MODULE(Command_decoder){
        sc_in_clk clk;                          // main clock
        sc_in<bool> sreset;                     // restart

        // to memory controller and alu
        sc_out<sc_uint<8>> cmd;
        sc_out<sc_uint<12>> operand1;
        sc_out<sc_uint<12>> operand2;
        sc_in<sc_uint<32>> data_cell;
        
        void decoding_command();

        SC_CTOR(Command_decoder) :
                cmd("cmd"),
                operand1("operand1"),
                operand2("operand2"),
                data_cell("data_cell")
        {
            cout << "new Command_decoder" << endl;
            SC_CTHREAD(decoding_command, clk.pos());
            async_reset_signal_is(sreset,false);
        }
    };
#endif