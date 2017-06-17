#include "systemc.h"

#ifndef DESIGN_H
#define	DESIGN_H

SC_MODULE(eightbit_register) {
    sc_in_clk clock; // Clock input of the design
    sc_in<bool> reset; // active high, asynchronous Reset input
    sc_in<bool> sreset_n; // active high, synch Reset input
    sc_in<bool> up_down; // active high - up, low - down
    sc_out<sc_uint<8> > register_out; // 8 bit vector output

    sc_uint<8> myregister;

    void register_store();

    SC_CTOR(eightbit_register) :
            clock("clock"),
            reset("reset"),
            sreset_n("sreset_n"),
            up_down("up_down"),
            register_out("register_out") {
        cout << "Executing new" << endl;
        SC_CTHREAD(register_store, clock.pos());
        async_reset_signal_is(reset, true);
        reset_signal_is(sreset_n, false);
    } // End of Constructor

}; // End of Module

#endif	/* DESIGN_H */

