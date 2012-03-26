/*
 * OGLRendering.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: iraklis
 */

#include <mastdlib.h>
#include "OGLScreen.h"

#include <madmath.h>

// Namespaces we want to access.
using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.


OGLScreen::OGLScreen(MAHandle particleImage): Screen()
{
	mParticleImageHandle = particleImage;
	mGLViewInitialized = false;
	mEnvironmentInitialized = false;
	mVariablesInitialized = false;
	mShouldRender = false;

	mTotalTime = 0;
	mFrameCounter = 0;

	createUI();

	mPrevTime = maGetMilliSecondCount();

	//Setthe timer that controls the rendering
	Environment::getEnvironment().addTimer(this,10,0);
}

OGLScreen::~OGLScreen()
{
	delete mParticles;
}

void OGLScreen::createUI()
{
	//Increase flow button
	mAddButton = new Button();
	mAddButton->fillSpaceHorizontally();
	mAddButton->wrapContentVertically();
	mAddButton->setText("Increase");
	mAddButton->addButtonListener(this);

	//Decrease flow button
	mRemoveButton = new Button();
	mRemoveButton->fillSpaceHorizontally();
	mRemoveButton->wrapContentVertically();
	mRemoveButton->setText("Decrease");
	mRemoveButton->addButtonListener(this);

	HorizontalLayout* hLayout = new HorizontalLayout();
	hLayout->wrapContentVertically();
	hLayout->addChild(mAddButton);
	hLayout->addChild(mRemoveButton);

	mFPSLabel = new Label();
	mFPSLabel->setText("FPS:");
	mFPSLabel->wrapContentVertically();
	mFPSLabel->fillSpaceHorizontally();

	mFlowLabel = new Label();
	mFlowLabel->setText("Particles:");
	mFlowLabel->wrapContentVertically();
	mFlowLabel->fillSpaceHorizontally();

	HorizontalLayout* hLayout2 = new HorizontalLayout();
	hLayout2->wrapContentVertically();
	hLayout2->addChild(mFPSLabel);
	hLayout2->addChild(mFlowLabel);

	//The widget that renders the animation
	mGLView = new GLView(MAW_GL_VIEW);
	mGLView->addGLViewListener(this);

	VerticalLayout* vLayout = new VerticalLayout();
	vLayout->addChild(hLayout);
	vLayout->addChild(hLayout2);
	vLayout->addChild(mGLView);

	setMainWidget(vLayout);
	setTitle("Native Rendering");
}

void OGLScreen::buttonClicked(Widget* button)
{
	//Notify the JavaScript code that a button was clicked
	if(button == mAddButton){
		mHTMLScreen->getWebView()->callJS("increaseParticles()");
	}
	else if(button == mRemoveButton){
		mHTMLScreen->getWebView()->callJS("decreaseParticles()");
	}
}

void OGLScreen::enableAddButton(bool state)
{
	if(state)
	{
		mAddButton->setEnabled(true);
		mAddButton->setFontColor(0x000000);
	}
	else
	{
		mAddButton->setEnabled(false);
		mAddButton->setFontColor(0x969696);
	}
}

void OGLScreen::enableRemoveButton(bool state)
{
	if(state)
	{
		mRemoveButton->setEnabled(true);
		mRemoveButton->setFontColor(0x000000);
	}
	else
	{
		mRemoveButton->setEnabled(false);
		mRemoveButton->setFontColor(0x969696);
	}
}

void OGLScreen::glViewReady(GLView* glView)
{
	//Set this GLView to receive OpenGL commands
	mGLView->bind();

	// Create the texture we will use for rendering.
	createTexture();

	// Flag that the GLView has been initialized.
	mGLViewInitialized = true;

	// Initialize OpenGL.
	initGL();
}

void OGLScreen::initVariables(HTMLScreen *htmlScreen,int maxParticles, int numParticles, int numSpikes,
		int radiusScale, int screenWidth, int screenHeight)
{
	MAX_PARTICLES = maxParticles;
	mNumSpikes = numSpikes;
	mRadiusScale = radiusScale;

	SCREN_WIDTH = screenWidth;

	SCREEN_HEIGHT = screenHeight;

	//Initialize the list of particles
	mParticles = new particle[MAX_PARTICLES];
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		//All particles in the list start out as inactive
		mParticles[i].alive = false;
	}

	mHTMLScreen = htmlScreen;
	mVariablesInitialized = true;

	addNewParticles(numParticles);
	// Initialize OpenGL.
	initGL();
}

/**
 * Create the texture used for rendering.
 */
void OGLScreen::createTexture()
{
	// Create an OpenGL 2D texture from the image resource.
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &mParticleTexture);
	glBindTexture(GL_TEXTURE_2D, mParticleTexture);
	maOpenGLTexImage2D(mParticleImageHandle);

	// Set texture parameters.
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/**
 * Setup the projection matrix.
 */
void OGLScreen::setViewport(int width, int height)
{
	// Set the viewport to fill the GLView
	glViewport(0, 0, (GLint)mGLView->getWidth(), (GLint)mGLView->getHeight());

	// Select the projection matrix.
	glMatrixMode(GL_PROJECTION);

	// Reset the projection matrix.
	glLoadIdentity();

	//Set an orthographic projection
	//The Y axis is set upside-down to
	//match the coordinate system of the
	//HTML5 canvas
	glOrthof((GLfloat)(-width / 2),
			(GLfloat)(+width / 2),
			(GLfloat)(-height / 2),
			(GLfloat)(+height / 2),
			0,1
			);
}

/**
 * Standard OpenGL initialization.
 */
