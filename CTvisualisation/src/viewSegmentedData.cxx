/*
 * ViewSegmentedData.cxx
 *
 *  Created on: 11 Mar 2014
 *      Author: craig
 */

#include <stdio.h>
#include <GL/gl.h>
#include "../headers/viewSegmentedData.h"
#include "../headers/global.h"

using namespace std;

/**
 * @title viewSegmentedData
 * @description set the 3D texture to binary data
 */

void viewSegmentedData()
{
	//generate a 3d texture from the binary volume
	binaryPixelType *binaryVolumePointer = binaryVolume->GetBufferPointer();
	signed short lWidth = binaryVolume->GetBufferedRegion().GetSize()[0];
	signed short lHeight = binaryVolume->GetBufferedRegion().GetSize()[1];
	signed short lDepth = binaryVolume->GetBufferedRegion().GetSize()[2];

	glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_LUMINANCE, lWidth, lHeight, lDepth, 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, NULL);

	GLint textureWidth;
	GLint textureDepth;
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &textureWidth);
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &textureDepth);


	if(textureWidth == 0 || textureDepth == 0)
	{
		cout << "Segmented Data can't be generated" << endl;
	}
	else
	{
		glBindTexture(GL_TEXTURE_3D,textureName+1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		//use alpha blending to generate the texture
		glTexImage3D(GL_TEXTURE_3D, 0, GL_ALPHA, lWidth, lHeight, lDepth, 0, GL_ALPHA,GL_UNSIGNED_BYTE, binaryVolumePointer);
		viewSegmented = 1;
	}
}
