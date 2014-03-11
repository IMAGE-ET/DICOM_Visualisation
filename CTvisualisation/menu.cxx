/*
 * menu.cxx
 *
 *  Created on: 10 Mar 2014
 *      Author: craig
 */

#include "segmentationMethods.h"
#include "global.h"
/**
*@title MainMenu
*@description the methods to run once a menu option has been selected
**/
void mainMenu(int value)
{
	if(value == 8)
	{
		exit(0);
	}

	switch ( value )
	{
		case 1:
			thresholdSegmentation();
			break;
		case 2:
			connectedThresholdSegmentation();
			break;
		case 3:
			confidenceConnectedSegmentation();
			break;
		case 4:
			viewSegmentedData();
			break;
		case 5:
			viewSegmented = 0;
			break;
		case 6:
			viewVolume = 1;
			//rotate the volume to the original orientation
			beta = -90;
			alpha = 180;
			viewCrossSection = 0;
			break;
		case 7:
			viewCrossSection = 1;
			beta = 0;
			alpha = 0;
			viewVolume = 0;
			break;
	}
}
