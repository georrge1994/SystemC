//-----------------------------------------------------
// An 8 bit register with asynchronous active high reset
//-----------------------------------------------------
#include "shift.h"

void eightbit_register::register_store() {
    // We check if reset is active    
    myregister = 0;
    last_bit = 0;
    
    wait();
    while (true) {
        if(load)
            myregister = c_in.read();
        
        if(ena){
            last_bit = 1 & myregister;
            myregister = myregister >> 1;
        }

        cout << "@" << sc_time_stamp() << " :: Have stored d_out" << myregister << " c_out" << last_bit<< endl;
        c_out.write(last_bit);
        d_out.write(myregister);
        wait();
    }
}
