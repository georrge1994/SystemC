//-----------------------------------------------------
// Testbench for the register ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "counter.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 1, SC_NS);
    sc_signal<bool> reset;
    sc_signal<bool> sreset_n;
    sc_signal<bool>  up_down;
    sc_signal<sc_uint<8> > register_out;
    int i = 0;
    
    // Connect the DUT
    eightbit_register test_register("test_register");
    test_register.clock(clock);
    test_register.reset(reset);
    test_register.sreset_n(sreset_n);
    test_register.up_down(up_down);
    test_register.register_out(register_out);

    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("register_waveform");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, sreset_n, "sreset_n");
    sc_trace(wf, up_down, "up_down");
    sc_trace(wf, register_out, "dout");

    up_down = 1;    // set on up
    reset = 1;      // Assert the reset
    sreset_n = 1;   // set sreset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;

    sc_start(6, SC_NS); // wait 6 clk

    reset = 0;      // De-assert the reset
    sreset_n = 0;   // De-assert the sreset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;

    // test by count up
    cout << "\n Up count:" << endl;
    up_down = 1;
    for (i = 1; i < 9; i++) {  
        sc_start(1, SC_NS);
        cout << "@" << sc_time_stamp() << " count = " << register_out.read() << endl;
        assert(register_out.read() == i);
    }
    
    // test by count down
    cout << "\n Revers count:" << endl;
    up_down = 0;
    for (i = 7; i > 0; i--) {  
        sc_start(1, SC_NS);
        cout << "@" << sc_time_stamp() << " count = " << register_out.read() << endl;
        assert(register_out.read() == i);
    }

    // test for asynch reset
    up_down = 1;
    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << "Asynch Asserting reset\n" << endl;
    sc_start(1, SC_NS);
    assert(register_out.read() == 0);
    sc_start(2, SC_NS);
    reset = 0;
    sc_start(2.5, SC_NS);

    //test for synch reset
    sreset_n = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Synch Asserting reset\n" << endl;
    sc_start(1, SC_NS);
    assert(register_out.read() == 0);
    sc_start(5, SC_NS);
    
    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation
}
