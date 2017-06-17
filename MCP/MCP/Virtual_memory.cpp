#include "Virtual_memory.h"

void Virtual_memory::workWithMemory()
{    
    while (true) {
        // reset read/write signals for memory
        ram_out_read = false;       
        ram_out_write = false;
        hm_out_read = false;
        hm_out_write = false;
        
        // free memory flag
        memory_is_lock = false;
        
        // reset output signal
        ch_out_data_from_cell = 0;

        // wait input signal "save ram in harddisk"
        if(!stop_and_save)
            stop_and_save = ch_in_save_all.read();


        // if ram memory and hard disk is ready, virtual memory is ready too
        if(ram_in_ready && hm_in_ready)
            ch_out_ready = true;
        else
            ch_out_ready = false;

        // read input signal, if:
        // 1. page is not reloading now
        // 2. have got input read/write signal
        // 3. ram is not saving pages to hard disk now
        if(!changing_page && (ch_in_read.read() || ch_in_write.read()) && !stop_and_save){
            
            // save addres cell and address page to inner register
            addr = ch_in_addr_cell.read();
            addr_page = (addr >> 8) & 15;                                           // 12-8 bits

            // save input signals to buffers
            inner_read = ch_in_read.read();
            inner_write = ch_in_write.read();
            inner_cell = ch_in_data_to_cell.read();

            // reset hit glag
            page_hit = false;

            // set hit flag true/false by situation
            if(ram_is_empty)
                page_hit = false;
            else{
                // set index of pages array in correct location
                page_index = pointer;
                
                // checking a page hit
                for(int i = 0; i < 4; i++){
                    if(addr_page == pages[i][0]){
                        page_index = i; // reload index if hit
                        page_hit = true;
                    }
                }
            }

            // set output signal, vm is not ready
            ch_out_ready = false;
        }

        
        // this part need for correct saving pages, when input signal "save all" is true 
        if(stop_and_save && !page_is_saving){    
            
            // reset inner registers
            addr_page = 0;
            wait_data = false;
            
            // set index of pages array in correct location
            page_index = pointer_for_end;
            if(pointer_for_end == 4){
                pointer_for_end = 0;
                stop_and_save = false;
            }else{
                pointer_for_end++;
            }
        }

        // part for hit-situation
        if(page_hit && ram_in_ready && !stop_and_save){

            //wait data from ram
            if(wait_data){
                // set output singals
                ch_out_ready = true;
                ch_out_data_from_cell = ram_in_data_from_cell;
                
                // reset an inner flag
                wait_data = false;
            }

            // writing
            if(inner_write){
                // set writing signals to ram memory
                ram_out_write = true;
                ram_out_data_to_cell = inner_cell;
                ram_out_addr_cell = (addr & 255) | (page_index << 8);           // 10-bit addr in ram
                
                // set signals to cache
                ch_out_data_from_cell = 0;
                ch_out_ready = false;                                           // set to cache "vm is not ready"
                
                // set the inner registers
                pages[page_index][1] = 1;                                       // page is changed
                inner_write = false;                                            // reset the inner flag of the write

            }

            // reading
            if(inner_read){
                // signals to read data from ram
                ram_out_read = true;
                ram_out_addr_cell = (addr & 255) | (page_index << 8);           // 10-bit addr in ram 
                
                // set signals to cache
                ch_out_data_from_cell = 0;
                ch_out_ready = false;                                           // set to cache "vm is not ready"
                
                // set the inner registers
                inner_read = false;                                             // reset the inner flag of the read
                wait_data = true;                                                   // flag to wait data from ram
            }

        }
  
        // this part2 need for correct saving pages, when input signal "save all" is true 
        if(stop_and_save && pages[page_index][1] == 1){
            page_is_saving = true;
        }

        // if page is missing, then reload page
        if((!page_hit || stop_and_save) && ram_in_ready && hm_in_ready){

            //set the updating page flag 
            changing_page = true;

            // save a old page to hard disk 
            if(pages[page_index][1] == 1){

                if(wait_data){
                    // write data from ram in hardware memory
                    hm_out_write = true;
                    hm_out_data_to_cell = ram_in_data_from_cell;
                    hm_out_addr_cell = (pages[page_index][0] << 8) + bias;          // 12-bit addr in hm
                    memory_is_lock = true;                                          // hardware is locked
                    bias++;
                    wait_data = false;
                }

                if(bias < page_size){

                    // signals to read data from ram
                    ram_out_read = true;
                    ram_out_addr_cell = (page_index << 8) + bias;                   // 10-bit addr in ram 
                    wait_data = true;                                               // flag to wait data from ram

                }else {

                    // if page is saved, reset flag "row is changed"
                    pages[page_index][0] = addr_page;
                    pages[page_index][1] = 0;
                    bias = 0;

                    page_is_saving = false;
                }
                ch_out_ready = false;
            }

            // read a new row
            if(pages[page_index][1] == 0 && !memory_is_lock && !stop_and_save){

                if(wait_data){
                    // write data from ram in hardware memory
                    ram_out_write = true;
                    ram_out_data_to_cell = hm_in_data_from_cell;
                    ram_out_addr_cell = (page_index << 8) + bias;                      // 10-bit addr in hm                
                    bias++;
                    wait_data = false;
                }
                if(bias < page_size){

                    // signals to read data from ram
                    hm_out_read = true;
                    hm_out_addr_cell = (addr & 3840) + bias;                        // 12-bit addr in ram 
                    wait_data = true;                                               // flag to wait data from ram

                }else{

                    ram_is_empty = false;
                    bias = 0;
                    changing_page = false;                                          // page is reload
                    pages[page_index][1] = 0;
                    pages[page_index][0] = addr_page;
                    page_hit = true;

                    pointer++;                                                      // incriment page-pointer

                    if(pointer == 4)
                        pointer = 0; 

                }
                ch_out_ready = false;
            }

        }

        wait();
    }
}