#include "../../headers/global.h"
#include "../../headers/openGLStatus.h"
#include <mgl2/mgl.h>
/*
 * HistogramDrawFunc.cxx
 *
 *  Created on: 11 May 2014
 *      Author: craig
 */

/**
 * @Title drawGraph
 * @description draw the plots of the graph
 */
int drawGraph(mglGraph *gr)
{
	//create the graph, draw the title
	gr->Title("Histogram");
	gr->SetRanges(minimum,maximum,0,highFreq-1);
	gr->SetOrigin(minimum,0,0);  // first axis
	gr->SetTuneTicks(0);
	gr->Axis();
	//gr->Label('y',"Frequency",-1);
	gr->Label('x',"Value",0);

	//convert the histogram arrays to mglData arrays
	mglData x;
	x.Set(xArray,range);
	mglData y;
	y.Set(yArray,range);
	//plot the values
	gr->Plot(x,y,"b");
	return 0;
}

/**
 * @Title DrawHistogram
 * @description draw the histogram of the data
 */
void DrawHistogramFunc()
{
	glErrorCheck();
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,graphSize, 0,graphSize,0, 1);

	//generate mglGraph object
	mglGraph gr;
	gr.SetSize(graphSize,graphSize);
	gr.Alpha(true);
	gr.Light(true);
	drawGraph(&gr);

	long w=gr.GetWidth(), h=gr.GetHeight();

	//pointer to an array which stores the graph
	char *buf = new char[4*w*h];
	gr.GetRGB(buf,4*w*h);

	//flip y axis, for correct drawing
	glPixelZoom(1, -1);
	glRasterPos3f(0,graphSize-1,-0.3);
	glDrawPixels(w,h,GL_RGB,GL_UNSIGNED_BYTE,buf);
	glutSwapBuffers();
}

/**
 * @title ReshapeFunc
 * @description called when reshaped or resized
 */
void ReshapeHistogramFunc(int new_width, int new_height)
{
	int hWidth = new_width;
	int hHeight = new_height;

	//avoid divide by zero
	if(hWidth == 0)
	{
		hWidth = 1;
	}
	else if(hHeight == 0)
	{
		hHeight = 1;
	}

	//aspect ratios
	float heightWidth = hHeight/(float) width;
	float widthHeight = hWidth/(float) height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glViewport (0, 0, (GLsizei)(hWidth), (GLsizei)(hHeight));
}
