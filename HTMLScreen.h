/*
 * HTMLScreen.h
 *
 *  Created on: Oct 13, 2011
 *      Author: iraklis
 */

#include <Wormhole/FileUtil.h>
#include <NativeUI/Widgets.h>


// Namespaces we want to access.
using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.
using namespace Wormhole; // Class WebAppMoblet

#ifndef HTMLSCREEN_H_
#define HTMLSCREEN_H_

#include "OGLScreen.h"

class HTMLScreen : public Screen, public ButtonListener, public SensorListener, public WebViewListener
{
public:
	HTMLScreen(OGLScreen *oglScreen);

	~HTMLScreen();

	void createUI();

	virtual void webViewHookInvoked(WebView* webView, int hookType, MAHandle urlData);

	virtual void buttonClicked(Widget* button);

	virtual void sensorEvent(MASensor a);

	void shouldRender(bool render);

	WebView* getWebView();
private:
	Button* mAddButton;

	Button* mRemoveButton;

	WebView* mWebView;

	bool mShouldRender;

	OGLScreen *mOGLScreen;
};


#endif /* HTMLSCREEN_H_ */
