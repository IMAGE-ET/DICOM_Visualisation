/*
 * GenerateHistogramFunc.cxx
 *
 *  Created on: 12 Mar 2014
 *  Author: craig parkinson
 *  Organisation: Bangor University
 *  Supervisor: Franck Vidal
 */
#include	<stdio.h>
#include	<ITK-4.8/itkImageToHistogramFilter.h>
#include	<ITK-4.8/itkStatisticsImageFilter.h>
#include	"../../headers/global.h"

using namespace std;

/**
*@title generateHistogram
*@description create the histogram of the dicom series
**/
void generateHistogram()
{
	//use the statistics filter to get the minimum and maximum values in the DICOM series

	typedef itk::Statistics::ImageToHistogramFilter<volumeImageType > HistogramFilterType;
	HistogramFilterType::Pointer histogramFilter = HistogramFilterType::New();

	typedef itk::StatisticsImageFilter<volumeImageType> statFilterType;
	statFilterType::Pointer statFilter = statFilterType::New();

	statFilter->SetInput(volumeImage);
	statFilter->Update();

	maximum = statFilter->GetMaximum();
	minimum = statFilter->GetMinimum();
	tempMax = maximum;
	tempMin = minimum;
	range = maximum-minimum;

	//calculate the size of the histogram
	typedef HistogramFilterType::HistogramSizeType SizeType;
	SizeType size(1);
	size[0] = range;
	histogramFilter->SetHistogramSize(size);
	histogramFilter->SetInput(volumeImage);
	histogramFilter->SetAutoMinimumMaximum(true);
	histogramFilter->SetMarginalScale(1.0);
	histogramFilter->Update();

	//use and iterator to store the histogram in an array

	typedef HistogramFilterType::HistogramType HistogramType;
	const HistogramType * histogram = histogramFilter->GetOutput();

	HistogramType::ConstIterator itr = histogram->Begin();
	HistogramType::ConstIterator end = histogram->End();

	if (xArray) delete [] xArray;
	if (yArray) delete [] yArray;
	xArray = new(nothrow) double[range];
	yArray = new(nothrow) double[range];
	double binNumber = minimum;
	highFreq = 0;
	int index = 0;

	while(itr != end)
	{
		xArray[index] = binNumber;
		yArray[index] = itr.GetFrequency();

		if(itr.GetFrequency() > highFreq)
		{
			highFreq = itr.GetFrequency();
		}
		++index;
		++itr;
		++binNumber;
	}

	tempHighFreq = highFreq;
	arraySize = index--;
}
