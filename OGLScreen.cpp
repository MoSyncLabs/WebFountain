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
	ax = ay = az = 0;

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

	//The widget that renders the animation
	mGLView = new GLView(MAW_GL_VIEW);
	mGLView->addGLViewListener(this);

	VerticalLayout* vLayout = new VerticalLayout();
	vLayout->addChild(hLayout);
	vLayout->addChild(mGLView);

	setMainWidget(vLayout);
	setTitle("Native Rendering");
}

void OGLScreen::buttonClicked(Widget* button)
{
	//Notify the JavaScript code that a button was clicked
	if(button == mAddButton){
		mHTMLScreen->getWebView()->callJS("increaseFlow()");
	}
	else if(button == mRemoveButton){
		mHTMLScreen->getWebView()->callJS("decreaseFlow()");
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

void OGLScreen::initVariables(HTMLScreen *htmlScreen,int maxParticles,
							int particleLifetime, float gravityScale,
							int screenWidth, int screenHeight)
{
	MAX_PARTICLES = maxParticles;
	PARTICLE_LIFETIME = particleLifetime;

	GRAVITY_SCALE = gravityScale;

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
	vcoords[0][0] = -1.0f; vcoords[0][1] = -1.0f; vcoords[0][2] = 0.0f;
	tcoords[1][0] = 0.0f;  tcoords[1][1] = 0.0f;
	vcoords[1][0] = 1.0f;  vcoords[1][1] = -1.0f; vcoords[1][2] = 0.0f;
	tcoords[2][0] = 0.0f;  tcoords[2][1] = 1.0f;
	vcoords[2][0] = 1.0f;  vcoords[2][1] = 1.0f; vcoords[2][2] = 0.0f;
	tcoords[3][0] = 1.0f;  tcoords[3][1] = 1.0f;
	vcoords[3][0] = -1.0f; vcoords[3][1] = 1.0f; vcoords[3][2] = 0.0f;

	// Set pointers to vertex coordinates and texture coordinates.
	glVertexPointer(3, GL_FLOAT, 0, vcoords);
	glTexCoordPointer(2, GL_FLOAT, 0, tcoords);

	// Enable texture and vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Render each active particle
	for(int i = 0; i < MAX_PARTICLES; i++) {
		if(mParticles[i].alive)
		{
			glPushMatrix();

			//Position the particle in the X and Y axis
			glTranslatef(mParticles[i].x, mParticles[i].y, 0.0f);

			//Scale to simulate the Z axis, since this is a 2D
			//image and we are using an Orthographic projection
			glScalef(mParticles[i].z / 2, mParticles[i].z / 2, 0.0f);

			// This draws the particle.
			glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

			glPopMatrix();
		}
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

	//The time since the last frame
	int cycleTime = (currentTime - mPrevTime);
	for(int i = 0; i < MAX_PARTICLES; i++) {

		particle* p = mParticles + i;
		if(p->alive == true)
		{
			//Each particle's time since the last frame is
			//usually equal to cycleTime, unless the particle
			//has just been created. Then it has a shorter time.
			int particleTime = cycleTime;
			if(p->addTime > mPrevTime)
			{
				particleTime = (currentTime - p->addTime);
			}

			//Calculate the new velocity vector
			p->zv += az*particleTime;
			p->xv += ax*particleTime;
			p->yv += ay*particleTime;

			//Calculate the new position vector
			p->z += p->zv*particleTime;
			p->x += p->xv*particleTime;
			p->y += p->yv*particleTime;
		}
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

void OGLScreen::sensorEvent(MASensor a)
{
	//Set the new gravity vector
	ax = a.values[0] * GRAVITY_SCALE;
	ay = a.values[1] * GRAVITY_SCALE;
	az = a.values[2] * GRAVITY_SCALE;
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

		//Deactivate any particles past their time
		removeOldParticles(currentTime);

		mPrevTime = currentTime;
	}
}

void OGLScreen::addNewParticles(float x, float y, float z,
								float xv, float yv, float zv)
{
	//Mark the time this particle was added
	int currentTime = maGetMilliSecondCount();

	//Find an inactive particle to initialize and activate
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		if(!mParticles[i].alive)
		{
			mParticles[i].alive = true;
			mParticles[i].addTime = currentTime;
			mParticles[i].xv = xv;
			mParticles[i].yv = yv;
			mParticles[i].zv = zv;
			mParticles[i].x = x;
			mParticles[i].y = y;
			mParticles[i].z = z;
			break;
		}
	}
}

void OGLScreen::removeOldParticles(int currentTime)
{
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		if(mParticles[i].alive == true && //if the particle is active
			(mParticles[i].addTime + PARTICLE_LIFETIME < currentTime //but past it's time
						|| mParticles[i].z < 0)) //Or if it fell below the 0 level
		{
			//Deactivate it
			mParticles[i].alive = false;
		}
	}
}


