#include "systemc.h"
#include "Top.h"

#define soft_assert(signal, expected) \
      if (signal.read() != expected) { \
          cerr << "@" << sc_time_stamp() << " Check failed. Expected: " << expected << ". Actual: " << signal.read() << ".\n" << endl; \
      }
sc_clock clk("clock", 4, SC_NS);
    sc_signal<bool> sreset;                         // restart

// virtual memory <=> real memory
// to ram memory
sc_signal<bool> ram_out_read;                  // read from cell
sc_signal<bool> ram_out_write;                 // write in cell
sc_signal<sc_uint<10> > ram_out_addr_cell;
sc_signal<sc_uint<32> > ram_out_data_to_cell;
sc_signal<sc_uint<32> > ram_in_data_from_cell;
sc_signal<bool> ram_in_ready;
// to hard memory
sc_signal<bool> hm_out_read;                  // read from cell
sc_signal<bool> hm_out_write;                 // write in cell
sc_signal<sc_uint<12> > hm_out_addr_cell;
sc_signal<sc_uint<32> > hm_out_data_to_cell;
sc_signal<sc_uint<32> > hm_in_data_from_cell;
sc_signal<bool> hm_in_ready;
// User <-> io_module
sc_signal<bool> user_read;                        
sc_signal<bool> user_write;                       
sc_signal<sc_uint<12> > user_data_to_reg;        
sc_signal<sc_uint<12> > user_data_from_reg;  

Top top("Top");
    
// memory
int ram_memory[1024];
int hardware_memory[4096];


int TIMEOUT_RAM = 4;   
int TIMEOUT_HARD_MEMORY = 7;  
int CYCLE_INDEX = TIMEOUT_HARD_MEMORY + 1;
int ram_counter = 0;
int hm_counter = 0;
bool ram_run = false, hm_run = false;
bool buffer_ram_read = false, buffer_ram_write = false, buffer_hm_read = false, buffer_hm_write = false;

void setSignal(){
    
    if(ram_out_read || ram_out_write){
        ram_run = true;
        ram_in_ready = false; 
        buffer_ram_read = ram_out_read.read();
        buffer_ram_write = ram_out_write.read();
    }
    
    if(ram_run){
        ram_counter++;
    }
    
    if(ram_counter == TIMEOUT_RAM && buffer_ram_read)
        ram_in_data_from_cell = ram_memory[ram_out_addr_cell.read()];

    
    if(ram_counter == TIMEOUT_RAM && buffer_ram_write)
        ram_memory[ram_out_addr_cell.read()] = ram_out_data_to_cell.read();
    
    
    if(ram_counter == TIMEOUT_RAM){
        ram_counter = 0;
        ram_run = false;
        buffer_ram_read = false;
        buffer_ram_write = false;
        ram_in_ready = true;
    }
    
    if(hm_run){
        hm_counter++;
    }
    
    if(hm_out_read || hm_out_write){
        hm_run = true;
        hm_in_ready = false; 
        buffer_hm_read = hm_out_read.read();
        buffer_hm_write = hm_out_write.read();
    }
    
    if(hm_counter == TIMEOUT_HARD_MEMORY && buffer_hm_read){        
        hm_in_data_from_cell = hardware_memory[hm_out_addr_cell.read()];
    }
    
    
    if(hm_counter == TIMEOUT_HARD_MEMORY && buffer_hm_write) 
        hardware_memory[hm_out_addr_cell.read()] = hm_out_data_to_cell.read();
    
    if(hm_counter == TIMEOUT_HARD_MEMORY){
        hm_counter = 0;
        hm_run = false;
        buffer_hm_read = false;
        buffer_hm_write = false;
        hm_in_ready = true;
    }

}

void printRAM(){
    cout <<"\n\n\n\n*******************************printRAM*************************" <<endl;
    int i,j;
    for(j = 0; j < 32; j++){
        for(i = 0; i < 32; i++){
            cout<< ram_memory[j*32 + i] <<" ";
        }
        if((j*32 + i) % 256 == 0)
            cout<<"_"<< endl;
        else
            cout<<endl;
    }
}

void printHARDWARE(){
    cout <<"\n\n\n\n*******************************printHARDWARE********************" <<endl;
    int i,j;
    for(j = 0; j < 128; j++){
        for(i = 0; i < 32; i++){
            cout<< hardware_memory[j*32 + i] <<" ";
        }
        if((j*32 + i) % 256 == 0)
            cout<<"_"<< endl;
        else
            cout<<endl;
    }
}

