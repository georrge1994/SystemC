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
                if(signal == 1)         // if input singal is 1
                    state = 0;          // go to 0 state
                
                else if(signal == 3)    // if input singal is 3
                    state = 1;          // go to 1 state
                
                else if(signal == 0)    // if input singal is 0
                    state = 2;          // go to 2 state
            
        // 1 state
        }else if(state.read() == 1){
                if(signal == 1 || signal == 3) // if input singal is 1 or 3
                    state = 0;          // go to 0 state
                
                else if(signal == 0)    // if input singal is 0
                    state = 3;          // go to 3 state
                
        // 2 state
        }else if(state.read() == 2){
                if(signal == 0)         // if input singal is 0
                    state = 0;          // go to 0 state
                
                else if(signal == 1)    // if input singal is 1
                    state = 1;          // go to 1 state
                
                else if(signal == 3)    // if input singal is 3
                    state = 3;          // go to 3 state
                
        // 3 state
        }else if(state.read() == 3){
            
                if(signal == 3)         // if input singal is 3
                    state = 2;          // go to 2 state
        }
        wait();
    }
}

void eightbit_register::data_out_method() {
    
    // output table
    switch(state.read()){
        case(0):
            register_out = 1;           // state 0 is 1 in out
            break;
        case(1):
            register_out = 3;           // state 1 is 3 in out
            break;
        case(2):
            register_out = 2;           // state 2 is 2 in out
            break;
        case(3):
            register_out = 0;           // state 3 is 0 in out
            break;
    }
}
