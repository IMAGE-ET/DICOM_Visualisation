#include "../../headers/global.h"

/**
 * @title HistogramKeyboardFunc
 * @description called on keyboard use
 * 
 *  Author: craig parkinson
 *  Organisation: Bangor University
 *  Supervisor: Franck Vidal
 */

void histogramKeyboardFunc(unsigned char key, int x, int y)
{
	// reset the histogram to its original values
	if(key == 'r' || key == 'R')
	{
		maximum = tempMax;
		minimum = tempMin;
		highFreq = tempHighFreq;
	}
}
