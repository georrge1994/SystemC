#include "ALU.h"
void Alu::clear_regs_and_flags(){
    // reset cashe
    for(int i = 0; i < (sizeof(registers)/sizeof(*registers)); i++)
        registers[i] = 0;
    
    // clear registers
    fbs = 0;
    fss = 0;
    fes = 0;
    fnes = 0;
    fbes = 0;
    fses = 0;
}

void Alu::calculate(){
    
    // Set uniq number of register
    id_reg = alu_reg_id.read();

    // write to registers
    if(alu_reg_write.read())
        registers[id_reg] = alu_reg_data_to.read();

    // read from registers
    if(alu_reg_read.read()){
        alu_reg_data_from = registers[id_reg];
    }

    // alu work
    op1 = operand1.read();
    op2 = operand2.read();
    comand = cmd.read();
    type_slave = comand & 1;

    if(type_slave == 0)// alu is ф slave with id equal zero
    {
        cmd_group = (comand >> 1) & 7;
        id_cmd = (comand >> 4) & 15;
        
        switch(cmd_group){
            case (0):
                // nope comand
                break;

            // Mathematic
            case(1):
                if(id_cmd == 0) { _add(op1, op2); break; }
                if(id_cmd == 1) { _sub(op1, op2); break; }
                if(id_cmd == 2) { _mult(op1, op2); break; }
                if(id_cmd == 3) { _div(op1, op2); break; }
                if(id_cmd == 4) { _inc(op1); break; }
                if(id_cmd == 5) { _dec(op1); break; }
                break;

            // Boolean 
            case(2):
                if(id_cmd == 0) { _xor(op1, op2); break; }
                if(id_cmd == 1) { _and(op1, op2); break; }
                if(id_cmd == 2) { _or(op1, op2); break; }
                if(id_cmd == 3) { _not(op1); break; }
                break; 

            // Shifts
            case(3):
                if(id_cmd == 0) { _rs(op1); break; }
                if(id_cmd == 1) { _ls(op1); break; }
                if(id_cmd == 2) { _rsn(op1, op2); break; }
                if(id_cmd == 3) { _lsn(op1, op2); break; }
                break;

            // Inequality
            case(4):
                if(id_cmd == 0) { _fbs(op1, op2); break; }
                if(id_cmd == 1) { _fss(op1, op2); break; }
                if(id_cmd == 2) { _fbes(op1, op2); break; }
                if(id_cmd == 3) { _fses(op1, op2); break; }
                if(id_cmd == 4) { _fes(op1, op2); break; }
                if(id_cmd == 5) { _fnes(op1, op2); break; }
                break;   
        }
    }

    alu_fbs = fbs;
    alu_fss = fss;
    alu_fes = fes;
    alu_fnes = fnes;
    alu_fbes = fbes;
    alu_fses = fses;

    wait();
}

// Mathematical actions (result write in a)
void Alu::_add(int id_1, int id_2){ registers[id_1] += registers[id_2]; /*cout<<"add"<<endl;*/}  
void Alu::_sub(int id_1, int id_2){ registers[id_1] -= registers[id_2]; /*cout<<"sub"<<endl;*/}  
void Alu::_mult(int id_1, int id_2){registers[id_1] *= registers[id_2]; /*cout<<"mult"<<endl;*/}  
void Alu::_div(int id_1, int id_2){ registers[id_1] /= registers[id_2];/* cout<<"div"<<endl;*/}  
void Alu::_inc(int id_1){registers[id_1] += 1; /*cout<<""<<endl;*/}
void Alu::_dec(int id_1){registers[id_1] -= 1; /*cout<<""<<endl;*/}

// Boolean actions
void Alu::_xor(int id_1, int id_2){registers[id_1] = registers[id_1] xor registers[id_2]; /*cout<<"xor"<<endl;*/}  
void Alu::_and(int id_1, int id_2){registers[id_1] &= registers[id_2];/* cout<<"and"<<endl;*/}   
void Alu::_or(int id_1, int id_2){registers[id_1] |= registers[id_2]; /*cout<<"or"<<endl;*/}   
void Alu::_not(int id_1){registers[id_1] = ~registers[id_1]; /*cout<<"not"<<endl;*/}   

// shifts
void Alu::_rs(int id_1){ registers[id_1] = registers[id_1] >> 1; /*cout<<"rs"<<endl;*/}  
void Alu::_ls(int id_1){ registers[id_1] = registers[id_1] << 1; /*cout<<"ls"<<endl;*/}  
void Alu::_rsn(int id_1, int id_2){ registers[id_1] = registers[id_1] >> registers[id_2]; /*cout<<"rsn"<<endl;*/}  
void Alu::_lsn(int id_1, int id_2){ registers[id_1] = registers[id_1] << registers[id_2]; /*cout<<"lsn"<<endl;*/}  

// comparison
// fbs-1, fss-2, fes-3, fnes-4, fbes-5, fses-6
void Alu::_fbs(int id_1, int id_2){ fbs = registers[id_1] > registers[id_2]; /*cout<<"bfs"<<endl;*/}
void Alu::_fss(int id_1, int id_2){ fss = registers[id_1] < registers[id_2]; /*cout<<"fss"<<endl;*/}
void Alu::_fbes(int id_1, int id_2){ fbes = registers[id_1] >= registers[id_2]; /*cout<<"fbes"<<endl;*/}
void Alu::_fses(int id_1, int id_2){ fses = registers[id_1] <= registers[id_2]; /*cout<<"fses"<<endl;*/}
void Alu::_fes(int id_1, int id_2){ fes = registers[id_1] == registers[id_2]; /*cout<<"fes"<<endl;*/}
void Alu::_fnes(int id_1, int id_2){fnes = registers[id_1] != registers[id_2]; /*cout<<"fnes"<<endl;*/}
