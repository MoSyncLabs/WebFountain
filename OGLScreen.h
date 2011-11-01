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

typedef struct particle{
	bool alive;
	int addTime;
	float x;
	float y;
	float z;
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

	void createUI();

	virtual void sensorEvent(MASensor a);

	virtual void glViewReady(GLView* glView);

	void initVariables(HTMLScreen *htmlScreen,int maxParticles, int particleLifetime, float gravityScale, int screenWidth, int screenHeight);

	void createTexture();

	void setViewport(int width, int height);

	void initGL();

	virtual void buttonClicked(Widget* button);

	void draw(int currentTime);

	void renderParticleObject();

	void shouldRender(bool render);

	virtual void runTimerEvent();

	void addNewParticles(float x, float y, float z, float xv, float yv, float zv);

	void removeOldParticles(int currentTime);

	void enableAddButton(bool state);

	void enableRemoveButton(bool state);
private:

	Button* mAddButton;

	Button* mRemoveButton;

	HTMLScreen *mHTMLScreen;

	MAHandle mParticleImageHandle;

	GLuint mParticleTexture;

	bool mGLViewInitialized;

	bool mVariablesInitialized;

	bool mEnvironmentInitialized;

	GLView* mGLView;

	particle* mParticles;

	bool mShouldRender;

	int mPrevTime;

	float ax, ay, az;

	int MAX_PARTICLES;

	int PARTICLE_LIFETIME;

	float GRAVITY_SCALE;

	int SCREN_WIDTH;

	int SCREEN_HEIGHT;
};


#endif /* OGLSCREEN_H_ */
