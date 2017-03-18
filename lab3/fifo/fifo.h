#include "systemc.h"

#ifndef FIFO_H
#define	FIFO_H
SC_MODULE(fifo){
	sc_in_clk clk;
	sc_in<bool> sreset_n;
	sc_in<sc_uint<8> > data_in;
	sc_in<bool> push;
	sc_in<bool> pop;
	sc_out<sc_uint<8> > data_out;
	sc_out<bool> empty;
	sc_out<bool> full;

	enum e {max = 10};
	unsigned int data[max];
	int elements, first;

	void processing();
	void read();
	void write();

	SC_CTOR(fifo):
		clk("clk"),
		sreset_n("reset"),
		data_in("data_in"),
		push("push"),
		pop("pop"),
		data_out("data_out"),
		empty("empty"),
		full("full")
	{
		cout << "Executing new" << endl;
        SC_CTHREAD(processing, clk.pos());
		reset_signal_is(sreset_n,true);
	}

};
#endif /*FIFO_H*/