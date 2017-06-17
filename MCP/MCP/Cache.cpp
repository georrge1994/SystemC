#include "Cache.h"

void Cache::clear_cache_memory(){
    // reset cashe
    for(int i = 0; i < (sizeof(cache_memory)/sizeof(*cache_memory)); i++){
        cache_memory[i][16] = 0;
        cache_memory[i][17] = 0;
    }
}

void Cache::workWithCache()
{
    while (true) {
        mc_data_from_cell = 0;
        if(delay == 1){
            mc_ready = true;
            memory_lock = false;
            m_save_all = false;
            
            
            // wait input signal "save ram in harddisk"
            if(!stop_and_save)
                stop_and_save = mc_save_all.read();


            if(!updating_cache && (mc_read.read() || mc_write.read()) && !stop_and_save){
                addr = mc_addr_cell.read();
                tag = (addr >> 4) & 255;        // 12-4 bits
                bias = addr & 15;               // 3-0 bits

                id_cache_row = tag & 31;        // index in table (8-4 bit)

                inner_read = mc_read.read();
                inner_write = mc_write.read();
                inner_cell = mc_data_to_cell.read();
            }

            // checking row in table
            if(cache_memory[id_cache_row][16] == tag && !isEmpty)
                cache_hit = true;
            else
                cache_hit = false;


            // this part need for correct saving pages, when input signal "save all" is true 
            if(stop_and_save && !row_is_saving){    

                // reset inner registers
                id_cache_row = pointer_for_end;
                wait_data = false;

                // set index of pages array in correct location
                if(pointer_for_end == 32){
                    pointer_for_end = 0;
                    stop_and_save = false;
                    save_in_next_clk = true;
                }else{
                    pointer_for_end++;
                }
               /// cout<<"pointer_for_end " <<pointer_for_end<<endl;
            }

            if(save_in_next_clk && m_ready){
                m_save_all = true;
                save_in_next_clk = false;
            }


            if(cache_hit && !stop_and_save){
                //cout<<"==================== " << addr<<endl;
                // writing
                if(inner_write){
                    cache_memory[id_cache_row][bias] = inner_cell;
                    cache_memory[id_cache_row][17] = 1;                             // cell is updated
                    inner_write = false;
                   // cout<<"write bro " << inner_cell<<endl;
                }

                // reading
                if(inner_read){
                    mc_data_from_cell = cache_memory[id_cache_row][bias];
                    //cout<<"read bro " << cache_memory[id_cache_row][bias] <<endl;
                    inner_read = false;
                }

            }

            // this part2 need for correct saving pages, when input signal "save all" is true 
            if(stop_and_save && cache_memory[id_cache_row][17] == 1){
                row_is_saving = true;
            }

            // if cache is missing, we should updating cache block
            if(!cache_hit && (inner_read || inner_write || isEmpty) || stop_and_save){

                //set the updating flag 
                updating_cache = true;
                mc_ready = false;
                memory_lock = false;
                m_write = false;
                m_read = false;

                // save a old row in the memory
                if(cache_memory[id_cache_row][17] == 1 && m_ready){

                    // set signals to writing in memory
                    m_write = true;
                    m_addr_cell = (cache_memory[id_cache_row][16] << 4) | id_cell;   // set addr
                    m_data_to_cell = cache_memory[id_cache_row][id_cell];           // set data

                    id_cell++;                                                      // incrementing counter id of the cells
                    memory_lock = true;

                    // if row is save, reset flag "row is changed"
                    if(id_cell == 16){
                        cache_memory[id_cache_row][17] = 0;
                        id_cell = 0;
                        row_is_saving = false;
                    }

                }

                // read a new row
                if(cache_memory[id_cache_row][17] == 0 && !memory_lock && m_ready && !stop_and_save){

                    // wait data from real memory
                    if(wait_data){
                        cache_memory[id_cache_row][id_cell] = m_data_from_cell.read();
                        id_cell++;
                    }

                    // if row is read, the change a tag
                    if(id_cell == 16){
                        cache_memory[id_cache_row][16] = tag;
                        id_cell = 0;
                        wait_data = false;
                        isEmpty = false;
                        updating_cache = false;
                    }else{

                        // set signals to reading from memory
                        m_read = true;
                        m_addr_cell = (tag << 4) | id_cell;                             // set addr

                        wait_data = true;
                    }

                }

            }
            delay = 0;
        }else{
            delay++;
            
            if(true)
                mc_ready = false;
        }
        wait();
    }
}