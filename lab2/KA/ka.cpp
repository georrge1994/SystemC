//-----------------------------------------------------
// An 8 bit register with asynchronous active high reset
//-----------------------------------------------------
#include "ka.h"

void eightbit_register::register_store() {
    // We check if reset is active    
    state = 0;
    int signal = -1;
    wait();
    while (true) {
        signal= register_in.read();
        // 0 state
        if(state.read() == 0){
                if(signal == 1)
                    state = 0;
                else if(signal == 3)
                    state = 1;
                else if(signal == 0)
                    state = 2;
            
        // 1 state
        }else if(state.read() == 1){
                if(signal == 1 || signal == 3)
                    state = 0;
                else if(signal == 0)
                    state = 3; 
                
        // 2 state
        }else if(state.read() == 2){
                if(signal == 0)
                    state = 0;
                else if(signal == 1)
                    state = 1;
                else if(signal == 3)
                    state = 3;
                
        // 3 state
        }else if(state.read() == 3){
                if(signal == 3)
                    state = 2;
        }
        wait();
    }
}

void eightbit_register::data_out_method() {
    register_out = state;
}
