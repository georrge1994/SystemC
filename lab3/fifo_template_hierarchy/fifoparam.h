#include <systemc.h>

#ifndef FIFOPARAM_H
#define	FIFOPARAM_H

template <typename T>
SC_MODULE(fifoparam){
public:	
	sc_in_clk clk;
	sc_in<bool> sreset_n;
	sc_in<T> data_in;
	sc_in<bool> push;
	sc_in<bool> pop;
	sc_out<T> data_out;
	sc_out<bool> empty;
	sc_out<bool> full;

	//изменить код, используя эти сигналы: для empty, full, almost_empty, almost_full сделать их изменение комбинаторным
	sc_signal<bool> is_write;
	sc_signal<bool> is_read;

	int max;
	T *data;
	int num_elements, first;

	void processing();
	void read();
	void write();
	void flags();
	//fifoparam(sc_module_name _name);

	SC_HAS_PROCESS(fifoparam);

	//SC_CTOR(fifoparam):
	fifoparam(sc_module_name _name, int _size):
	clk("clk"),
		sreset_n("reset"),
		data_in("data_in"),
		push("push"),
		pop("pop"),
		data_out("data_out"),
		empty("empty"),
		full("full"),
		max(_size)
	{
		data = new T[max];
		cout << "Executing new" << endl;
		SC_CTHREAD(processing, clk.pos());
		reset_signal_is(sreset_n,true);
		SC_METHOD(flags);
		sensitive << is_read << is_write <<clk;
	}

};

template <typename T>
void fifoparam<T>::processing()
{
	//empty = true;
	//full = false;
	is_read = false;
	is_write = false;
	data_out = 0;
	first = 0;
	num_elements = 0;
	for(int i = 0; i<=max; i++)
		data[i] = 0;
	wait();
	while(true){
		if(num_elements == max)
		{
			//full = true;
			read();
		}
		else if(num_elements == 0)
		{
			//empty = true;
			write();
		}
		else{
			//empty = false;
			//full = false;
			read();
			write();
		}
		wait();
	}
}

template <typename T>
void fifoparam<T>::read()
{
	if(pop)
	{
		is_read = !is_read;
		data_out = data[first];
		--num_elements;
		first = (first + 1) % max;
	}
}

template <typename T>
void fifoparam<T>::write()
{
	if(push)
	{
		is_write = !is_write;
		data[(first + num_elements) % max] = data_in.read();
		++num_elements;
	}
}

template <typename T>
void fifoparam<T>::flags()
{
	full = false;
	empty = true;
	if(num_elements == max)
	{
		full = true;
		empty = false;
	}
	else if(num_elements == 0)
	{
		empty = true;
		full = false;
	}
	else
	{
		full = false;
		empty = false;
	}
}
#endif /*FIFO_H*/