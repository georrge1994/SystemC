#include "systemc.h"

#ifndef CACHE_H
#define CACHE_H

    SC_MODULE (Cache)
    {
        // [size of cache][ 16 data cell + 1 tag + 1 state]
        unsigned int cache_memory[32][18];           // cache 
        
        sc_in_clk clk;                              // main clock
        sc_in<bool> sreset;                         // restart
        
        // to mp
        sc_in<bool> mc_read;                        // read from cache cell
        sc_in<bool> mc_write;                       // write to cache in cell
        sc_in<sc_uint<12> > mc_addr_cell;           // addr needing memory cell
        sc_in<sc_uint<32> > mc_data_to_cell;        // data wrote in memory cell
        sc_out<sc_uint<32> > mc_data_from_cell;     // data from memory cell
        sc_out<bool> mc_ready;       
        sc_in<bool> mc_save_all; 
        
        // to virtual memory
        sc_out<bool> m_read;                        // if data in cache not exit, get it from memory
        sc_out<bool> m_write;                  
        sc_out<sc_uint<12> > m_addr_cell;           // addr needing memory cell
        sc_out<sc_uint<32> > m_data_to_cell;        // data wrote in memory cell
        sc_in<sc_uint<32> > m_data_from_cell;       // data from memory cell
        sc_in<bool> m_ready;
        sc_out<bool> m_save_all;
        
        
        bool cache_hit = false;
        bool updating_cache = false;
        bool memory_lock = false;
        bool wait_data = false;         // wait data from memory
        bool isEmpty = true;            // cashe is empty
        
        //buffer
        bool inner_read, inner_write;   
        int inner_cell;
        bool stop_and_save = false;
        bool row_is_saving = false;
        
        // the local variables
        int tag = 0, addr = 0, bias = 0, id_cache_row = 0;
        int id_cell = 0;
        int pointer_for_end = 0;
        int delay = 0;
        
        // trigger
        bool save_in_next_clk = false;
        void clear_cache_memory();
        void workWithCache();
        
        SC_CTOR(Cache) :
                    clk("clk"),
                    sreset("sreset"),
                
                    mc_read("mc_read"),
                    mc_write("mc_write"),
                    mc_addr_cell("mc_addr_cell"), 
                    mc_data_to_cell("mc_data_to_cell"),
                    mc_data_from_cell("mc_data_from_cell"),
                    mc_ready("mc_ready"),
                    mc_save_all("mc_save_all"),
                
                    m_read("m_read"),
                    m_write("m_write"),
                    m_addr_cell("m_addr_cell"), 
                    m_data_to_cell("m_data_to_cell"),
                    m_data_from_cell("m_data_from_cell"),
                    m_ready("m_ready"),
                    m_save_all("m_save_all")
        {
            SC_CTHREAD(workWithCache, clk.pos());
            async_reset_signal_is(sreset,false);
            clear_cache_memory();
        }
    };
    
#endif
