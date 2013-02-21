/**
 * @author Craig Parkinson
 * @title visualisation
 * @description create four different view ports displaying CT Data
 */

#include	<stdlib.h>
#include	<GL/glut.h>
#include 	<GL/glext.h>
#include	<math.h>
#include 	<itkImage.h>
#include 	<iostream>
#include	<stdio.h>
#include 	<itkImageSeriesReader.h>
#include 	<itkDICOMImageIO2.h>
#include 	<itkDICOMSeriesFileNames.h>
#include 	<itkExtractImageFilter.h>
#include	<itkRescaleIntensityImageFilter.h>
#include	"openGLStatus.h"

GLuint displayListIndex;
GLuint textureName;
static int	left_click = GLUT_UP;
static int	right_click = GLUT_UP;
static int	xold;
static int	yold;
double xSpacing;
double ySpacing;
double zSpacing;
int view = 2;
int sizeX;
int sizeY;
int sizeZ;
int width;
int height;
int wh;
int hw;
int z = 0;
int zSliceNumber;
int noImages;
int xSliceNumber;
int xImages;
int ySliceNumber;
int yImages;
static float alpha = 0;
static float beta = 0;
float imageDepth;
float textureDepth;
float newRange;
float oldRange;
float zSlice;
float ySlice;
float xSlice;
typedef float InputPixelType;
typedef float OutputPixelType;
typedef short volumePixelType;
typedef float scaledPixelType;
typedef itk::Image< InputPixelType, 3 > InputImageType;
typedef itk::Image< volumePixelType, 3 > volumeImageType;
typedef itk::Image< scaledPixelType, 3 > scaledVolumeType;
scaledVolumeType::Pointer scaledVolume;

using namespace std;

/**
 * @title renderCrossSection
 * @description render axial plane slice of the volume
 */
void renderCrossSection()
{
	zSlice = (zSliceNumber+0.5)/noImages;

	glColor4f(1,1,1,0.75);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture (GL_TEXTURE_3D, textureName);

	glBegin (GL_QUADS);
	    glTexCoord3f(0.0,0.0,zSlice);
	    glVertex3f(-0.75,zSlice,-0.75);

	    glTexCoord3f(1.0,0.0,zSlice);
	    glVertex3f(0.75,zSlice,-0.75);

	    glTexCoord3f(1.0, 1.0,zSlice);
	    glVertex3f(0.75, zSlice,0.75);

	    glTexCoord3f(0.0, 1.0,zSlice);
	    glVertex3f(-0.75, zSlice,0.75);
	glEnd ();

    xSlice = (xSliceNumber+0.5)/xImages;

    glBindTexture (GL_TEXTURE_3D, textureName);

    glBegin (GL_QUADS);
    	glTexCoord3f(xSlice,1.0,0.0);
        glVertex3f(xSlice,-0.75,-0.75);

        glTexCoord3f(xSlice,0.0,0.0);
        glVertex3f(xSlice,-0.75,0.75);

        glTexCoord3f(xSlice, 0.0,1.0);
        glVertex3f(xSlice, 0.75,0.75);

        glTexCoord3f(xSlice, 1.0,1.0);
        glVertex3f(xSlice, 0.75,-0.75);
   glEnd ();

   ySlice = (ySliceNumber+0.5)/yImages;

   glBindTexture (GL_TEXTURE_3D, textureName);

   glBegin (GL_QUADS);
   	   glTexCoord3f(0.0,ySlice,0.0);
       glVertex3f(-0.75,-0.75,0);

       glTexCoord3f(1.0,ySlice,0.0);
       glVertex3f(0.75,-0.75,0);

       glTexCoord3f(1.0,ySlice,1.0);
       glVertex3f(0.75, 0.75,0);

       glTexCoord3f(0.0,ySlice, 1.0);
       glVertex3f(-0.75, 0.75,0);
   glEnd ();
   glDisable(GL_BLEND);
}

/**
 * @title renderVolume
 * @description  draw the segments of the 3D texture according to its depth
 */
void renderVolume()
{
  glColor4f(1,1,1,0.1);
  glBindTexture (GL_TEXTURE_3D, textureName);

  //blend the textures together
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	 glBegin(GL_QUADS);
	  for(int i = (noImages-1); i >= 0; i--)
	  {
		  imageDepth = (i+0.5)/noImages;
		  oldRange = noImages + (noImages * zSpacing);
		  newRange = 1.5;
		  textureDepth = (((i - 0) * newRange) / oldRange) + -0.75;
		  glTexCoord3f(0.0,0.0,imageDepth);
		  glVertex3f(-0.5,-0.5,textureDepth);

		  glTexCoord3f(1.0,0.0,imageDepth);
		  glVertex3f(0.5,-0.5,textureDepth);

		  glTexCoord3f(1.0, 1.0,imageDepth);
		  glVertex3f(0.5, 0.5,textureDepth);

		  glTexCoord3f(0.0, 1.0,imageDepth);
		  glVertex3f(-0.5, 0.5,textureDepth);
	  }
	 glEnd();
	glDisable(GL_BLEND);
}

/**
 * @title cross Section
 * @description wire frame of a teapot
 */
