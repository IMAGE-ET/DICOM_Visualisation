/**
 * @author Craig Parkinson
 * @title visualisation
 * @date 22/02/2013
 */

/**
* This application reads in a dicom series from the command line,
* the series is read in as a itk Volume, the values are rescaled
* allowing for the correct display of the data.
* This is then converted to a three dimensional OpenGL texture
* The window is generated with four different viewports, the application
* iterates across the texture to visualise the data.
* If the user right clicks on the window, they can change the view of the data.
*/

#include	<stdlib.h>
#include	<GL/glut.h>
#include 	<GL/glext.h>
#include	<math.h>
#include 	<ITK-4.5/itkImage.h>
#include 	<iostream>
#include	<stdio.h>
#include 	<ITK-4.5/itkExtractImageFilter.h>
#include	<ITK-4.5/itkRescaleIntensityImageFilter.h>
#include	<ITK-4.5/itkBinaryThresholdImageFilter.h>
#include	<ITK-4.5/itkConnectedThresholdImageFilter.h>
#include	<ITK-4.5/itkConfidenceConnectedImageFilter.h>
#include 	<ITK-4.5/itkOrientImageFilter.h>
#include	<mgl2/mgl.h>
#include	"../headers/openGLStatus.h"
#include	"../headers/histogramKeyboardFunc.h"
#include	"../headers/histogramMouseFunc.h"
#include	"../headers/menu.h"
#include	"../headers/global.h"
#include	"../headers/importDicomFile.h"
#include	"../headers/rescaleVolume.h"
#include	"../headers/histogramDrawFunc.h"

using namespace std;


void renderCrossSection();
void volumisedImage();
void originalCT();
void xPlane();
void yPlane();
void generateHistogram();
void rescaleVolumeForDisplay();
void mainMenu(int value);
void generateVolumeTexture();
void DisplayFunc();
void ReshapeFunc(int new_width, int new_height);
void KeyboardFunc(unsigned char key, int x, int y);
void MouseFunc(int button, int state, int x, int y);
void MotionFunc(int x, int y);
void IdleFunc();
void compileDisplayLists();


/**
 * @title main
 * @description Initialise scene
 */
int	main(int argc, char **argv)
{
	if(argc == 2)
	{
		if(importDicomFile(argv[1]) == EXIT_FAILURE)
		{
			cout << "There was a problem loading the data set" << endl;
		}
	}

	//create a volume in memory from the series
	itk::OrientImageFilter<volumeImageType,volumeImageType>::Pointer orienter = itk::OrientImageFilter<volumeImageType,volumeImageType>::New();
	orienter->UseImageDirectionOn();
	orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
	orienter->SetInput(volumeImage);
	orienter->Update();
	volumeImage = orienter->GetOutput();


	//Generate the histogram based on the Hounsfield units
	generateHistogram();

	//Rescaled the volume so that it can be viewed
	rescaleVolumeForDisplay();

	//start in the middle of the image
	InputImageType::RegionType inputRegion = volumeImage->GetLargestPossibleRegion();
	InputImageType::SizeType volumeSize = inputRegion.GetSize();

	//always start the center of the volume
	noImages = volumeSize[2];
	zSliceNumber = noImages/2;
	xImages = volumeSize[0];
	xSliceNumber = xImages/2;
	yImages = volumeSize[1];
	ySliceNumber = yImages/2;


	/**
	 * create window
	 */
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	width = glutGet(GLUT_SCREEN_WIDTH);
	height = glutGet(GLUT_SCREEN_HEIGHT);

	//create the window based on monitor size
	if(width >= height)
	{
		glutInitWindowSize(height,height);
		glutInitWindowPosition(width/2-(height/2),0);
		width = height;
	}
	else if(height > width)
	{
		glutInitWindowSize(width,width);
		glutInitWindowPosition(height/2-(width/2),0);
		height = width;
	}

	glErrorCheck();

	//call backs and window generation for the first window
	glutCreateWindow("Visualisation");
	glClearColor(0, 0, 0, 0);
	glutCreateMenu(mainMenu); // single menu, no need for id
	glutAddMenuEntry("Threshold Segmentation", 1);
	glutAddMenuEntry("Connected Threshold Region Growing Segmentation", 2);
	glutAddMenuEntry("Confidence Connected Segmentation", 3);
	glutAddMenuEntry("View Segmented Data", 4);
	glutAddMenuEntry("View Original Data", 5);
	glutAddMenuEntry("View Volume", 6);
	glutAddMenuEntry("View Cross Section", 7);
	glutAddMenuEntry("Exit", 8);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glErrorCheck();

	glEnable(GL_TEXTURE_3D);
	glGenTextures(3,&textureName);

	//generate a 3D texture from the volume in memory
	generateVolumeTexture();

	glDisable(GL_TEXTURE_3D);
    /**
	 * call backs
	 */
	glutDisplayFunc(&DisplayFunc);
	glutReshapeFunc(&ReshapeFunc);
	glutKeyboardFunc(&KeyboardFunc);
	glutMouseFunc(&MouseFunc);
	glutMotionFunc(&MotionFunc);
	glutIdleFunc(&IdleFunc);

	compileDisplayLists();
	glErrorCheck();

	//create a window for the separate display of the histogram
	glutInitWindowSize(graphSize,graphSize);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Histogram");
	glutDisplayFunc(&DrawHistogramFunc);
	glutReshapeFunc(&ReshapeHistogramFunc);
	glutKeyboardFunc(&histogramKeyboardFunc);
	glutMouseFunc(&HistogramMouseFunc);
	glutIdleFunc(&IdleFunc);
	glErrorCheck();

	glutMainLoop();

	return 0;
}

