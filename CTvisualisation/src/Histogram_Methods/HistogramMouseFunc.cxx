#include <GL/glut.h>
#include "../../headers/global.h"

/**
 * @title MouseFunc
 * @description called on mouse button
 */
void HistogramMouseFunc(int button, int state, int x, int y)
{

	if(GLUT_LEFT_BUTTON == button && state == GLUT_DOWN)
	{
		//calculate the minimum value of the histogram
		if(x >= 71 && x <= 330)
		{
			minimum = (((x - 71) * (maximum - minimum))/(330-71)) + minimum;
		}
	}

	if(GLUT_RIGHT_BUTTON == button && state == GLUT_DOWN)
	{
		//calculate the maximum value of the histogram
		maximum = (((x - 71) * (maximum - minimum))/(330-71)) + minimum;
	}

	//re-generate the highest histogram value
	highFreq = 0;
	for(int i = 0; i < arraySize; i++)
	{
		if(xArray[i] >= minimum && xArray[i] <= maximum)
		{
			if(yArray[i] > highFreq)
			{
				highFreq = yArray[i];
			}
		}
	}
}