void VolumisedImage(void)
{
  glRotatef(beta, 1, 0, 0);
  glRotatef(alpha, 0, 1, 0);

  //switch between the views according to the view value
  switch (view)
  {
  	  case 1:
  		  glErrorCheck();
  		  renderCrossSection();
  		  break;
  	  case 2:
  		  glErrorCheck();
  		  renderVolume();
  		  break;
  	  default:
  		  glErrorCheck();
  		  renderVolume();
  		  break;
  }

  //wireframe mode
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  
  glCallList(displayListIndex+1);

  //fill polygons
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

/**
 * @title originalCT
 * @description display the original CT scan
 */
void originalCT(void)
{
	zSlice = (zSliceNumber+0.5)/noImages;
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
}

/**
 * @title xPlane
 * @description display an iterpolated image of the x-plane of the volumised ct series
 */
void xPlane(void)
{
	xSlice = (xSliceNumber+0.5)/xImages;
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
}

/**
 * @title xPlane
 * @description display an iterpolated image of the y-plane of the volumised ct series
 */
void yPlane(void)
{
	ySlice = (ySliceNumber+0.5)/yImages;
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
    glEnd ();
}

/**
 * @Title DisplayFunc
 * @description update rendering
 */
void DisplayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,width,height);
	glOrtho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2);

	glCallList(displayListIndex);

	/**
	 * bottom right
	 */
	glErrorCheck();

	glViewport((width/2)+0.5, 0, (width/2)-0.5, (height/2)-0.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2);

	VolumisedImage();

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

	//set the depth of the top left image to be displayed
	if ('+' == key)
	{
		if(ySliceNumber != noImages )
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
	if(GLUT_DOWN == left_click)
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
 * @description compile the window seperation and wireframe cube
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

void mainMenu(int value)
{
	if(value == 1)
	{
		view = 1;
	}
	else if(value == 2)
	{
		view = 2;
	}
	else if(value == 3)
	{
		exit(0);
	}
}

/**
 * @title main
 * @description Initialise scene
 */
int	main(int argc, char **argv)
{
	if(argc < 2)
	{
		cerr << "Usage: " << argv[0] << " DicomDirectory [seriesName] " << endl;
		return EXIT_FAILURE;
	}

	itk::DICOMImageIO2::Pointer dicomIO = itk::DICOMImageIO2::New();

	//Get the DICOM filenames from the directory
	itk::DICOMSeriesFileNames::Pointer nameGenerator = itk::DICOMSeriesFileNames::New();
	nameGenerator->SetDirectory(argv[1]);

	try
	{
		typedef vector< string> seriesIdContainer;
		const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

		seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
		seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

		cout << endl << "The Directory: " << endl;
		cout << endl << argv[1] << endl << endl;
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

		if(argc < 4)
		{
			cout << seriesUID.begin()->c_str() << endl;
			fileNames = nameGenerator->GetFileNames();
		}
		else
		{
			cout << argv[2] << endl;
			fileNames = nameGenerator->GetFileNames(argv[2]);
		}

		cout << endl << endl;

		typedef itk::ImageSeriesReader< volumeImageType > ReaderType;
		volumeImageType::Pointer volumeImage = volumeImageType::New();
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

		//store the volumised image
		volumeImage = reader->GetOutput();
		volumeImage->Update();

		typedef itk::RescaleIntensityImageFilter< volumeImageType, scaledVolumeType > RescaleFilterType;
		RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
		rescaleFilter->SetInput(volumeImage);
		rescaleFilter->SetOutputMinimum(0.0);
		rescaleFilter->SetOutputMaximum(1.0);
		rescaleFilter->Update();
		scaledVolume = rescaleFilter->GetOutput();
		scaledVolume->Update();

		InputImageType::RegionType inputRegion = volumeImage->GetLargestPossibleRegion();
		InputImageType::SizeType volumeSize = inputRegion.GetSize();
	    noImages = volumeSize[2];
	    noImages--;
	    zSliceNumber = noImages/2;
	    xImages = volumeSize[0];
	    xImages--;
	    xSliceNumber = xImages/2;
	    yImages = volumeSize[1];
	    yImages--;
	    ySliceNumber = yImages/2;
	}
	catch(itk::ExceptionObject &ex)
	{
		cout << ex;
		return EXIT_FAILURE;
	}

	/**
	 * create window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	width = glutGet(GLUT_SCREEN_WIDTH);
	height = glutGet(GLUT_SCREEN_HEIGHT);

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

	glutCreateWindow("Visualisation");
	glClearColor(0.3, 0.3, 0.3, 0.3);
	glutCreateMenu(mainMenu); // single menu, no need for id
	glutAddMenuEntry("Cross Sectional View", 1);
	glutAddMenuEntry("Volume View", 2);
	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	compileDisplayLists();

	glEnable(GL_TEXTURE_3D);
	glGenTextures(1,&textureName);

	OutputPixelType *volumePointer = scaledVolume->GetBufferPointer();
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

	glBindTexture(GL_PROXY_TEXTURE_3D,textureName);
	glTexImage3D(GL_PROXY_TEXTURE_3D, 0, GL_LUMINANCE, lWidth, lHeight, lDepth, 0, GL_LUMINANCE,GL_FLOAT, NULL);

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
		glBindTexture(GL_TEXTURE_3D,textureName);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, lWidth, lHeight, lDepth, 0, GL_LUMINANCE,GL_FLOAT, volumePointer);
	}

    /**
	 * call backs
	 */
	glutDisplayFunc(&DisplayFunc);
	glutReshapeFunc(&ReshapeFunc);
	glutKeyboardFunc(&KeyboardFunc);
	glutMouseFunc(&MouseFunc);
	glutMotionFunc(&MotionFunc);
	glutIdleFunc(&IdleFunc);
	glutMainLoop();

	return 0;
}
