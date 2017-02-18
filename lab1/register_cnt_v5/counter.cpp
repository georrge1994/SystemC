//-----------------------------------------------------
// An 8 bit register with asynchronous active high reset
//-----------------------------------------------------
#include "counter.h"

void eightbit_register::register_store() {
    // We check if reset is active    
    myregister = 0;
    register_out.write(myregister);
    wait();
    while (true) {
        if(up_down)
            myregister++;
        else
            myregister--;
       
        register_out.write(myregister);
        wait();
    }
}
