#include <stdio.h>
#include "../../headers/global.h"
#include <ITK-4.8/itkRescaleIntensityImageFilter.h>

/*
 * RescaleVolume.cxx
 *
 *  Created on: 11 May 2014
 *      Author: craig
 */
void rescaleVolumeForDisplay()
{
	typedef itk::RescaleIntensityImageFilter< volumeImageType, scaledVolumeType > RescaleFilterType;
	RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
	rescaleFilter->SetInput(volumeImage);
	rescaleFilter->SetOutputMinimum(0);
	rescaleFilter->SetOutputMaximum(255);
	rescaleFilter->Update();
	scaledVolume = rescaleFilter->GetOutput();
	scaledVolume->Update();
}



