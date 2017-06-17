/* 
 * File:   fifoparamextend.h
 * Author: student
 *
 * Created on March 17, 2017, 11:56 AM
 */
#include <systemc.h>
#include "fifoparam.h"

#ifndef FIFOPARAMEXTEND_H
#define	FIFOPARAMEXTEND_H

template <typename T>
struct fifoparamextend : public fifoparam<T> {
public:
	sc_out<bool> almost_empty;
	sc_out<bool> almost_full;

	unsigned int almost;
	//void processing();
	void almostfe();

	SC_HAS_PROCESS(fifoparamextend);

	fifoparamextend(sc_module_name name, int _size, unsigned int _almost):
	fifoparam<T>(name, _size),
		almost(_almost)
	{
		cout << "Executing new" << endl;
		SC_METHOD(almostfe);
		sc_module::sensitive << fifoparam<T>::is_write << fifoparam<T>::is_read << fifoparam<T>::clk;            
	}
};

template <typename T>
void fifoparamextend<T>::almostfe()
{
	if(fifoparam<T>::num_elements <= almost)
		almost_empty = true;
	else almost_empty = false;
	if(fifoparam<T>::num_elements >= fifoparam<T>::max - almost)
		almost_full = true;
	else almost_full = false;
}
#endif	
