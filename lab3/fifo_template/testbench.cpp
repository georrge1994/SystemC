#include "systemc.h"
#include "fifo_template.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clock", 4, SC_NS);
    sc_signal<char> data_in;
    sc_signal<char> data_out;
    sc_signal<bool> sreset_n;
    sc_signal<bool> push;
    sc_signal<bool> pop;
    sc_signal<bool> empty;
    sc_signal<bool> full;

    fifo<char> test("test");
    test.clk(clk);
    test.sreset_n(sreset_n);
    test.data_in(data_in);
    test.push(push);
    test.pop(pop);
    test.data_out(data_out);
    test.empty(empty);
    test.full(full);
        
    // Open VCD file
    sc_trace_file *wf = sc_create_vcd_trace_file("fifo_waveform");
    // Dump the desired signals
    sc_trace(wf, clk, "clock");
    sc_trace(wf, sreset_n, "reset");
    sc_trace(wf, data_in, "in");
    sc_trace(wf, push, "push");
    sc_trace(wf, pop, "pop");
    sc_trace(wf, data_out, "out");
    sc_trace(wf, empty, "empty");
    sc_trace(wf, full, "full");
    sc_trace(wf, test.elements, "elements");

    // test of reset
    sreset_n = 1; // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << endl;
    sc_start(6, SC_NS);
    assert(empty.read() == 1);
    
    
    // test of push a data (full)
    sreset_n = 0; // De-assert the reset
    push = true;
    for(int i = 1; i < 12; i++){
        cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;
        data_in = 'a' + i;
        sc_start(4, SC_NS);
        if(i > 1)
            assert(empty.read() == 0);
        else
            assert(empty.read() == 1);
        
        if(i>10)
             assert(full.read() == 1);
    }
    
    
    // test of pop a data
    pop = true;
    push = false;
    for(int i = 1; i < 7; i++){
        cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << endl;
        sc_start(4, SC_NS);
        assert(data_out.read() == 'a' + i);
    }


    return 0;
}