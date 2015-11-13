/*
 * importDicomFile.cxx
 *
 *  Created on: 18 Mar 2014
 *      Author: craig
 */

#include	<stdio.h>
#include 	<ITK-4.8/itkImageSeriesReader.h>
#include 	<ITK-4.8/itkGDCMImageIO.h>
#include 	<ITK-4.8/itkGDCMSeriesFileNames.h>
#include	"../../headers/global.h"

using namespace std;

int importDicomFile(char *directory)
{
	//pointer to Image reader
	itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();

	//Get the DICOM filenames from the directory
	itk::GDCMSeriesFileNames::Pointer nameGenerator = itk::GDCMSeriesFileNames::New();
	nameGenerator->SetDirectory(directory);

	try
	{
		//get the Dicom series by the series ID
		typedef vector< string> seriesIdContainer;
		const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

		seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
		seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

		cout << endl << "The Directory: " << endl;
		cout << endl << directory << endl << endl;
		cout << "Contains the following DICOM Series: ";
		cout << endl << endl;

		while( seriesItr != seriesEnd)
		{
			cout << seriesItr->c_str() << endl;
			++seriesItr;
		}

		cout << endl << endl;
		cout << "Now reading series: " << endl << endl;

		typedef vector< string> fileNamesContainer;
		fileNamesContainer fileNames;

		cout << seriesUID.begin()->c_str() << endl;
		fileNames = nameGenerator->GetFileNames(seriesUID.begin()->c_str());

		cout << endl << endl;

		//create a series reader
		typedef itk::ImageSeriesReader< volumeImageType > ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileNames(fileNames);
		reader->SetImageIO(dicomIO);

		try
		{
			reader->Update();
		}
		catch(itk::ExceptionObject &ex)
		{
			cout << ex << endl;
			return EXIT_FAILURE;
		}

		//create a volume in memory from the series
		volumeImage = reader->GetOutput();
		volumeImage->Update();
	}
	catch (itk::ExceptionObject &ex)
	{
		cout << ex << endl;
		return EXIT_FAILURE;
	}
}
