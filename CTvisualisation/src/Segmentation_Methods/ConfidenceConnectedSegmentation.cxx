/*
 * ConfidenceConnectedSegmentation.cxx
 *
 *  Created on: 12 Mar 2014
 *      Author: craig
 */
#include <stdio.h>
#include "../../headers/global.h"
#include <ITK-4.5/itkConfidenceConnectedImageFilter.h>

using namespace std;

/**
 * @title confidenceConnectedSegmenation
 * @description segment region based upon statistics
 */
void confidenceConnectedSegmentation()
{
	 typedef itk::ConfidenceConnectedImageFilter<volumeImageType, binaryVolumeType> ConnectedFilterType;
	 ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();
	 confidenceConnected->SetInput(volumeImage);

	 cout << "Please enter the number of iterations required followed by a <return> or <enter>: ";
	 float iterations = 0;

	 while ( !(cin >> iterations) || (iterations == 0))
	 {
		 // Enter this loop if input fails because of invalid data.

		 cout << "Incorrect value, please try again: ";
		 cin.clear ();   // reset the "failure" flag

		 // The input "cursor" is still positioned at the beginning of
		 // the invalid input, so we need to skip past it.
		 cin.ignore (1000, '\n');
	}

	cout << "Please enter the value to multiply the standard deviation by followed by a <return> or <enter>: ";
	float multiplier = 0;

	while ( !(cin >> multiplier) || (multiplier == 0))
	{
		// Enter this loop if input fails because of invalid data.

		cout << "Incorrect value, please try again: ";
		cin.clear ();   // reset the "failure" flag

		// The input "cursor" is still positioned at the beginning of
		// the invalid input, so we need to skip past it.

		cin.ignore (1000, '\n');
	}

	confidenceConnected->SetMultiplier(multiplier);
	confidenceConnected->SetNumberOfIterations(iterations);
	confidenceConnected->SetReplaceValue(175);

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

	confidenceConnected->SetSeed(index);
	confidenceConnected->SetInitialNeighborhoodRadius(2);
	confidenceConnected->Update();
	binaryVolume = confidenceConnected->GetOutput();
}