int sc_main(int argc, char* argv[]) {
    
    top.clk(clk);
    top.sreset(sreset);
    // to ram memory
    top.ram_out_read(ram_out_read);
    top.ram_out_write(ram_out_write);
    top.ram_out_addr_cell(ram_out_addr_cell);
    top.ram_out_data_to_cell(ram_out_data_to_cell);
    top.ram_in_data_from_cell(ram_in_data_from_cell);
    top.ram_in_ready(ram_in_ready);
    // to hard memory
    top.hm_out_read(hm_out_read);
    top.hm_out_write(hm_out_write);
    top.hm_out_addr_cell(hm_out_addr_cell);
    top.hm_out_data_to_cell(hm_out_data_to_cell);
    top.hm_in_data_from_cell(hm_in_data_from_cell);
    top.hm_in_ready(hm_in_ready);
    // io_module
    top.user_read(user_read);
    top.user_write(user_write);
    top.user_data_to_reg(user_data_to_reg);
    top.user_data_from_reg(user_data_from_reg);

    sc_trace_file *wf = sc_create_vcd_trace_file("MPC_waveform");
    sc_trace(wf, clk, "0_clock");
    sc_trace(wf, sreset, "0_sreset");
    // virtual memory <=> real memory
    // ram
    sc_trace(wf, ram_out_read, "1_ram_out_read");
    sc_trace(wf, ram_out_write, "2_ram_out_write");
    sc_trace(wf, ram_out_addr_cell, "3_ram_out_addr_cell");
    sc_trace(wf, ram_out_data_to_cell, "4_ram_out_data_to_cell");
    sc_trace(wf, ram_in_data_from_cell, "5_ram_in_data_from_cell");
    sc_trace(wf, ram_in_ready, "6_ram_in_ready");
    // to hard disk
    sc_trace(wf, hm_out_read, "7_hard_out_read");
    sc_trace(wf, hm_out_write, "8_hard_out_write");
    sc_trace(wf, hm_out_addr_cell, "9_hard_out_addr_cell");
    sc_trace(wf, hm_out_data_to_cell, "10_hard_out_data_to_cell");
    sc_trace(wf, hm_in_data_from_cell, "11_hard_in_data_from_cell");
    sc_trace(wf, hm_in_ready, "12_hard_in_ready");
    // virtual memory <-> cache
    sc_trace(wf, top.m_read, "13_memory_read");
    sc_trace(wf, top.m_write, "14_memory_write");
    sc_trace(wf, top.m_addr_cell, "15_memory_addr_cell");
    sc_trace(wf, top.m_data_to_cell, "16_memory_data_to_cell");
    sc_trace(wf, top.m_data_from_cell, "17_memory_data_from_cell");
    sc_trace(wf, top.m_ready, "18_memory_ready");
    sc_trace(wf, top.m_save_data, "19_memory_save_data");
    // cache <-> memory controller
    sc_trace(wf, top.mc_read, "20_cache_read");
    sc_trace(wf, top.mc_write, "21_cache_write");
    sc_trace(wf, top.mc_addr_cell, "22_cache_addr_cell");
    sc_trace(wf, top.mc_data_to_cell, "23_cache_data_to_cell");
    sc_trace(wf, top.mc_data_from_cell, "24_cache_data_from_cell");
    sc_trace(wf, top.mc_ready, "25_cache_ready");
    sc_trace(wf, top.mc_save_data, "26_cache_save_data");  
    // command decoder <-> alu and memory controller
    sc_trace(wf, top.cmd, "27_cmd");
    sc_trace(wf, top.operand1, "28_operand1");
    sc_trace(wf, top.operand2, "29_operand2");
    sc_trace(wf, top.data_cell, "30_data_cell");
    // alu <-> memory controller
    sc_trace(wf, top.reg_id, "31_alu_reg_id");
    sc_trace(wf, top.reg_read, "32_alu_reg_read");
    sc_trace(wf, top.reg_write, "33_alu_reg_write"); 
    sc_trace(wf, top.reg_data_to, "34_alu_reg_data_to");
    sc_trace(wf, top.reg_data_from, "35_alu_reg_data_from");
    sc_trace(wf, top.fbs, "36_alu_fbs");
    sc_trace(wf, top.fss, "37_alu_fss");
    sc_trace(wf, top.fes, "38_alu_fes"); 
    sc_trace(wf, top.fnes, "39_alu_fnes");
    sc_trace(wf, top.fbes, "40_alu_fbes");
    sc_trace(wf, top.fses, "41_alu_fses"); 
    // memory controller <-> io_module
    sc_trace(wf, top.io_read, "42_io_read"); 
    sc_trace(wf, top.io_write, "43_io_write");
    sc_trace(wf, top.io_data_to_reg, "44_io_data_to_reg");
    sc_trace(wf, top.io_data_from_reg, "45_io_data_from_reg"); 
    // User <-> io_module
    sc_trace(wf, user_read, "46_user_read"); 
    sc_trace(wf, user_write, "47_user_write");
    sc_trace(wf, user_data_to_reg, "48_user_data_to_reg");
    sc_trace(wf, user_data_from_reg, "49_user_data_from_reg"); 
 
    for(int i = 0; i < 4095; i++)
        hardware_memory[i] = i;
    
    int n = 100;
    for(int i = 200; i< 200 + n + 1; i++)
        hardware_memory[i] = 1;
    for(int i = 200; i< 200 + n + 1; i = i + 1)
        hardware_memory[i] = 2;
    for(int i = 200; i< 200 + n + 1; i = i + 4)
        hardware_memory[i] = 3;
        for(int i = 200; i< 200 + n + 1; i = i + 10)
        hardware_memory[i] = 10;
    // programm
    hardware_memory[0] =  1392508938;	// mov reg0, m_width   # m_width
    hardware_memory[1] =  1392513034;	// mov reg1, m_height  # m_height
    hardware_memory[2] =  1392517123;	// mov reg2, w_width   # w_width
    hardware_memory[3] =  1392521219;	// mov reg3, w_height  # w_height
    hardware_memory[4] =  855695362;	// nope cmd (mov reg14, reg2)       FIX
    hardware_memory[5] =  1392566280;   //mov reg14, #8    # count of elements in window    FIX
    hardware_memory[6] =  1392570370;	// mov reg15, #2       
    hardware_memory[7] =  855654402;	// mov reg4, reg2      # w_width
    hardware_memory[8] =  1392558208;	// # bias for window
    hardware_memory[9] =  1392562376;	// # bias for memory   01010011 000000001101 000011001000
    hardware_memory[10] =  838877199;	// div reg4, reg15     # edgex = w_width/2
    hardware_memory[11] =  855658499;	// mov reg5, reg3      # w_height
    hardware_memory[12] =  838881295;	// div reg5, reg15     # edgey = w_height/2
    hardware_memory[13] =  855662596;	// mov reg6, reg4      # x = edgex    
   //!X1
    hardware_memory[14] =  855666693;	// mov reg7, reg5      # y = edgey                   
   //!X2                                                              
    hardware_memory[15] =  1392541696;	// mov reg8, #0        # fx = 0          
    //!X3                                                                              
    hardware_memory[16] =  1392545792;	// mov reg9, #0        # fy = 0                       
    //!X4                                                
    // window[fx][fy] := matrix[x + fx - edgex][y + fy - edgey]                                   
    hardware_memory[17] =  855678982;	// mov reg10, reg6     # x                                                                             
    hardware_memory[18] =  33595400;	// add reg10, reg8     # x + fx
    hardware_memory[19] =  302030852;	// sub reg10, reg4     # x + fx - edgex
    hardware_memory[20] =  570466304;	// mult reg10,reg0     # (x + fx - edgex) * m_width
    hardware_memory[21] =  33595399;	// add reg10, reg7     # (x + fx - edgex) * m_width + y 
    hardware_memory[22] =  33595401;	// add reg10, reg9     # (x + fx - edgex) * m_width + y + fy
    hardware_memory[23] =  302030853;	// sub reg10, reg5     # (x + fx - edgex) * m_width + y + fy - edgey
    hardware_memory[24] =  33595405;	// add reg10, #200     # 200 + (x + fx - edgex) * m_width + y + fy - edgey = bias in memory to element
    hardware_memory[25] =  117481482;	// movx reg10, reg10   # reg10 = memory[reg10]
    hardware_memory[26] =  855683080;	// mov reg11, reg8     # fx
    hardware_memory[27] =  570470402;	// mult reg11, reg2    # fx * w_width
    hardware_memory[28] =  33599497;	// add reg11, reg9     # fx * w_width + fy
    hardware_memory[29] =  33599500;	// add reg11, #128     # 128 + fx * w_width + fy = bias in the window memory
    hardware_memory[30] =  654352395;	// movm reg10, reg11   # memory[reg11] = reg10 
    // цикл 4
    hardware_memory[31] =  1107333120;	// inc reg9
    hardware_memory[32] =  402690051;	// fss reg9, reg3      # if reg9 < w_height
    hardware_memory[33] =  352329745;	// jmp fss, X4(17)	//00010101 000000000010 000000010001
    // цикл 3
    hardware_memory[34] =  1107329024;	// inc reg8
    hardware_memory[35] =  402685954;	// fss reg8, reg2      # if reg8 < w_width
    hardware_memory[36] =  352329744;	// jmp fss, X3(16)
    hardware_memory[37] =  889458688;	// call save_reg(65) 00110101 000001000001 000000000000
    hardware_memory[38] =  855650318;	// mov reg3, reg14;    # count of the elements in window
    hardware_memory[39] =  889556992;	// call sort (89) 00110101 000001011001 000000000000
    hardware_memory[40] =  889507840;	// call read_reg(77) 00110101 000001001101 000000000000
    // matrix[x][y] := window[w_width / 2][w_height / 2]
    hardware_memory[41] =  855678978;	// mov reg10, reg2     # w_width
    hardware_memory[42] =  838901775;	// div reg10, reg15    # w_width / 2
    hardware_memory[43] =  570466306;	// mult reg10, reg2    # w_width / 2 * w_width
    hardware_memory[44] =  1392553987;	// mov reg11, #3       # w_height    FIX
    hardware_memory[45] =  838905871;	// div reg11, reg15    # w_height / 2
    hardware_memory[46] =  33595403;	// add reg10, reg11    # w_width / 2 * w_width + w_height / 2
    hardware_memory[47] =  33595404;	// add reg10, #128     # 128 + w_width / 2 * w_width + w_height / 2
    hardware_memory[48] =  117481482;	// movx reg10, reg10   # reg10 = memory[reg10]
    hardware_memory[49] =  855683078;	// mov reg11, reg6     # x
    hardware_memory[50] =  570470400;	// mult reg11, reg0    # x * m_width
    hardware_memory[51] =  33599495;	// add reg11, reg7     # x * m_width + y
    hardware_memory[52] =  33599501;	// add reg11, #200
    hardware_memory[53] =  654352395;	// movm reg10, reg11   # memory[reg11] = reg10 
    // цикл 2
    hardware_memory[54] =  1107324928;	// inc reg7
    hardware_memory[55] =  855699457;	// mov reg15, reg1     # m_height
    hardware_memory[56] =  302051333;	// sub reg15, reg5     # m_height - edgey
    hardware_memory[57] =  402681871;	// fss reg7, reg15     # if reg7 < reg15
    hardware_memory[58] =  352329743;	// jmp fss, X2(15) 
    // цикл 1
    hardware_memory[59] =  1107320832;	// inc reg6
    hardware_memory[60] =  855699456;	// mov reg15, reg0     # m_width
    hardware_memory[61] =  302051332;	// sub reg15, reg4     # m_width - edgex
    hardware_memory[62] =  402677775;	// fss reg6, reg15     # if reg6 < reg15
    hardware_memory[63] =  352329742;	// jmp fss, X1(14)
    hardware_memory[64] =  1157627904;	// end
    //!save_reg
    hardware_memory[65] =  587816960;	// mov #150, reg0                                                                                  
    hardware_memory[66] =  587821057;	// mov #151, reg1
    hardware_memory[67] =  587825154;	// mov #152, reg2
    hardware_memory[68] =  587829252;	// mov #153, reg4
    hardware_memory[69] =  587833349;	// mov #154, reg5
    hardware_memory[70] =  587837446;	// mov #155, reg6
    hardware_memory[71] =  587841543;	// mov #156, reg7
    hardware_memory[72] =  587845640;	// mov #157, reg8
    hardware_memory[73] =  587849737;	// mov #158, reg9
    hardware_memory[74] =  587853837;	// mov #159, reg13
    hardware_memory[75] =  587857935;	// mov #160, reg15
    hardware_memory[76] =  620756992;	// ret
    //!read_reg
    hardware_memory[77] =  1124073622;	// mov reg0, #150                                                                                  
    hardware_memory[78] =  1124077719;	// mov reg1, #151  
    hardware_memory[79] =  1124081816;	// mov reg2, #152
    hardware_memory[80] =  1124090009;	// mov reg4, #153
    hardware_memory[81] =  1124094106;	// mov reg5, #154
    hardware_memory[82] =  1124098203;	// mov reg6, #155
    hardware_memory[83] =  1124102300;	// mov reg7, #156
    hardware_memory[84] =  1124106397;	// mov reg8, #157
    hardware_memory[85] =  1124110494;	// mov reg9, #158
    hardware_memory[86] =  1124126879;	// mov reg13, #159
    hardware_memory[87] =  1124135072;	// mov reg15, #160
    hardware_memory[88] =  620756992;	// ret
    // Sort
    hardware_memory[89] = 1392508928;	// mov reg0, #0;        # init 
    hardware_memory[90] = 1392513024;	// mov reg1, #0;
    hardware_memory[91] = 1392517120;	// mov reg2, #0;
    hardware_memory[92] = 855691267;	// mov reg13, reg3;    	# n - 2
    hardware_memory[93] = 1375784960;	// dec reg13;
    hardware_memory[94] = 855654403;	// mov reg4, reg3;
    hardware_memory[95] = 1375748096;	// dec reg4;           	# n-1
    hardware_memory[96] = 1392529536;	// mov reg5, #128;      # start address
    hardware_memory[97] = 1392570369;	// mov reg15, #1;
    hardware_memory[98] = 84332544;		// jmp X3(109);
    hardware_memory[99] = 1107296256;	// inc reg0;           	# j++							!X1
    hardware_memory[100] = 134217741; 	// fbs reg0, reg13;    	# do while j <= n-2
    hardware_memory[101] = 352325756;	// jmp fbs, X6(124);        
    hardware_memory[102] = 1392517120;	// mov reg2, #0;       	# f = 0
    hardware_memory[103] = 1392513024;	// mov reg1, #0;       	# i = 0
    hardware_memory[104] = 1375748096;	// dec reg4;           	# n - 1 - j
    hardware_memory[105] = 84332544;	// jmp X3(109);
    hardware_memory[106] = 1107300352;	// inc reg1            	# i++							!X2
    hardware_memory[107] = 134221828;	// fbs reg1, reg4      	# do while i <= n - 1 - j
    hardware_memory[108] = 352325731;	// jmp fbs, X1(99)
    hardware_memory[109] = 855662593;	// mov reg6, reg1;     	# i					A[i]		!X3
    hardware_memory[110] = 33579013;	// add reg6, reg5;     	# i + a0
    hardware_memory[111] = 117469190;	// movx reg7, reg6;    	# memory[i + a0]
    hardware_memory[112] = 855670790;	// mov reg8, reg6;     	# i + a0				A[i + 1]
    hardware_memory[113] = 1107329024;	// inc reg8;           	# i + a0 + 1
    hardware_memory[114] = 117477384;	// movx reg9, reg8;    	# memory[i + a0 + 1]
    hardware_memory[115] = 134246409;	// fbs reg7, reg9;
    hardware_memory[116] = 352325750;	// jmp fbs, X4(118);
    hardware_memory[117] = 84381696;	// jmp X5(121);
    hardware_memory[118] = 654348294;	// movm reg9, reg6;	# swap                               !X4				
    hardware_memory[119] = 654340104;	// movm reg7, reg8; 
    hardware_memory[120] = 1392517121;	// mov reg2, #1;	# f = 1
    hardware_memory[121] = 1207967759;	// fes reg2, reg15;	#                               	 !X5
    hardware_memory[122] = 352325738;	// jmp fes, X2(106);	# if f == 1 the cycle is continues
    hardware_memory[123] = 84291584;	// jmp X1(99);
    hardware_memory[124] = 620756992;	// ret			#                       				!X6   
    
    // it he beggining 
    hm_in_ready = true;
    ram_in_ready = true;
        
    // read page from hardware memory
    for(int i = 0; i < 50000 * CYCLE_INDEX; i ++){
        setSignal();
        sc_start(4, SC_NS); 
    }

    printHARDWARE();
    cout<<endl;
    cout<<endl;
    cout<<endl;
    printRAM();
    return 0;
}