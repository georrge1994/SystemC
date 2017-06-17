#include "systemc.h"

#ifndef ALU_H
#define	ALU_H

SC_MODULE(Alu){
	sc_in_clk clk;
	sc_in<bool> sreset;
        
        // to command decoder
        sc_in<sc_uint<8>> cmd;
        sc_in<sc_uint<12>> operand1;
        sc_in<sc_uint<12>> operand2;
        
        // memory controller
        sc_in<sc_uint<4>> alu_reg_id;
        sc_in<bool> alu_reg_read;                        // read from cache cell
        sc_in<bool> alu_reg_write;                       // write to cache in cell
        sc_in<sc_uint<32>> alu_reg_data_to;
        sc_out<sc_uint<32>> alu_reg_data_from;   
        sc_out<bool> alu_fbs;
        sc_out<bool> alu_fss;
        sc_out<bool> alu_fes;
        sc_out<bool> alu_fnes;
        sc_out<bool> alu_fbes;
        sc_out<bool> alu_fses;
        
        //registers
        unsigned int registers[16];   
        
        // fbs-1, fss-2, fes-3, fnes-4, fbes-5, fses-6
        bool fbs, fss, fes, fnes, fbes, fses;
        
        // Mathematical actions (result write in a)
        void _add(int a, int b);   
        void _sub(int a, int b);
        void _mult(int a, int b);
        void _div(int a, int b);
        void _inc(int a);
        void _dec(int a);
        
        // Boolean actions
        void _xor(int a, int b);
        void _and(int a, int b);
        void _or(int a, int b);
        void _not(int a);
        
        // shifts
        void _rs(int a);
        void _ls(int a);
        void _rsn(int a, int n);
        void _lsn(int a, int n);
        
        // comparisons
        void _fbs(int first, int two);
        void _fss(int first, int two);
        void _fbes(int first, int two);
        void _fses(int first, int two);
        void _fes(int first, int two);
        void _fnes(int first, int two);

        void calculate();
        void clear_regs_and_flags();
        
        int comand, type_slave, cmd_group, id_cmd; 
        int id_reg, op1, op2;
    
	SC_CTOR(Alu):
		clk("clk"),
		sreset("reset"),
                cmd("cmd"),
                operand1("operand1"),
                operand2("operand2"),
                alu_reg_id("alu_reg_id"),
                alu_reg_read("alu_reg_read"),
                alu_reg_write("alu_reg_write"),
                alu_reg_data_to("alu_reg_data_to"),
                alu_reg_data_from("alu_reg_data_from"),
                fbs("fbs"),
                fss("fss"),
                fes("fes"),
                fnes("fnes"),
                fbes("fbes"),
                fses("fses")
	{
            SC_CTHREAD(calculate, clk.pos());
            cout << "Executing new" << endl;
            async_reset_signal_is(sreset,false);
            clear_regs_and_flags();
	}

};
#endif 