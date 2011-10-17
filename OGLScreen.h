/*
 * OGLScreen.h
 *
 *  Created on: Oct 13, 2011
 *      Author: iraklis
 */


#include <NativeUI/Widgets.h>
#include <GLES/gl.h>

// Namespaces we want to access.
using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.

#ifndef OGLSCREEN_H_
#define OGLSCREEN_H_

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
					public WebViewListener,
					public TimerListener
{
public:
	OGLScreen(int maxParticles, int minFlow, int maxFlow,
			int particleLifetime, float gravityScale, float initVelocity,
			MAHandle particleImage);

	~OGLScreen();

	void createUI();

	virtual void webViewHookInvoked(WebView* webView, int hookType, MAHandle urlData);

	virtual void sensorEvent(MASensor a);

	virtual void glViewReady(GLView* glView);

	void createTexture();

	void setViewport(int width, int height);

	void initGL();

	void draw(int currentTime);

	void renderParticleObject();

	void shouldRender(bool render);

	virtual void runTimerEvent();

	void addNewParticles(int currentTime);

	void removeOldParticles(int currentTime);
private:
	WebView* mWebView;

	MAHandle mParticleImageHandle;

	GLuint mParticleTexture;

	bool mGLViewInitialized;

	GLView* mGLView;

	particle* mParticles;

	int mFlow;

	int mInnerWidth;

	bool mShouldRender;

	int mPrevTime;

	int mTimeToNextParticle;

	float ax, ay, az;

	int MAX_PARTICLES;

	int MIN_FLOW;

	int MAX_FLOW;

	int PARTICLE_LIFETIME;

	float GRAVITY_SCALE;

	float INIT_VELOCITY;
};


#endif /* OGLSCREEN_H_ */
