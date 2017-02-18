#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

SC_MODULE(eightbit_register) {
    sc_in_clk clock; // Clock input of the design
    sc_in<bool> reset; // active high, asynchronous Reset input
    sc_in<bool> sreset_n; // active high, synch Reset input
    sc_in<bool> ena; // ena is ena
    sc_in<bool> load;   // load data
    sc_in<sc_uint<8> >  c_in;       // 8 bit vector input
    sc_out<sc_uint<8> > d_out;      // 8 bit vector output
    sc_out<sc_uint<1> > c_out;      // 1 bit

    sc_uint<8> myregister;
    sc_uint<1> last_bit;
    
    void register_store();

    SC_CTOR(eightbit_register) :
            clock("clock"),
            reset("reset"),
            sreset_n("sreset_n"),
            ena("ena"),
            load("load"),
            c_in("c_in"),
            c_out("c_out"), 
            d_out("d_out") {
        cout << "Executing new" << endl;
        SC_CTHREAD(register_store, clock.pos());
        async_reset_signal_is(reset, true);
        reset_signal_is(sreset_n, true);
    } // End of Constructor

}; // End of Module

#endif	/* DESIGN_H */

