/*
 * OGLScreen.h
 *
 *  Created on: Oct 13, 2011
 *      Author: iraklis
 */


#include <NativeUI/Widgets.h>
#include <GLES/gl.h>
#include "HTMLScreen.h"


// Namespaces we want to access.
using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.

#ifndef OGLSCREEN_H_
#define OGLSCREEN_H_

class HTMLScreen;

//A struct that describes a single particle
typedef struct particle{
	bool alive; //Whether the particle exists or not
	int addTime; //The time that the particle was created
	//Position vector
	float x;
	float y;
	float z;
	//Velocity vector
	float xv;
	float yv;
	float zv;

};


class OGLScreen :	public Screen,
					public GLViewListener,
					public SensorListener,
					public TimerListener,
					public ButtonListener
{
public:
	OGLScreen(MAHandle particleImage);

	~OGLScreen();

	/**
	 * This method creates the user interface of the screen
	 */
	void createUI();

	/**
	 * This method is called when the GLView has been initialized
	 * and it's ready to accept OpenGL commands
	 * @param glView The GLView that was initialized
	 */
	virtual void glViewReady(GLView* glView);

	/**
	 * This method initializes the parameters of the environment that
	 * the particles live in, and also sets the screen that handles communication
	 * with JavaScript
	 * @param htmlScreen The screen that handles the JavaScript code
	 * @param maxParticles The maximum number of particles that will be rendered
	 * @param particleLifetime How long a particle will exists after it's creation
	 * @param gravityScale How strong is the gravity
	 * @param screenWidth The width of the area that willbe used for rendering
	 * @param screenHeight The height of the area that willbe used for rendering
	 *
	 */
	void initVariables(HTMLScreen *htmlScreen,int maxParticles, int particleLifetime,
							float gravityScale, int screenWidth, int screenHeight);

	/**
	 * This method will initialize the particle texture
	 */
	void createTexture();

	/**
	 * This method will initialize the OpenGL viewport to fill the GLView,
	 * And set the coordinate system to the specified size
	 * @param width The width of the coordinate system
	 * @param height The height of the coordinate system
	 */
	void setViewport(int width, int height);

	/**
	 * This method will initialize the OpenGL to the parameters
	 * that are needed by our particular animation
	 */
	void initGL();

	/**
	 * This method handles button clicks that this screen manages.
	 * @param button The button that was clicked
	 */
	virtual void buttonClicked(Widget* button);

	/**
	 * This method calculates the new position of the particles
	 * and renders a frame each time it's called.
	 * @currentTime the current animation time
	 */
	void draw(int currentTime);

	/**
	 * This method renders the particles on the screen
	 */
	void renderParticleObject();

	/**
	 * Tell the screen whether it should render the animation or not
	 * @param render The state of rendering.
	 */
	void shouldRender(bool render);

	/**
	 * This method is called whenever the timer is fired,
	 * and controls the animation
	 */
	virtual void runTimerEvent();

	/**
	 * This method handles accelerometer events
	 * @param a A struct containing the accelerometer info
	 */
	virtual void sensorEvent(MASensor a);

	/**
	 * This method adds a new particle to the rendering with the specified parameters
	 * @param x The horizontal position of the particle.
	 * @param y The vertical position of the particle.
	 * @param z The height of the particle.
	 * @param xv The particle's velocity on the x axis
	 * @param yv The particle's velocity on the y axis
	 * @param zv The particle's velocity on the z axis
	 */
	void addNewParticles(float x, float y, float z,
						float xv, float yv, float zv, int flow);

	/**
	 * Checks for particles past their lifetime, and removes them
	 * @currentTime the current animation time
	 */
	void removeOldParticles(int currentTime);

	/**
	 * This method enables or disables the Increase flow button
	 * @state The new state of the button
	 */
	void enableAddButton(bool state);

	/**
	 * This method enables or disables the Decrease flow button
	 * @state The new state of the button
	 */
	void enableRemoveButton(bool state);
private:

	Button* mAddButton; //Increase flow

	Button* mRemoveButton; //Decrease flow

	HTMLScreen *mHTMLScreen; //The screen that handles communication with JS

	MAHandle mParticleImageHandle; //The image resource for particles

	GLuint mParticleTexture; //The particle texture used by OpenGL

	bool mGLViewInitialized; //Initialization flag for the GLView wiget

	bool mVariablesInitialized; //Initialization flag for the animation parameters

	bool mEnvironmentInitialized; //Initialization flag for rendering

	Label* mFPSLabel; //A label for showing the frame rate

	Label* mFlowLabel; //A label for showing the flow rate

	GLView* mGLView; //The widget that renders the frames

	particle* mParticles; //The list of particles

	bool mShouldRender; //The state of rendering

	int mPrevTime; //The previous time that a frame was rendered

	int mTotalTime; //Used to average the FPS value for several frames

	int mFrameCounter; //Used to count when it needs to update the FPS

	float ax, ay, az; //The gravity vector

	int MAX_PARTICLES; //The maximum number of partiles

	int PARTICLE_LIFETIME; //The lifetime of each partile, in milliseconds

	float GRAVITY_SCALE; //The strenght of gravity

	int SCREN_WIDTH; //The width of the screen

	int SCREEN_HEIGHT; //The height of the screen
};


#endif /* OGLSCREEN_H_ */