/**
 * @title renderCrossSection
 * @description render axial plane slice of the volume
 */
void renderCrossSection()
{	/**
	//Transverse Plane
	zSlice = (zSliceNumber+0.5)/noImages;

	double temp = (double)(xImages*xSpacing)/(double)(noImages*zSpacing);
	double scaledRange = (double)(1/temp);
	double newScaledMin = (double)(1-(scaledRange))/2;
	double newMax = (double)(1-newScaledMin);

	zSlice = (((zSlice-0)*(newMax-newScaledMin))/(1-0))+ newScaledMin;**/

	//calculate the correct depth of the image
	double oR = noImages * zSpacing;
	double temp = (double)(xImages*xSpacing)/(double)(noImages*zSpacing);
	double nR = 1.5/temp;
	double max = nR/2;
	textureDepth = ((((zSliceNumber*zSpacing) - 0) * nR) / oR) + -max;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_TEXTURE_3D);
	glBindTexture (GL_TEXTURE_3D, textureName+2);

	glBegin (GL_QUADS);
		glTexCoord3f(1.0, 1.0,zSlice);
	    glVertex3f(-0.75,textureDepth,-0.75);

	    glTexCoord3f(0.0, 1.0,zSlice);
	    glVertex3f(0.75,textureDepth,-0.75);

	    glTexCoord3f(0.0,0.0,zSlice);
	    glVertex3f(0.75, textureDepth,0.75);

	    glTexCoord3f(1.0,0.0,zSlice);
	    glVertex3f(-0.75, textureDepth,0.75);
	glEnd ();

	//Sagital Plane
	xSlice = (xSliceNumber+0.5)/xImages;
	oldRange = xImages;
	newRange = 1.5;
	textureDepth = (((xSliceNumber - 0) * newRange) / oldRange) + -0.75;

    glBindTexture (GL_TEXTURE_3D, textureName+2);

    glBegin (GL_QUADS);
    	glTexCoord3f(xSlice,1.0,0.0);
        glVertex3f(-textureDepth,-0.75,-0.75);

        glTexCoord3f(xSlice,0.0,0.0);
        glVertex3f(-textureDepth,-0.75,0.75);

        glTexCoord3f(xSlice, 0.0,1.0);
        glVertex3f(-textureDepth, 0.75,0.75);

        glTexCoord3f(xSlice, 1.0,1.0);
        glVertex3f(-textureDepth, 0.75,-0.75);
   glEnd ();
	
	oldRange = yImages;
	newRange = 1.5;
	textureDepth = (((ySliceNumber - 0) * newRange) / oldRange) + -0.75;

   glBindTexture (GL_TEXTURE_3D, textureName+2);

   glBegin (GL_QUADS);
   	   glTexCoord3f(0.0,ySlice,0.0);
       glVertex3f(0.75,-0.75,-textureDepth);

       glTexCoord3f(1.0,ySlice,0.0);
       glVertex3f(-0.75,-0.75,-textureDepth);

       glTexCoord3f(1.0,ySlice,1.0);
       glVertex3f(-0.75, 0.75,-textureDepth);

       glTexCoord3f(0.0,ySlice, 1.0);
       glVertex3f(0.75, 0.75,-textureDepth);
   glEnd ();
   glDisable(GL_BLEND);

   if(viewSegmented == 1)
   {

	   	//calculate the correct depth of the image
	   	double oR = noImages * zSpacing;
	   	double nR = 1.5/temp;
	   	double max = nR/2;
	   	textureDepth = ((((zSliceNumber*zSpacing) - 0) * nR) / oR) + -max;

	   	glColor3f(0,1,0);
	   	glEnable(GL_BLEND);
	   	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	   	glBindTexture (GL_TEXTURE_3D, textureName+1);

	   	glBegin (GL_QUADS);
	   		glTexCoord3f(1.0, 1.0,zSlice);
	   	    glVertex3f(-0.75,textureDepth,-0.75);

	   	    glTexCoord3f(0.0, 1.0,zSlice);
	   	    glVertex3f(0.75,textureDepth,-0.75);

	   	    glTexCoord3f(0.0,0.0,zSlice);
	   	    glVertex3f(0.75, textureDepth,0.75);

	   	    glTexCoord3f(1.0,0.0,zSlice);
	   	    glVertex3f(-0.75, textureDepth,0.75);
	   	glEnd ();

	   	//Sagital Plane
	   	xSlice = (xSliceNumber+0.5)/xImages;
	   	oldRange = xImages;
	   	newRange = 1.5;
	   	textureDepth = (((xSliceNumber - 0) * newRange) / oldRange) + -0.75;

	       glBindTexture (GL_TEXTURE_3D, textureName+1);

	       glBegin (GL_QUADS);
	       	glTexCoord3f(xSlice,1.0,0.0);
	           glVertex3f(-textureDepth,-0.75,-0.75);

	           glTexCoord3f(xSlice,0.0,0.0);
	           glVertex3f(-textureDepth,-0.75,0.75);

	           glTexCoord3f(xSlice, 0.0,1.0);
	           glVertex3f(-textureDepth, 0.75,0.75);

	           glTexCoord3f(xSlice, 1.0,1.0);
	           glVertex3f(-textureDepth, 0.75,-0.75);
	      glEnd ();

	   	//Coronal Plane
	      ySlice = (ySliceNumber+0.5)/yImages;

	   	oldRange = yImages;
	   	newRange = 1.5;
	   	textureDepth = (((ySliceNumber - 0) * newRange) / oldRange) + -0.75;

	      glBindTexture (GL_TEXTURE_3D, textureName+1);

	      glBegin (GL_QUADS);
	      	   glTexCoord3f(0.0,ySlice,0.0);
	          glVertex3f(0.75,-0.75,-textureDepth);

	          glTexCoord3f(1.0,ySlice,0.0);
	          glVertex3f(-0.75,-0.75,-textureDepth);

	          glTexCoord3f(1.0,ySlice,1.0);
	          glVertex3f(-0.75, 0.75,-textureDepth);

	          glTexCoord3f(0.0,ySlice, 1.0);
	          glVertex3f(0.75, 0.75,-textureDepth);
	      glEnd ();
	      glDisable(GL_BLEND);
       	 glColor3f(1,1,1);
       }
   glDisable(GL_TEXTURE_3D);
}

