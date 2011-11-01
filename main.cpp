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
		mOGLScreen = new OGLScreen(PARTICLE_IMAGE);
		mHTMLScreen = new HTMLScreen(mOGLScreen);

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
