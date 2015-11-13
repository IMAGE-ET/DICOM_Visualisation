/*
 * ConnectedThresholdSegmentation.cxx
 *
 *  Created on: 12 Mar 2014
 *      Author: craig
 */

#include <stdio.h>
#include "../../headers/global.h"
#include <ITK-4.8/itkConnectedThresholdImageFilter.h>

using namespace std;

/**
 * @title connectedThresholdSegmentation
 * @description segment the volume based on a seed value
 */
void connectedThresholdSegmentation()
{
	typedef itk::ConnectedThresholdImageFilter< volumeImageType,binaryVolumeType > ConnectedFilterType;
	ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
	connectedThreshold->SetInput(volumeImage);

	cout << "Please enter the lower threshold value followed by a <return> or <enter>: ";
    float lowerThreshold = tempMin;

	while ( !(cin >> lowerThreshold) || (lowerThreshold < tempMin))
    {
       	// Enter this loop if input fails because of invalid data.

		cout << "Incorrect value, please try again: ";
        cin.clear ();   // reset the "failure" flag

        // The input "cursor" is still positioned at the beginning of
        // the invalid input, so we need to skip past it.
       	cin.ignore (1000, '\n');
   	}

	cout << "Please enter the Upper threshold value followed by a <return> or <enter>: ";
    float upperThreshold = tempMax;

	while ( ! (cin >> upperThreshold) || (upperThreshold < lowerThreshold) || (upperThreshold > tempMax))
    {
       	// Enter this loop if input fails because of invalid data.

       	cout << "Incorrect value, please try again: ";
       	cin.clear ();   // reset the "failure" flag

       	// The input "cursor" is still positioned at the beginning of
       	// the invalid input, so we need to skip past it.

       	cin.ignore (1000, '\n');
    }

	connectedThreshold->SetLower(lowerThreshold);
	connectedThreshold->SetUpper(upperThreshold);
	connectedThreshold->SetReplaceValue(175);

	float oR = 1.8;
	float yNr = yImages;
	float xNr = xImages;

	//take the texture coordinates and convert them to volume coordinates
	int nX = (((seedX-(-0.9))*xNr)/oR)+0;
	int nY = (((-seedY-(-0.9))*yNr)/oR)+0;

	volumeImageType::IndexType index;
	index[0] = nX;
	index[1] = nY;
	index[2] = zSliceNumber;

	connectedThreshold->SetSeed(index);
	connectedThreshold->Update();
	binaryVolume = connectedThreshold->GetOutput();
}
