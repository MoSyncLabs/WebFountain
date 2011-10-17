/**
 * @file main.cpp
 *
 * Sample application that illustrates how to call into C++
 * from JavaScript.
 */

// Include Moblet for web applications.
#include <Wormhole/WebAppMoblet.h>
#include <NativeUI/Widgets.h>
#include "HTMLScreen.h"
#include "OGLScreen.h"
#include "MAHeaders.h"

// Namespaces we want to access.
using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.
using namespace Wormhole; // Class WebAppMoblet

/**
 * The application class.
 */
class WebFountain : public Moblet, public TabScreenListener
{
public:
	WebFountain()
	{
		// Create file utility object.
		mFileUtil = new FileUtil();

		// Extract bundled files to the local file system.
		mFileUtil->extractLocalFiles();
		mHTMLScreen = new HTMLScreen(MAX_PARTICLES, MIN_FLOW, MAX_FLOW,
									PARTICLE_LIFETIME, GRAVITY_SCALE,
									INIT_VELOCITY);
		mOGLScreen = new OGLScreen(MAX_PARTICLES, MIN_FLOW, MAX_FLOW,
								PARTICLE_LIFETIME, GRAVITY_SCALE,
								INIT_VELOCITY, PARTICLE_IMAGE);
		Environment::getEnvironment().addSensorListener(mHTMLScreen);
		Environment::getEnvironment().addSensorListener(mOGLScreen);
		maSensorStart(1, 100);

		mTabScreen = new TabScreen();
		mTabScreen->addTab(mHTMLScreen);
		mTabScreen->addTab(mOGLScreen);
		mTabScreen->addTabScreenListener(this);

		mTabScreen->setActiveTab(0);
		mTabScreen->show();
		mHTMLScreen->shouldRender(true);
	}

	~WebFountain()
	{
		delete mHTMLScreen;
		delete mOGLScreen;
		delete mFileUtil;
	}

	virtual void tabScreenTabChanged(
            TabScreen* tabScreen,
            const int tabScreenIndex)
    {
    	if(tabScreenIndex == 0)
    	{
    		mHTMLScreen->shouldRender(true);
    		mOGLScreen->shouldRender(false);
    	}
    	else
    	{
    		mHTMLScreen->shouldRender(false);
    		mOGLScreen->shouldRender(true);
    	}
    }



private:
	HTMLScreen* mHTMLScreen;

	OGLScreen* mOGLScreen;

	FileUtil* mFileUtil;

	TabScreen* mTabScreen;

	static const int MAX_PARTICLES = 65;

	static const int MIN_FLOW = 1;

	static const int MAX_FLOW = 10;

	static const int PARTICLE_LIFETIME = 5000;

	static const float GRAVITY_SCALE = 0.002;

	static const float INIT_VELOCITY = 1.5;
};

/**
 * Main function that is called when the program starts.
 * Here an instance of the MyMoblet class is created and
 * the program enters the main event loop.
 */
extern "C" int MAMain()
{
	Moblet::run(new WebFountain());
	return 0;
}