/**
 * @title cross Section
 * @description wire frame of a teapot
 */
void volumisedImage()
{
  if(viewVolume == 1)
  {
	  glMatrixMode(GL_TEXTURE);
	  glLoadIdentity();
	  glTranslatef(0.5,0.5,0.5);

	  glScaled((float)(xImages*xSpacing)/(float)(xImages*xSpacing), -1.0f*(float)(xImages*xSpacing)/(float)(yImages*ySpacing),(float)(xImages*xSpacing)/(float)(noImages*zSpacing));

	  glRotatef(-beta, 1, 0, 0);
	  glRotatef(alpha, 0, 1, 0);

	  glTranslatef(-0.5,-0.5,-0.5);
  }
  else
  {
	  glLoadIdentity();
	  glRotatef(beta, 1, 0, 0);
	  glRotatef(alpha, 0, 1, 0);
  }

  if(viewVolume == 1)
  {
	//enable the clipping plane and set the plane equation
	glClipPlane(GL_CLIP_PLANE0,planeEq0);
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,volumeAlpha);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_BLEND);
  	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  	if(viewSegmented == 1)
  	{
  		glBindTexture (GL_TEXTURE_3D, textureName+1);
  	}
  	else
  	{
  		glBindTexture(GL_TEXTURE_3D, textureName+2);
  	}
   	//calculate the correct depth of the image
   	oldRange = noImages * zSpacing;
   	newRange = 1.5;

	for(int i = 0; i < noImages; i++)
	{
		textureDepth = ((((i*zSpacing) - 0) * newRange) / oldRange) + -0.75;
		imageDepth = (i+0.5)/noImages;
		glBegin (GL_QUADS);
			glTexCoord3f(0,0.0,imageDepth);
			glVertex3f(-1.2,-1.2,textureDepth);

			glTexCoord3f(1.0,0.0,imageDepth);
			glVertex3f(1.2,-1.2,textureDepth);

			glTexCoord3f(1.0, 1.0,imageDepth);
			glVertex3f(1.2, 1.2,textureDepth);

			glTexCoord3f(0.0, 1.0,imageDepth);
			glVertex3f(-1.2, 1.2,textureDepth);
		glEnd();
	}
	glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
  	glDisable(GL_TEXTURE_3D);
  	glDisable(GL_CLIP_PLANE0);
  }
  else if (viewCrossSection == 1)
  {
	  renderCrossSection();
  }

  if(viewVolume == 0)
  {
	  //wire frame mode
	  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  
	  glCallList(displayListIndex+1);

	  //fill polygons
	  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }
}

