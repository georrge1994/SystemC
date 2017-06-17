#include "systemc.h"

#ifndef FIFOPARAM_H
#define	FIFOPARAM_H

template <typename T> SC_MODULE(fifo){
    public:	
            sc_in_clk clk;
            sc_in<bool> sreset_n;
            sc_in<T> data_in;
            sc_in<bool> push;
            sc_in<bool> pop;
            sc_out<T> data_out;
            sc_out<bool> empty;
            sc_out<bool> full;
            sc_out<sc_uint<4> > count_elements;
            
            enum e {max = 10};
            T data[max];
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
                    full("full"),
                    count_elements("count_elements")
            {
                    cout << "Executing new" << endl;
            SC_CTHREAD(processing, clk.pos());
                    reset_signal_is(sreset_n,true);
            }

};

template <typename T> void fifo<T>::processing()
{
    empty = true;
    full = false;
    data_out = 0;
    first = 0;
    elements = 0;
    for(int i = 0; i<=max; i++)
        data[i] = 0;

    while(true){
        if(elements == max)
            read();
        else if(elements == 0)
            write();
        else{
            read();
            write();
        }
        if(elements == max)
            full = true;
        else
            full = false;
        if(elements == 0)
            empty = true;
        else
            empty = false;
        
        count_elements = elements;
        wait();
    }
}

template <typename T> void fifo<T>::read()
{
    if(pop)
    {
        data_out = data[first];
        --elements;
        first = (first + 1) % max;
    }
}

template <typename T> void fifo<T>::write()
{
    if(push)
    {
        data[(first + elements) % max] = data_in.read();
        ++elements;
    }
}
#endif