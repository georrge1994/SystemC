#include "fifo.h"

void fifo::processing()
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

void fifo::read()
{
    if(pop)
    {
        data_out = data[first];
        --elements;
        first = (first + 1) % max;
    }
}

void fifo::write()
{
    if(push)
    {
        data[(first + elements) % max] = data_in.read();
        ++elements;
    }
}