#include "systemc.h"

#include "Virtual_memory.h"
#include "ALU.h"
#include "Cache.h"
#include "Memory_controller.h"
#include "Command_decoder.h"
#include "io_module.h"

#ifndef Main_bus
#define	Main_bus
    SC_MODULE(Top){
        sc_in_clk clk;                          // main clock
        sc_in<bool> sreset;                     // restart


        Virtual_memory vm;
        Alu alu;
        Cache cache;
        Memory_controller mc;
        Command_decoder cd;
        IO io;
        
        // virtual memory <=> real memory
        // to ram memory
        sc_out<bool> ram_out_read;                  // read from cell
        sc_out<bool> ram_out_write;                 // write in cell
        sc_out<sc_uint<10> > ram_out_addr_cell;
        sc_out<sc_uint<32> > ram_out_data_to_cell;
        sc_in<sc_uint<32> > ram_in_data_from_cell;
        sc_in<bool> ram_in_ready;
        // to hard memory
        sc_out<bool> hm_out_read;                  // read from cell
        sc_out<bool> hm_out_write;                 // write in cell
        sc_out<sc_uint<12> > hm_out_addr_cell;
        sc_out<sc_uint<32> > hm_out_data_to_cell;
        sc_in<sc_uint<32> > hm_in_data_from_cell;
        sc_in<bool> hm_in_ready;
        
        // memory <-> cache
        sc_signal<bool> m_read;                         // if data in cache not exit, get it from memory
        sc_signal<bool> m_write;                  
        sc_signal<sc_uint<12> > m_addr_cell;            // addr needing memory cell
        sc_signal<sc_uint<32> > m_data_to_cell;         // data wrote in memory cell
        sc_signal<sc_uint<32> > m_data_from_cell;       // data from memory cell
        sc_signal<bool> m_ready;                        // cache is ready to work
        sc_signal<bool> m_save_data;
        
        // cache <-> memory controller
        sc_signal<bool> mc_read;                        // read from cache cell
        sc_signal<bool> mc_write;                       // write to cache in cell
        sc_signal<sc_uint<12> > mc_addr_cell;           // addr needing memory cell
        sc_signal<sc_uint<32> > mc_data_to_cell;        // data wrote in memory cell
        sc_signal<sc_uint<32> > mc_data_from_cell;      // data from memory cell
        sc_signal<bool> mc_ready;                       // cache is ready to work
        sc_signal<bool> mc_save_data;
        
        // command decoder <-> alu and memory controller
        sc_signal<sc_uint<8>> cmd;
        sc_signal<sc_uint<12>> operand1;
        sc_signal<sc_uint<12>> operand2;
        sc_signal<sc_uint<32>> data_cell;   
        
        // alu <-> memory controller
        sc_signal<sc_uint<4>> reg_id;
        sc_signal<bool> reg_read;                        // read from cache cell
        sc_signal<bool> reg_write;                       // write to cache in cell
        sc_signal<sc_uint<32>> reg_data_to;
        sc_signal<sc_uint<32>> reg_data_from;  
        //--flags
        sc_signal<bool> fbs;
        sc_signal<bool> fss;
        sc_signal<bool> fes;
        sc_signal<bool> fnes;
        sc_signal<bool> fbes;
        sc_signal<bool> fses;
        
        // memory controller <-> io_module
        sc_signal<bool> io_read;                        
        sc_signal<bool> io_write;                       
        sc_signal<sc_uint<12> > io_data_to_reg;        
        sc_signal<sc_uint<12> > io_data_from_reg; 
        
        // User <-> io_module
        sc_in<bool> user_read;                        
        sc_in<bool> user_write;                       
        sc_in<sc_uint<12> > user_data_to_reg;        
        sc_out<sc_uint<12> > user_data_from_reg;  
        
        void main_cycle();

        SC_CTOR(Top) : vm("memory"), cache("cache"), mc("mc"), 
                alu("alu"), cd("cd"), io("io"){              
            
            // 1. MEMORY
            vm.clk(clk);
            vm.sreset(sreset);
            // to cache
            vm.ch_out_data_from_cell(m_data_from_cell);
            vm.ch_in_data_to_cell(m_data_to_cell);
            vm.ch_in_addr_cell(m_addr_cell);
            vm.ch_in_read(m_read);
            vm.ch_in_write(m_write);
            vm.ch_out_ready(m_ready);
            vm.ch_in_save_all(m_save_data);
            // to real memory
            vm.ram_out_read(ram_out_read);
            vm.ram_out_write(ram_out_write);
            vm.ram_out_addr_cell(ram_out_addr_cell);
            vm.ram_out_data_to_cell(ram_out_data_to_cell);
            vm.ram_in_data_from_cell(ram_in_data_from_cell);
            vm.ram_in_ready(ram_in_ready);
            vm.hm_out_read(hm_out_read);
            vm.hm_out_write(hm_out_write);
            vm.hm_out_addr_cell(hm_out_addr_cell);
            vm.hm_out_data_to_cell(hm_out_data_to_cell);
            vm.hm_in_data_from_cell(hm_in_data_from_cell);
            vm.hm_in_ready(hm_in_ready);
            
            // 2. CACHE
            cache.clk(clk);
            cache.sreset(sreset);
            // to memory controller
            cache.mc_data_from_cell(mc_data_from_cell);
            cache.mc_data_to_cell(mc_data_to_cell);
            cache.mc_addr_cell(mc_addr_cell);
            cache.mc_read(mc_read);
            cache.mc_write(mc_write);   
            cache.mc_ready(mc_ready);
            cache.mc_save_all(mc_save_data);
            // to memory
            cache.m_read(m_read);                      // if data in cache not exit, get it from memory
            cache.m_write(m_write);
            cache.m_addr_cell(m_addr_cell);             // addr needing memory cell
            cache.m_data_to_cell(m_data_to_cell);       // data wrote in memory cell
            cache.m_data_from_cell(m_data_from_cell);   // data from memory cell
            cache.m_ready(m_ready);
            cache.m_save_all(m_save_data);
            
            // 3. COMANDS DECODER
            cd.clk(clk);
            cd.sreset(sreset);
            cd.cmd(cmd);
            cd.operand1(operand1);
            cd.operand2(operand2);
            cd.data_cell(data_cell);
            
            // 5. ALU
            alu.clk(clk);
            alu.sreset(sreset);
            // to command decoder
            alu.cmd(cmd);
            alu.operand1(operand1);
            alu.operand2(operand2);        
            // to memory controller
            // --flag
            alu.alu_fbs(fbs);
            alu.alu_fss(fss);
            alu.alu_fes(fes);
            alu.alu_fnes(fnes);
            alu.alu_fbes(fbes);
            alu.alu_fses(fses);
            // --registers and acums
            alu.alu_reg_id(reg_id);
            alu.alu_reg_read(reg_read);
            alu.alu_reg_write(reg_write);
            alu.alu_reg_data_to(reg_data_to);
            alu.alu_reg_data_from(reg_data_from);
                
                
            // 6. MEMORY CONTROLLER
            mc.clk(clk);
            mc.sreset(sreset);
            // to command decoder
            mc.cd_cmd(cmd);
            mc.cd_operand1(operand1);
            mc.cd_operand2(operand2);        
            mc.cd_data_cell(data_cell);
            // to alu
            // --flags
            mc.alu_fbs(fbs);
            mc.alu_fss(fss);
            mc.alu_fes(fes);
            mc.alu_fnes(fnes);
            mc.alu_fbes(fbes);
            mc.alu_fses(fses);
            // -- registers and acums
            mc.alu_reg_id(reg_id);
            mc.alu_reg_read(reg_read);
            mc.alu_reg_write(reg_write);
            mc.alu_reg_data_to(reg_data_to);
            mc.alu_reg_data_from(reg_data_from);
            // to cache
            mc.ch_read(mc_read);
            mc.ch_write(mc_write);
            mc.ch_addr_cell(mc_addr_cell);
            mc.ch_data_to_cell(mc_data_to_cell);
            mc.ch_data_from_cell(mc_data_from_cell);
            mc.ch_ready(mc_ready);
            mc.ch_save_data(mc_save_data);
            // to io_module
            mc.io_read(io_read);
            mc.io_write(io_write);
            mc.io_data_to_reg(io_data_to_reg);
            mc.io_data_from_reg(io_data_from_reg);

            // 7.IO_MODULE
            io.clk(clk);
            io.sreset(sreset);
            // to mc
            io.io_read(io_read);
            io.io_write(io_write);
            io.io_data_to_reg(io_data_to_reg);
            io.io_data_from_reg(io_data_from_reg);
            
            io.user_read(user_read);
            io.user_write(user_write);
            io.user_data_to_reg(user_data_to_reg);
            io.user_data_from_reg(user_data_from_reg); 

            cout<<"Signals initializing is finished"<<endl;
        }

    };
#endif