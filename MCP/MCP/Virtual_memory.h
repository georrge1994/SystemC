#include "systemc.h"

#ifndef Memory_H
#define	Memory_H
    SC_MODULE(Virtual_memory){

            sc_in_clk clk;                          // main clock
            sc_in<bool> sreset;                     // restart
            
            // to cache
            sc_in<bool> ch_in_read;                       // read from cell
            sc_in<bool> ch_in_write;                      // write in cell
            sc_in<sc_uint<12> > ch_in_addr_cell;
            sc_in<sc_uint<32> > ch_in_data_to_cell;
            sc_out<sc_uint<32> > ch_out_data_from_cell;
            sc_out<bool> ch_out_ready;
            sc_in<bool> ch_in_save_all;
            
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
            
            unsigned int pages[4][2];               // page-table   (addr + flag_changed)
            
            
            
            int page, addr;                        // local variables
            bool first_step = true;                // first step in the executing command

            // buffer for input signals
            bool inner_read = false;                        
            bool inner_write = false;                      
            int inner_cell;
    
            // flags
            bool changing_page = false;
            bool page_hit = false;
            bool ram_is_empty = true;
            bool memory_is_lock = false;
            bool wait_data = false;
            bool stop_and_save = false;
            bool page_is_saving = false;
            
            // local registers
            int addr_page = 0;       // addr of the page
            int page_index = 0;     // index in array
            int page_size = 256;
            int bias = 0;
            int pointer = 0;
            int pointer_for_end = 0;
            
            void workWithMemory();
            
            SC_CTOR(Virtual_memory) :
                    clk("clk"),
                    sreset("sreset"),
                    ch_in_read("ch_in_read"),
                    ch_in_write("ch_in_write"),
                    ch_in_addr_cell("ch_in_addr_cell"), 
                    ch_in_data_to_cell("ch_in_data_to_cell"),
                    ch_out_data_from_cell("ch_out_data_from_cell"),
                    ch_out_ready("ch_out_ready"),
                    ch_in_save_all("ch_in_save_all"),
                    ram_out_read("ram_out_read"),
                    ram_out_write("ram_out_write"),
                    ram_out_addr_cell("ram_out_addr_cell"),
                    ram_out_data_to_cell("ram_out_data_to_cell"),
                    ram_in_data_from_cell("ram_in_data_from_cell"),
                    ram_in_ready("ram_in_ready"),
                    hm_out_read("hm_out_read"),
                    hm_out_write("hm_out_write"),
                    hm_out_addr_cell("hm_out_addr_cell"),
                    hm_out_data_to_cell("hm_out_data_to_cell"),
                    hm_in_data_from_cell("hm_in_data_from_cell"),
                    hm_in_ready("hm_in_ready")
            {      
                SC_CTHREAD(workWithMemory, clk.pos());
                async_reset_signal_is(sreset,false);
                
                for(int i = 0; i < 4; i++){
                    pages[i][0] = 16;
                    pages[i][1] = 0;
                }
                cout<<"reset"<<endl;
            }
    };
#endif