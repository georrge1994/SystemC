#include "Memory_controller.h"

void Memory_controller::controlling()
{    
    alu_reg_read = false;                        // don`t read from alu
    alu_reg_write = false;                       // don`t write in alu reg
    ch_write = false;                           // don`t write in memory/cache
    ch_read = false;                            // only read new cmd from memory
    io_read = false;
    io_write = false;
    cd_data_cell = 0;                              // send nope comand to comd decoder
    ch_save_data = false;
    
    //if(ch_ready)
        mc_delay++;
    
    // memory controller is work only if cache ready
    if(ch_ready && !end && mc_delay >1){

        // don`t process if cmd nope or controller is busy
        if(cd_cmd.read() != 0 && first_step){
            // read operands and comand in buffer
            op1 = cd_operand1.read();
            op2 = cd_operand2.read();
            comand = cd_cmd.read();

            type_slave = comand & 1;
            cmd_group = (comand >> 1) & 7;
            id_cmd = (comand >> 4) & 15;     
            
            wait_cmd = false;
            
            cmd_counter++;
        }

        // if cmd for memory controoler, then work
        if(type_slave == 1)
        {
            if(first_step && second_step){
                
                // first step
                switch(cmd_group){
                    // moving cmd
                    case(1):

                        // mov addr, #5 (1 time)
                        if(id_cmd == 0) {  
                            // set signals for write
                            ch_write = true;
                            ch_addr_cell = op1;     // addr of cell in memory
                            ch_data_to_cell = op2;  // data, which will be written

                            // set flags for this cmd
                            memory_lock = true;     
                            first_step = true;
                            mc_delay=0;
                            
                            //cout<<"mov addr, #5"<<endl;
                            break; 
                        }

                        // mov X, addr (2 time) -- read data-cell from addr
                        if(id_cmd == 1 || id_cmd == 4) {  

                            // set signals for read
                            ch_read = true;
                            ch_addr_cell = op2;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;
                            
                            mc_delay=0;
                            //cout<< " mov addr, addr    or    mov reg, addr"<<endl;
                            break; 
                        }

                        // mov X, reg (2 time)  -- read data from register
                        if(id_cmd == 2 || id_cmd == 3) {  

                            // set signals for read
                            alu_reg_read = true;
                            alu_reg_id = op2;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;
                            //cout<<" mov addr, reg   or    mov reg, reg"<<endl;
                            break; 
                        }

                        // mov reg, #5 (1 time)
                        if(id_cmd == 5) {  
                            // set signals for write
                            alu_reg_write = true;
                            alu_reg_id = op1;        // id of reg
                            alu_reg_data_to = op2;   // data, which will be written

                            // set flags for this cmd
                            memory_lock = true;     
                            first_step = true;
                            //cout<<"mov reg, #5"<<endl;
                            break; 
                        }
                        
                        // mov reg, cmd_counter (1 time)
                        if(id_cmd == 6) {  
                            // set signals for write
                            alu_reg_write = true;
                            alu_reg_id = op1;                // id of reg
                            alu_reg_data_to = cmd_counter;   // data, which will be written

                            // set flags for this cmd
                            memory_lock = true;     
                            first_step = true;
                            //cout<<"mov reg, cmd_counter"<<endl;
                            break; 
                        }
                        
                        // mov cmd_counter, reg1 (2 time)
                        if(id_cmd == 7) {  
                            // set signals for write
                            alu_reg_read = true;
                            alu_reg_id = op2;                // id of reg
                            alu_reg_data_to = cmd_counter;   // data, which will be written

                            // set flags for this cmd
                            memory_lock = true;     
                            first_step = false;
                            //cout<<"mov cmd_counter, reg1"<<endl;
                            break; 
                        }
                        
                        break;

                    // jumping
                    case(2):

                        // jmp #5 (jump to addr)
                        if(id_cmd == 0) {  
                            cmd_counter = op1;
                            first_step = true;
                            //cout<<"jmp #5 "<<endl;
                            break; 
                        }

                        // jump if condition is true
                        if(id_cmd == 1) {  
                            //cout<<"jump if condition"<<endl;
                            // fbs-1, fss-2, fes-3, fnes-4, fbes-5, fses-6
                            if(op1 == 1 && alu_fbs){ cmd_counter = op2; /*cout<<"alu_fbs"<<endl;*/}
                            if(op1 == 2 && alu_fss){ cmd_counter = op2; /*cout<<"alu_fss"<<endl;*/}
                            if(op1 == 3 && alu_fes){ cmd_counter = op2;/* cout<<"alu_fes"<<endl;*/}
                            if(op1 == 4 && alu_fnes){ cmd_counter = op2; /*cout<<"alu_fnes"<<endl;*/}
                            if(op1 == 5 && alu_fbes){ cmd_counter = op2; /*cout<<"alu_fbes"<<endl;*/}
                            if(op1 == 6 && alu_fses){ cmd_counter = op2; /*cout<<"alu_fses"<<endl;*/}
                            
                            first_step = true;
                            
                            break; 
                        }

                        // ret
                        if(id_cmd == 2) {  
                            //cout<<"ret"<<endl;
                            if(level4 != 0){
                                cmd_counter = level4;
                                level4 = 0;

                            }else if(level3 != 0){
                                cmd_counter = level3;
                                level3 = 0;

                            }else if(level2 != 0){
                                cmd_counter = level2;
                                level2 = 0;

                            }else if(level1 != 0){
                                cmd_counter = level1;
                                level1 = 0;
                            }
                            
                            first_step = true;
                            
                            break; 
                        }

                        // call adr
                        if(id_cmd == 3) { 
                            //cout<<"call adr"<<endl;
                            if(level1 == 0){
                                level1 = cmd_counter;
                                cmd_counter = op1;

                            }else if(level2 == 0){
                                level2 = cmd_counter;
                                cmd_counter = op1;

                            }else if(level3 == 0){
                                level3 = cmd_counter;
                                cmd_counter = op1;

                            }else if(level4 == 0){
                                level4 = cmd_counter;
                                cmd_counter = op1;
                            }
                            
                            first_step = true;
                            break; 
                        }
                        
                        if(id_cmd == 4) {
                            //cout<<"end"<<endl;
                            end = true;
                            memory_lock = true;
                            first_step = true;
                            ch_save_data = true;
                            break;
                        }
                        break; 

                    // movxing cmd
                    case(3):

                        // movx reg1, reg2 (reg1 = memory[*reg2]) STEP1 - read addr from reg2
                        if(id_cmd == 0) {  
                            //cout<<"movx reg1, reg2"<<endl;
                            // set signals for read
                            alu_reg_read = true;
                            alu_reg_id = op2;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;

                            break; 
                        }

                        // movx reg1, addr (reg1 = memory[*addr]) STEP1 - read addr from addr
                        if(id_cmd == 1) {  
                            // cout<<"movx reg1, addr"<<endl;
                            // set signals for read
                            ch_read = true;
                            ch_addr_cell = op2;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;
                            
                            mc_delay=0;
                            break; 
                        }
                        
                        // movm reg1, reg2 (memory[*reg2] = reg1) STEP1 - read addr from reg2
                        if(id_cmd == 2) {  
                            //cout<<"movm reg1, reg2"<<endl;
                            // set signals for read
                            alu_reg_read = true;
                            alu_reg_id = op2;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;

                            break; 
                        }
                        
                        // movm addr, reg2 (memory[*reg2] = memory[addr]) STEP1 - read addr from reg2
                        if(id_cmd == 3) {  
                            //cout<<"movm addr, reg2"<<endl;
                            // set signals for read
                            alu_reg_read = true;
                            alu_reg_id = op2;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;

                            break; 
                        }
                        
                        break;
                        
                    // reading
                    case(4):

                        //mov reg, -io_reg-
                        if(id_cmd == 0){
                            //cout<<"mov reg, -io_reg-"<<endl;
                            // set signals for read
                            io_read = true;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;
                            
                            break;
                        }

                        // mov -io_reg-, reg
                        if(id_cmd == 1){
                            //cout<<"mov -io_reg-, reg"<<endl;
                            // set signals for read
                            alu_reg_read = true;
                            alu_reg_id = op1;

                            // set flags for this cmd
                            memory_lock = true;  
                            first_step = false;
                            break; 
                        }
                        break;
                }
                if(first_step){
                    op1 = 0;
                    op2 = 0;
                    comand = 0;
                    type_slave = 0;
                    cmd_group = 0;
                    id_cmd = 0;
                    first_step = true;
                }
            }
            else if(second_step)
            {   
                switch(cmd_group){
                    // moving cmd
                    case(1):
                        // mov addr, addr (2 time)
                        if(id_cmd == 1) {  

                            // set signals for write
                            ch_write = true;
                            ch_addr_cell = op1;
                            ch_data_to_cell = ch_data_from_cell.read();

                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = true;
                            
                            mc_delay=0;
                            break; 
                        }

                        // mov addr, reg (2 time)
                        if(id_cmd == 2) {  

                            // set signals for write
                            ch_write = true;
                            ch_addr_cell = op1;
                            ch_data_to_cell = alu_reg_data_from.read();
                            
                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = true;
                            
                            mc_delay=0;
                            break; 
                        }

                        // mov reg, reg (2 time)
                        if(id_cmd == 3) {  

                            // set signals for write
                            alu_reg_write = true;
                            alu_reg_id = op1;
                            alu_reg_data_to = alu_reg_data_from.read();

                            // set flags for this cmd
                            memory_lock = true; 
                            second_step = true;
                            break; 
                        }

                        // mov reg, addr (2 time)
                        if(id_cmd == 4) {  

                            // set signals for write
                            alu_reg_write = true;
                            alu_reg_id = op1;
                            alu_reg_data_to = ch_data_from_cell.read();

                            // set flags for this cmd
                            memory_lock = true; 
                            second_step = true;
                            break; 
                        }
                        
                        // mov cmd_counter, reg (2 time)
                        if(id_cmd == 7) {  

                            // set signals for write
                            cmd_counter = alu_reg_data_from.read();

                            // set flags for this cmd
                            memory_lock = false;  
                            second_step = true;
                            break; 
                        }
                        break;
                    
                    // Indirect addressing
                    case(3):
                        // movx reg1, reg2 (reg1 = memory[*reg2]) STEP2 - read data from memory 
                        if(id_cmd == 0){
                            // set signals for read
                            ch_read = true;
                            ch_addr_cell = alu_reg_data_from.read() & 4095;

                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = false;
                            
                            mc_delay=0;
                            break;
                        }
                        
                        // movx reg1, addr (reg1 = memory[*addr]) STEP2 - read data from addr
                        if(id_cmd == 1) {  
                            // set signals for read
                            ch_read = true;
                            ch_addr_cell = ch_data_from_cell.read() & 4095;

                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = false;

                            mc_delay=0;
                            break; 
                        }

                        // movm reg1, reg2 (memory[*reg2] = reg1) STEP2 - read data from reg1
                        if(id_cmd == 2) {  
                            // save addr in inner register
                            inner_reg = alu_reg_data_from.read();

                            // set signals for read
                            alu_reg_read = true;
                            alu_reg_id = op1;

                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = false;

                            break; 
                        }
                        
                        // movm addr, reg2 (memory[*reg2] = memory[addr]) STEP2 - read data from memory
                        if(id_cmd == 3) {  
                            
                            // save addr in inner register
                            inner_reg = alu_reg_data_from.read();

                            // set signals for read
                            ch_read = true;
                            ch_addr_cell = op1;

                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = false;

                            mc_delay=0;
                            break; 
                        }
                    case(4):
                        //mov reg, io_reg
                        if(id_cmd == 0){

                            // set signals for read
                            alu_reg_write = true;
                            alu_reg_id = op1;
                            alu_reg_data_to = io_data_from_reg.read();
                            
                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = true;
                            
                            break;
                        }
                        // mov io_reg, reg
                        if(id_cmd == 1){

                            // set signals for read
                            io_write = true;
                            io_data_to_reg = alu_reg_data_from.read() & 4095;

                            // set flags for this cmd
                            memory_lock = true;  
                            second_step = true;
                            break; 
                        }
                }

                if(second_step){
                    op1 = 0;
                    op2 = 0;
                    comand = 0;
                    type_slave = 0;
                    cmd_group = 0;
                    id_cmd = 0;
                    first_step = true;
                }
            }else{
                
                switch(cmd_group){
                    case(3):
                        
                        // movx reg1, reg2 (reg1 = memory[*reg2]) STEP3 - write data from memory in register
                        if(id_cmd == 0 || id_cmd == 1){
                            
                            // set signals for read
                            alu_reg_write = true;
                            alu_reg_id = op1;
                            alu_reg_data_to = ch_data_from_cell;

                            // set flags for this cmd
                            memory_lock = true;  
                            break;
                        }
                                                // movm reg1, reg2 (memory[*reg2] = reg1) STEP2 - read data from reg1
                        if(id_cmd == 2) {  
                            // set signals for write
                            ch_write = true;
                            ch_addr_cell = inner_reg;
                            ch_data_to_cell = alu_reg_data_from.read();

                            // set flags for this cmd
                            memory_lock = true; 
                            
                            mc_delay=0;
                            break; 
                        }
                        
                                                // movm addr, reg2 (memory[*reg2] = memory[addr]) STEP2 - read data from memory
                        if(id_cmd == 3) {  
                            
                            // set signals for write
                            ch_write = true;
                            ch_addr_cell = inner_reg;
                            ch_data_to_cell = ch_data_from_cell.read();

                            // set flags for this cmd
                            memory_lock = true;  
                            
                            mc_delay=0;
                            break; 
                        }
                        break;
                }
                
                op1 = 0;
                op2 = 0;
                comand = 0;
                type_slave = 0;
                cmd_group = 0;
                id_cmd = 0;
                first_step = true;
                second_step = true;

            }
        }

        // set signal for read cmd 
        if(wait_cmd){      
            cd_data_cell = ch_data_from_cell;

            if(cd_delay >= 2){
                wait_cmd = false;
                cd_delay = 0;
            }else{
                cd_delay++;
            }
            memory_lock = true;

        }

        if(!memory_lock){
            // set signal for read cmd
            ch_read = true;                 // read new cmd from memory
            ch_addr_cell = cmd_counter;     // set addr, witch will be read from memory
            wait_cmd = true;

            // reset inner register
            type_slave = 0;
            cmd_group = 0;
            id_cmd = 0;
            mc_delay=0;
        }

        // free lock
        memory_lock = false;
        
    }else{
        ch_save_data = false;
    }

    wait();
}