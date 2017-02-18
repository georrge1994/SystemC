//-----------------------------------------------------
// Testbench for the register ---------------->
//-----------------------------------------------------
#include "systemc.h"
#include "shift.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clock("clock", 4, SC_NS);
    sc_signal<bool> reset;
    sc_signal<bool> sreset_n;
    sc_signal<bool> ena; // ena is ena
    sc_signal<bool> load;   // load dat
    sc_signal<sc_uint<8> >  c_in;       // 8 bit vector input
    sc_signal<sc_uint<8> > d_out;      // 8 bit vector output
    sc_signal<sc_uint<1> > c_out;      // 1 bit
    
    // Connect the DUT
    eightbit_register test_register("test_register");
    test_register.clock(clock);
    test_register.reset(reset);
    test_register.sreset_n(sreset_n);
    test_register.ena(ena);
    test_register.load(load);
    test_register.c_in(c_in);
    test_register.d_out(d_out);
    test_register.c_out(c_out);
    
    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("register_waveform");
    // Dump the desired signals
    sc_trace(wf, clock, "clock");
    sc_trace(wf, reset, "reset");
    sc_trace(wf, sreset_n, "sreset_n");
    sc_trace(wf, ena, "ena");
    sc_trace(wf, load, "load");
    sc_trace(wf, c_in, "c_in");
    sc_trace(wf, d_out, "d_out");
    sc_trace(wf, c_out, "c_out");

    reset = 1; // Assert the reset
    sreset_n = 1; 
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;

    sc_start(6, SC_NS);

    reset = 0; // De-assert the reset
    sreset_n = 0; 
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;

    load = 1;       // load data
    c_in = 9;    // data
    sc_start(4, SC_NS);
    load = 0; 
    ena = 1;
    int tmp_d, tmp_c;
    for (int i = 0; i < 5; i++) {
        tmp_d = d_out.read();
        tmp_c = d_out.read() & 1;
        sc_start(4, SC_NS);
        cout << "@" << sc_time_stamp() << " count = " << d_out.read() <<" " << c_out.read() << endl;
        assert(d_out.read() == tmp_d >> 1);
        assert(c_out.read() == tmp_c);
    }

    reset = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << "Asynch Asserting reset\n" << endl;
    sc_start(1, SC_NS);
    assert(d_out.read() == 0);
    assert(c_out.read() == 0);
    sc_start(2, SC_NS);
    reset = 0;
    sc_start(2.5, SC_NS);

    
    sreset_n = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Synch Asserting reset\n" << endl;
    sc_start(1, SC_NS);
    assert(d_out.read() == 0);
    assert(c_out.read() == 0);
    sc_start(5, SC_NS);
    
    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << endl;
    sc_close_vcd_trace_file(wf);
    return 0; // Terminate simulation
}