/**
 * @title originalCT
 * @description display the original CT scan
 */
void originalCT()
{
	zSlice = (zSliceNumber+0.5)/noImages;

	double temp = (xImages*xSpacing)/(noImages*zSpacing);
	double scaledRange = 1/temp;
	double newScaledMin = (1-(scaledRange))/2;
	double newMax = 1-newScaledMin;

	zSlice = (((zSlice-0)*(newMax-newScaledMin))/(1-0))+ newScaledMin;


	glEnable(GL_TEXTURE_3D);
	glBindTexture (GL_TEXTURE_3D, textureName);

    glBegin (GL_QUADS);
    	  glTexCoord3f(0.0,0.0,zSlice);
    	  glVertex3f(-0.9,-0.9,0);

    	  glTexCoord3f(1.0,0.0,zSlice);
    	  glVertex3f(0.9,-0.9,0);

    	  glTexCoord3f(1.0, 1.0,zSlice);
    	  glVertex3f(0.9, 0.9,0);

    	  glTexCoord3f(0.0, 1.0,zSlice);
    	  glVertex3f(-0.9, 0.9,0);
    glEnd ();

    if(viewSegmented == 1)
    {
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    	glColor3f(0,1,0);
    	glBindTexture (GL_TEXTURE_3D, textureName+1);
    	    glBegin (GL_QUADS);
    	    	  glTexCoord3f(0.0,0.0,zSlice);
    	    	  glVertex3f(-0.9,-0.9,0);

    	    	  glTexCoord3f(1.0,0.0,zSlice);
    	    	  glVertex3f(0.9,-0.9,0);

    	    	  glTexCoord3f(1.0, 1.0,zSlice);
    	    	  glVertex3f(0.9, 0.9,0);

    	    	  glTexCoord3f(0.0, 1.0,zSlice);
    	    	  glVertex3f(-0.9, 0.9,0);
    	    glEnd ();
    	 glDisable(GL_BLEND);
    	 glColor3f(1,1,1);
    }
    glDisable(GL_TEXTURE_3D);
}