void OGLScreen::initGL()
{
	//This function is called twice, but executes it's code
	//Only when both the other two initialization methods
	//have been executed
	if(!(mGLViewInitialized && mVariablesInitialized))
	{
		return;
	}

	//Configure the viewport
	setViewport(SCREN_WIDTH, SCREEN_HEIGHT);
    // Enable texture mapping.
    glEnable(GL_TEXTURE_2D);

    // Enable smooth shading.
	glShadeModel(GL_SMOOTH);

	// Set the depth value used when clearing the depth buffer.
	glClearDepthf(1.0f);

	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ONE);

	mEnvironmentInitialized = true;
}

void OGLScreen::renderParticleObject()
{
	// Array used for object coordinates.
	GLfloat vcoords[4][3];
	// Array used for texture coordinates.
	GLfloat tcoords[4][2];
	// Array used to convert from QUAD to TRIANGLE_STRIP.
	// QUAD is not available on the OpenGL implementation
	// we are using.
	GLubyte indices[4] = {0, 1, 3, 2};

	// Select the texture to use when rendering the box.
	glBindTexture(GL_TEXTURE_2D, mParticleTexture);

	// Define the particle.
	tcoords[0][0] = 1.0f;  tcoords[0][1] = 0.0f;
	vcoords[0][0] = 0.0f; vcoords[0][1] = 1.0f; vcoords[0][2] = 0.0f;
	tcoords[1][0] = 0.0f;  tcoords[1][1] = 0.0f;
	vcoords[1][0] = 0.0f;  vcoords[1][1] = 0.0f; vcoords[1][2] = 0.0f;
	tcoords[2][0] = 0.0f;  tcoords[2][1] = 1.0f;
	vcoords[2][0] = 1.0f;  vcoords[2][1] = 0.0f; vcoords[2][2] = 0.0f;
	tcoords[3][0] = 1.0f;  tcoords[3][1] = 1.0f;
	vcoords[3][0] = 1.0f; vcoords[3][1] = 1.0f; vcoords[3][2] = 0.0f;

	// Set pointers to vertex coordinates and texture coordinates.
	glVertexPointer(3, GL_FLOAT, 0, vcoords);
	glTexCoordPointer(2, GL_FLOAT, 0, tcoords);

	// Enable texture and vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Render each active particle
	for(int i = 0; i < mNumParticles; i++) {

			glPushMatrix();

			//Position the particle in the X and Y axis
			glTranslatef(mParticles[i].x, mParticles[i].y, 0.0f);

			//Scale to simulate the Z axis, since this is a 2D
			//image and we are using an Orthographic projection
			glScalef(mParticles[i].z , mParticles[i].z , 0.0f);

			// This draws the particle.
			glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

			glPopMatrix();

	}

	// Disable texture and vertex arrays
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


}

void OGLScreen::draw(int currentTime)
{
	// The GL_View must be initialized before we can do any drawing.
	if (!mEnvironmentInitialized)
	{
		return;
	}
	// Set the background color to be used when clearing the screen.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Clear the screen and the depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the model matrix.
	glMatrixMode(GL_MODELVIEW);

	// Reset the model matrix.
	glLoadIdentity();

	float twoPI = 2*M_PI;

	for(int i = 0; i < mNumParticles; i++) {
		float maxRamp = SCREN_WIDTH/6 * sin(currentTime*0.00075);

		float ramp = maxRamp*cos(-currentTime*0.001);
		float r = (float)SCREN_WIDTH/mRadiusScale + ramp*sin(((float)mNumSpikes*i/mNumParticles)*twoPI);
		// var particleSize = 80 + 70*Math.sin(time*0.004 + 4*i/NUM_PARTICLES*twoPI);

		float particleSize = 80 + 70*sin(currentTime*0.004 + 4.0f*i/mNumParticles*twoPI);

		//mParticles[i].x = -particleSize/2 + SCREN_WIDTH/2 +r * cos(currentTime * 0.0005 + ((float)i / mNumParticles) * twoPI);
		//mParticles[i].y = -particleSize/2 + SCREN_WIDTH/2 +r * sin(currentTime * 0.0005 + ((float)i / mNumParticles) * twoPI);
		mParticles[i].x = -particleSize/2 +r * cos(currentTime * 0.0005 + ((float)i / mNumParticles) * twoPI);
		mParticles[i].y = -particleSize/2 +r * sin(currentTime * 0.0005 + ((float)i / mNumParticles) * twoPI);
		mParticles[i].z = particleSize;
	}

	//Render the particles at their new positions
	renderParticleObject();

	// Wait (blocks) until all GL drawing commands to finish.
	glFinish();

	mGLView->redraw();
}



void OGLScreen::shouldRender(bool render)
{
	mShouldRender = render;
}

void OGLScreen::runTimerEvent()
{
	//Execute only if the screen is active
	if(mShouldRender)
	{
		//Get the current system time
		int currentTime = maGetMilliSecondCount();

		//Calculate and draw the positions for the new frame
		draw(currentTime);

		mFrameCounter++;
		mTotalTime += currentTime - mPrevTime;
		if(mFrameCounter == 100)
		{
			char buffer[32];
			sprintf(buffer,"FPS:%4.1f", 100000.0f / mTotalTime);
			mFPSLabel->setText(buffer);
			mTotalTime = 0;
			mFrameCounter = 0;
		}
		mPrevTime = currentTime;
	}
}

void OGLScreen::addNewParticles(int amount)
{
	mNumParticles = amount;
	char buffer[32];
	sprintf(buffer,"Particles:%d", mNumParticles);
	mFlowLabel->setText(buffer);
	lprintfln("NumPartiles:%d", mNumParticles);
}


