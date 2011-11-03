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

class HTMLScreen : 	public Screen,
					public ButtonListener,
					public SensorListener,
					public WebViewListener
{
public:
	HTMLScreen(OGLScreen *oglScreen);

	~HTMLScreen();

	/**
	 * This method creates the user interface of the screen
	 */
	void createUI();

	/**
	 * This method handles messages sent from the WebView.
	 * @param webView The WebView that sent the message.
	 * @param urlData Data object that holds message content.
	 * Note that the data object will be valid only during
	 * the life-time of the call of this method, then it
	 * will be deallocated.
	 */
	virtual void webViewHookInvoked(WebView* webView, int hookType, MAHandle urlData);

	/**
	 * This method handles button clicks that this screen manages.
	 * @param button The button that was clicked
	 */
	virtual void buttonClicked(Widget* button);

	/**
	 * This method handles accelerometer events
	 * @param a A struct containing the accelerometer info
	 */
	virtual void sensorEvent(MASensor a);

	/**
	 * Tell the screen whether it should render the animation or not
	 * @param render The state of rendering.
	 */
	void shouldRender(bool render);

	/**
	 * This method returns the webview that this screen manages
	 * @param render The state of rendering.
	 */
	WebView* getWebView();
private:
	Button* mAddButton; //Increases the flow of particles

	Button* mRemoveButton; //Decreases the flow of particles

	WebView* mWebView; //Renders the animation in Javascript

	OGLScreen *mOGLScreen; //The OpenGL rendering screen

	float PRECISION; //Precision of floating point numbers
};


#endif /* HTMLSCREEN_H_ */