/**
 * @title xPlane
 * @description display an iterpolated image of the x-plane of the volumised ct series
 */
void xPlane()
{
	xSlice = (xSliceNumber+0.5)/xImages;
	glEnable(GL_TEXTURE_3D);
    glBindTexture (GL_TEXTURE_3D, textureName);

    glBegin (GL_QUADS);
    	  glTexCoord3f(xSlice,1.0,0.0);
    	  glVertex3f(-0.9,-0.9,0);

    	  glTexCoord3f(xSlice,0.0,0.0);
    	  glVertex3f(0.9,-0.9,0);

    	  glTexCoord3f(xSlice, 0.0,1.0);
    	  glVertex3f(0.9, 0.9,0);

    	  glTexCoord3f(xSlice, 1.0,1.0);
    	  glVertex3f(-0.9, 0.9,0);
    glEnd ();

    if(viewSegmented == 1)
    {
    	glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(0,1,0);
        glBindTexture (GL_TEXTURE_3D, textureName+1);

        glBegin (GL_QUADS);
        	glTexCoord3f(xSlice,1.0,0.0);
            glVertex3f(-0.9,-0.9,0);

            glTexCoord3f(xSlice,0.0,0.0);
            glVertex3f(0.9,-0.9,0);

            glTexCoord3f(xSlice, 0.0,1.0);
            glVertex3f(0.9, 0.9,0);

            glTexCoord3f(xSlice, 1.0,1.0);
            glVertex3f(-0.9, 0.9,0);
        glEnd ();
        glDisable(GL_BLEND);
        glColor3f(1,1,1);
      }
    glDisable(GL_TEXTURE_3D);
}

/**
 * @title xPlane
 * @description display an iterpolated image of the y-plane of the volumised ct series
 */
void yPlane()
{
	ySlice = (ySliceNumber+0.5)/yImages;

	//calculate the positioning
	double temp =  (double)(xImages*xSpacing)/(double)(yImages*ySpacing);
	double scaledRange = (double)(1/temp);
	double newScaledMin = (double)(1-(scaledRange))/2;
	double newMax = (double)(1-newScaledMin);

	ySlice = (((ySlice-0)*(newMax-newScaledMin))/(1-0))+ newScaledMin;

	glEnable(GL_TEXTURE_3D);
    glBindTexture (GL_TEXTURE_3D, textureName);

    glBegin (GL_QUADS);
    	  glTexCoord3f(0.0,ySlice,0.0);
    	  glVertex3f(-0.9,-0.9,0);

    	  glTexCoord3f(1.0,ySlice,0.0);
    	  glVertex3f(0.9,-0.9,0);

    	  glTexCoord3f(1.0,ySlice,1.0);
    	  glVertex3f(0.9, 0.9,0);

    	  glTexCoord3f(0.0,ySlice, 1.0);
    	  glVertex3f(-0.9, 0.9,0);
    glEnd();

    if(viewSegmented == 1)
    {
       	glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(0,1,0);
        glBindTexture (GL_TEXTURE_3D, textureName+1);

        glBegin (GL_QUADS);
       	  glTexCoord3f(0.0,ySlice,0.0);
       	  glVertex3f(-0.9,-0.9,0);

       	  glTexCoord3f(1.0,ySlice,0.0);
       	  glVertex3f(0.9,-0.9,0);

       	  glTexCoord3f(1.0,ySlice,1.0);
       	  glVertex3f(0.9, 0.9,0);

       	  glTexCoord3f(0.0,ySlice, 1.0);
       	  glVertex3f(-0.9, 0.9,0);
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(1,1,1);
    }
    glDisable(GL_TEXTURE_3D);
}

