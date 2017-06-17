//-----------------------------------------------------
// Testbench for the register ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "ka.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

sc_signal<sc_uint<2> > register_in;
sc_signal<sc_uint<2> > register_out;
sc_signal<sc_uint<2> > state;  

void change_state(int link, int number_state, int out){
    register_in = link;
    sc_start(4, SC_NS);
    assert(register_out.read() == out);
    cout << "@" << sc_time_stamp() << " KA set in "<< number_state <<"("<<out<<") state with help "<< link << endl;
}

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 4, SC_NS);
    sc_signal<bool> reset;
    
    // Connect the DUT
    eightbit_register test_register("test_register");
    test_register.clock(clock);
    test_register.reset(reset);
    test_register.register_in(register_in);
    test_register.register_out(register_out);
   // test_register.state(state);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("KA");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, register_in, "link");
    sc_trace(wf, test_register.state, "state");
    sc_trace(wf, register_out, "out");

    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;

    sc_start(6, SC_NS);

    reset = 0; // De-assert the reset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;

    change_state(1, 0, 1); // link: 1 state: r0 -> r0 (out = 1)
    
    
    change_state(3, 1, 3); // link: 3, state: r0 -> r1 (out = 3)
    change_state(3, 0, 1); // link: 3, state: r1 -> r0 (out = 1)
    change_state(0, 2, 2); // link: 0, state: r0 -> r2 (out = 2) 
    change_state(3, 3, 0); // link: 3, state: r2 -> r3 (out = 0) 
    change_state(3, 2, 2); // link: 3, state: r3 -> r2 (out = 2) 
    change_state(1, 1, 3); // link: 1, state: r2 -> r1 (out = 3) 
    change_state(1, 0, 1); // link: 3, state: r1 -> r0 (out = 1)
    
    change_state(0, 2, 2); // link: 0, state: r0 -> r2 (out = 2
    change_state(0, 0, 1); // link: 0, state: r2 -> r0 (out = 1)
    
    change_state(3, 1, 3); // link: 3, state: r0 -> r1 (out = 3)
    change_state(0, 3, 0); // link: 0, state: r1 -> r3 (out = 0)
    
    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;
    sc_start(1, SC_NS);
    assert(register_out.read() == 1);
    sc_start(5, SC_NS);

    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation

}


