#include "systemc.h"


#ifndef MEMORY_CONTROLLER_H
#define MEMORY_CONTROLLER_H

    SC_MODULE (Memory_controller)
    {          
        sc_in_clk clk;                              // main clock
        sc_in<bool> sreset;                         // restart
        
        // to cache
        sc_out<bool> ch_read;                        // read from cache cell
        sc_out<bool> ch_write;                       // write to cache in cell
        sc_out<sc_uint<12> > ch_addr_cell;           // addr needing memory cell
        sc_out<sc_uint<32> > ch_data_to_cell;        // data wrote in memory cell
        sc_in<sc_uint<32> > ch_data_from_cell;     // data from memory cell
        sc_in<bool> ch_ready;       
        sc_out<bool> ch_save_data;
        
        // to command decoder
        sc_in<sc_uint<8>> cd_cmd;
        sc_in<sc_uint<12>> cd_operand1;
        sc_in<sc_uint<12>> cd_operand2;
        sc_out<sc_uint<32>> cd_data_cell;    
        
        // alu
        //--registers 
        sc_out<sc_uint<4>> alu_reg_id;
        sc_out<bool> alu_reg_read;                        // read from cache cell
        sc_out<bool> alu_reg_write;                       // write to cache in cell
        sc_out<sc_uint<32>> alu_reg_data_to;
        sc_in<sc_uint<32>> alu_reg_data_from;
        //--flags
        sc_in<bool> alu_fbs;
        sc_in<bool> alu_fss;
        sc_in<bool> alu_fes;
        sc_in<bool> alu_fnes;
        sc_in<bool> alu_fbes;
        sc_in<bool> alu_fses;
        
        // to io
        sc_out<bool> io_read;                        
        sc_out<bool> io_write;                       
        sc_out<sc_uint<12> > io_data_to_reg;        
        sc_in<sc_uint<12> > io_data_from_reg;     
        
        int cmd_counter = 0;
        bool memory_lock;           // one of the memory controller block blocked access to memory
        bool first_step = true;     // cmd in first step of the execute
        bool second_step = true;
        bool wait_cmd;              // memory controller send signal "getNewCmd", and is awaiting result
        bool end = false;
        
        // the inner registers
        int comand, type_slave, cmd_group, id_cmd, inner_reg; 
        int op1, op2;
        int cd_delay = 0;
        int mc_delay = 0;
        int alu_delay = 0;
        
        // sub-programs
        int level1, level2, level3, level4;
        
        void controlling();
        
        
        SC_CTOR(Memory_controller) :
                clk("clk"),
                sreset("sreset"),
                ch_read("ch_read"),
                ch_write("ch_write"),
                ch_addr_cell("ch_addr_cell"),
                ch_data_to_cell("ch_data_to_cell"),
                ch_data_from_cell("ch_data_from_cell"),
                ch_save_data("ch_save_data"),
                cd_cmd("cd_cmdcmd"),
                cd_operand1("cd_operand1"),
                cd_operand2("cd_operand2"),
                cd_data_cell("cd_data_cell"),
                alu_reg_id("alu_reg_id"),
                alu_reg_read("alu_reg_read"),
                alu_reg_write("alu_reg_write"),
                alu_reg_data_to("alu_reg_data_to"),
                alu_reg_data_from("alu_reg_data_from"),
                alu_fbs("alu_fbs"),
                alu_fss("alu_fss"),
                alu_fes("alu_fes"),
                alu_fnes("alu_fnes"),
                alu_fbes("alu_fbes"),
                alu_fses("alu_fses")
        {   
            SC_CTHREAD(controlling, clk.pos());
            async_reset_signal_is(sreset,false);
        }
    };
    
#endif