/**
 * @Title DisplayFunc
 * @description update rendering
 */
void DisplayFunc()
{
	glErrorCheck();

	glClear(GL_COLOR_BUFFER_BIT);

	/**
	 * bottom right
	 */
	glErrorCheck();

	if(viewVolume == 1)
	{
		glViewport(0,0,width,height);
		glErrorCheck();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glErrorCheck();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glErrorCheck();
		glOrtho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2);
		glErrorCheck();

		volumisedImage();

	}
	else
	{
		glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(0.5,0.5,0.5);
			glScaled((float)(xImages*xSpacing)/(float)(xImages*xSpacing), -1.0f*(float)(xImages*xSpacing)/(float)(yImages*ySpacing),(float)(xImages*xSpacing)/(float)(noImages*zSpacing));
			glTranslatef(-0.5,-0.5,-0.5);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			glErrorCheck();


			glViewport(0,0,width,height);
			glErrorCheck();

			glOrtho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2);

			glErrorCheck();
			glColor3f(1,0,0);
			glCallList(displayListIndex);
			glColor3f(1,1,1);
	glViewport((width/2)+0.5, 0, (width/2)-0.5, (height/2)-0.5);
	glErrorCheck();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glErrorCheck();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glErrorCheck();
	glOrtho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2);
	glErrorCheck();

	volumisedImage();

	//scale the texture
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(0.5,0.5,0.5);
	glScaled((float)(xImages*xSpacing)/(float)(xImages*xSpacing), -1.0f*(float)(xImages*xSpacing)/(float)(yImages*ySpacing),(float)(xImages*xSpacing)/(float)(noImages*zSpacing));
	glTranslatef(-0.5,-0.5,-0.5);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2);

	glMatrixMode(GL_MODELVIEW);
	glErrorCheck();

	/**
	 * top left
	 */
	glViewport(0, (height/2)+0.5, (width/2)-0.5, (height/2)-0.5);
	glPushMatrix();
	originalCT();
	glPopMatrix();
	glPushMatrix();
	//display the seed point
	if(seedPoint == 1)
	{
		glPointSize(5.0f);
		glBegin(GL_POINTS);
			glColor3f(1,0,0);
			glVertex2f(seedX,seedY);
			glColor3f(1,1,1);
		glEnd();
	}
	glPopMatrix();
	glErrorCheck();

	/**
	 * top right
	 */
	glViewport((width/2)+0.5, (height/2)+0.5, (width/2)-0.5, (height/2)-0.5);
	glPushMatrix();
	xPlane();
	glPopMatrix();
	glErrorCheck();

	/**
	 * bottom left
	 */
	glViewport(0, 0, (width/2)-0.5, (height/2)-0.5);
	glPushMatrix();
	yPlane();
	glPopMatrix();
	glErrorCheck();
	}
	glutSwapBuffers();
}

/**
 * @title ReshapeFunc
 * @description called when reshaped or resized
 */
void ReshapeFunc(int new_width, int new_height)
{
	width = new_width;
	height = new_height;

	//avoid divide by zero
	if(height == 0)
	{
		height = 1;
	}
	else if(width == 0)
	{
		width = 1;
	}

	//aspect ratios
	hw = height/(float) width;
	wh = width/(float) height;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(20 * sqrtf(1 + hw * hw), wh, 8, 12);

	glMatrixMode(GL_MODELVIEW);
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));
}

/**
 * @title IdleFunc
 * @description refresh the screen if the application is idle
 */
void IdleFunc()
{
	glutPostRedisplay();
}

/**
 * @title KeyboardFunc
 * @description called on keyboard use
 */
void KeyboardFunc(unsigned char key, int x, int y)
{
	//avoid warning, no effect
	xold = x;
	yold = y;

	if ('q' == key || 'Q' == key || 27 == key)
	{
		exit(0);
	}

	if('a' == key || 'A' == key)
	{
		if(volumeAlpha > 0.0)
		{
			volumeAlpha -= 0.01;

		}
		else
		{
			volumeAlpha = 0.0;
		}
	}

	if('s' == key || 'S' == key)
	{
		if(volumeAlpha < 1.0)
		{
			volumeAlpha += 0.01;
		}
		else
		{
			volumeAlpha = 1.0;
		}
	}

	//translate the cutting plane throught the volume by using aX + bY + cZ + d = 0;
	if('x' == key || 'X' == key)
	{
		if(planeEq0[3] >= 0.8)
		{
		}
		else
		{
			planeEq0[3] += 0.01;
		}
	}

	if('z' == key || 'Z' == key)
	{
		if(planeEq0[3] <= -0.8)
		{
		}
		else
		{
			planeEq0[3] -= 0.01;
		}
	}

	//set the depth of the top left image to be displayed
	if ('+' == key)
	{
		if(zSliceNumber != noImages)
		{
			zSliceNumber += 1;
		}
		else
		{
			zSliceNumber = noImages;
		}
	}

	if ('-' == key)
	{
		if(zSliceNumber != 0)
		{
			zSliceNumber -= 1;
		}
		else
		{
			zSliceNumber = 0;
		}
	}

	//set the depth of the top right image to be displayed
	if ('6' == key)
	{
		if(xSliceNumber != xImages)
		{
			xSliceNumber += 1;
		}
		else
		{
			xSliceNumber = xImages;
		}
	}

	if ('4' == key)
	{
		if(xSliceNumber != 0)
		{
			xSliceNumber -= 1;
		}
		else
		{
			xSliceNumber = 0;
		}
	}

	//set the depth of the bottom left image to be displayed
	if ('8' == key)
	{
		if(ySliceNumber != yImages )
		{
			ySliceNumber += 1;
		}
		else
		{
			ySliceNumber = yImages;
		}
	}

	if ('2' == key)
	{
		if(ySliceNumber != 0)
		{
			ySliceNumber -= 1;
		}
		else
		{
			ySliceNumber = 0;
		}
	}
}

/**
 * @title MouseFunc
 * @description called on mouse button
 */
void MouseFunc(int button, int state, int x, int y)
{
	if(GLUT_LEFT_BUTTON == button)
	{
		left_click = state;
		if(left_click == GLUT_DOWN)
		{
			//disable seed point
			seedPoint = 0;

			//double check the user has clicked in the top left view port
			if((x <= ((width/2)+0.5)) && (y <= ((height/2)-0.5)))
			{
				//convert window coordinates to orthogonal coordinates
				float oR = ((width/2)+0.5)-0;
				float nR = 1.2-(-1.2);
				float nX = (((x-0)*nR)/oR)+(-1.2);

				oR = ((height/2)-0.5)-0;
				float nY = (((y-0)*nR)/oR)+(-1.2);

				//check the user has clicked on the texture
				if((nX >= -0.9) && (nX <= 0.9) && (nY >= -0.9) && (nY <= 0.9))
				{
					//set the orthogonal coordinates to seedX and seedY
					//they are used in the display function to show the seedpoint
					seedX = nX;
					seedY = -nY;
					seedPoint = 1;
				}
			}
			else
			{
				seedPoint = 0;
			}
		}
	}

	if(GLUT_RIGHT_BUTTON == button)
	{
		right_click = state;
	}

	xold = x;
	yold = y;
}

/**
 * @title MotionFunc
 * @description called when mouse moved
 */
void MotionFunc(int x, int y)
{
	//calculate the rotation angle
	if((GLUT_DOWN == left_click) && (x > ((width/2)+0.5)) && (y > ((height/2)-0.5)))
    {
		beta = beta - (y - yold) / 2.f;
		alpha = alpha - (x - xold) / 2.f;

		glutPostRedisplay();
    }
	xold = x;
	yold = y;
}

/**
 * @title compileDisplayLists
 * @description compile the window separation and wire frame cube
 */
void compileDisplayLists()
{
	displayListIndex = glGenLists(2);

	glNewList(displayListIndex, GL_COMPILE);
		glBegin(GL_LINE_LOOP);//start drawing a line loop
			glVertex3f(-1.2f,0.0f,0.0f);//left of window
			glVertex3f(1.2f,0.0f,0.0f);//right of window
			glVertex3f(0.0f,0.0f,0.0f);//middle of window
			glVertex3f(0.0f,1.2f,0.0f);//top of window
			glVertex3f(0.0f,-1.2f,0.0f);//bottom of window
			glVertex3f(0.0f,0.0f,0.0f);//middle of window
		glEnd();//end drawing of line
	glEndList();

	glNewList(displayListIndex+1, GL_COMPILE);
		glBegin(GL_QUADS);
	  	  //front face of bounding box
	  	  glVertex3f(-0.75,-0.75,-0.75);
		  glVertex3f(0.75,-0.75,-0.75);
		  glVertex3f(0.75,0.75,-0.75);
		  glVertex3f(-0.75,0.75,-0.75);

		  //left side of bounding box
		  glVertex3f(-0.75,-0.75,-0.75);
		  glVertex3f(-0.75,-0.75,0.75);
		  glVertex3f(-0.75,0.75,0.75);
		  glVertex3f(-0.75,0.75,-0.75);

		  //Back side of bounding box
		  glVertex3f(-0.75,-0.75,0.75);
		  glVertex3f(0.75,-0.75,0.75);
		  glVertex3f(0.75,0.75,0.75);
		  glVertex3f(-0.75,0.75,0.75);

		  //right side of bounding box
		  glVertex3f(0.75,-0.75,-0.75);
		  glVertex3f(0.75,-0.75,0.75);
		  glVertex3f(0.75,0.75,0.75);
		  glVertex3f(0.75,0.75,-0.75);

		glEnd();
	glEndList();
}

/**
 * @title generateVolumeTexture
 * @description create a 3D texture of the DICOM volume
 */
void generateVolumeTexture()
{
	//create a 3D texture from the display volume
	scaledPixelType *volumePointer = scaledVolume->GetBufferPointer();
	signed short lWidth = scaledVolume->GetBufferedRegion().GetSize()[0];
	signed short lHeight = scaledVolume->GetBufferedRegion().GetSize()[1];
	signed short lDepth = scaledVolume->GetBufferedRegion().GetSize()[2];

	/**
	 * the spacing between each image slice
	 */
	const scaledVolumeType::SpacingType spacing = scaledVolume->GetSpacing();
	xSpacing = spacing[0];
	ySpacing = spacing[1];
	zSpacing = spacing[2];

	//use a proxy texture to check the texture can be generated
	glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_LUMINANCE, lWidth, lHeight, lDepth, 0, GL_LUMINANCE,GL_UNSIGNED_BYTE, NULL);

	GLint textureWidth;
	GLint textureDepth;
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &textureWidth);
	glGetTexLevelParameteriv(GL_PROXY_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &textureDepth);


	if(textureWidth == 0 || textureDepth == 0)
	{
		cout << "Volume can not be created from this image series." << endl;
		exit(0);
	}
	else
	{
		glErrorCheck();
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glBindTexture(GL_TEXTURE_3D,textureName);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glErrorCheck();
		glTexImage3D(GL_TEXTURE_3D,0, GL_LUMINANCE, lWidth, lHeight, lDepth,0,GL_LUMINANCE,GL_UNSIGNED_BYTE, volumePointer);

		glBindTexture(GL_TEXTURE_3D,textureName+2);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_BORDER);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glErrorCheck();
		glTexImage3D(GL_TEXTURE_3D,0, GL_INTENSITY, lWidth, lHeight, lDepth,0,GL_LUMINANCE,GL_UNSIGNED_BYTE, volumePointer);
	}
}
