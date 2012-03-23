/*
 * HTMLScreen.cpp
 *
 *  Created on: Oct 13, 2011
 *      Author: iraklis
 */

#include "HTMLScreen.h"
#include <Wormhole/WebViewMessage.h>

// Namespaces we want to access.
using namespace MAUtil; // Class Moblet
using namespace NativeUI; // WebView widget.
using namespace Wormhole; // Class WebViewMessage

HTMLScreen::HTMLScreen(OGLScreen *oglScreen):Screen()
{
	createUI();
	// Enable message sending from JavaScript to C++.
	mWebView->enableWebViewMessages();

	// Remove this line to enable the user to
	// zoom the web page. To disable zoom is one
	// way of making web pages display in a
	// reasonable degault size on devices with
	// different screen sizes.
	mWebView->disableZoom();

	mWebView->addWebViewListener(this);

	// The page in the "LocalFiles" folder to
	// show when the application starts.
	mWebView->openURL("fountain.html");
	mOGLScreen = oglScreen;
}

HTMLScreen::~HTMLScreen()
{

}

void HTMLScreen::createUI()
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

	//The layout that holds the buttons
	HorizontalLayout* hLayout = new HorizontalLayout();
	hLayout->wrapContentVertically();
	hLayout->addChild(mAddButton);
	hLayout->addChild(mRemoveButton);

	//The webview that renders the animation
	mWebView = new WebView();
	mWebView->fillSpaceHorizontally();
	mWebView->fillSpaceVertically();

	//The layout that forms the screen
	VerticalLayout* vLayout = new VerticalLayout();
	vLayout->addChild(hLayout);
	vLayout->addChild(mWebView);

	setMainWidget(vLayout);

	setTitle("HTML Rendering");
}

void HTMLScreen::webViewHookInvoked(WebView* webView, int hookType, MAHandle urlData)
{
	// Create message object. This parses the message.
	WebViewMessage message(webView, urlData);

	//This event tells us whether we should disable
	//a button or not. This happens if we hit the
	//maximum or minimum flow of particles
	//We also notify the OpenGL screen because it also manages
	//a set of buttons.
	if (message.is("enableButtons"))
	{
		if(message.getParam("add") == "true")
		{
			mAddButton->setEnabled(true);
			mAddButton->setFontColor(0x000000);
			mOGLScreen->enableAddButton(true);
		}
		else
		{
			mAddButton->setEnabled(false);
			mAddButton->setFontColor(0x969696);
			mOGLScreen->enableAddButton(false);
		}

		if(message.getParam("remove") == "true")
		{
			mRemoveButton->setEnabled(true);
			mRemoveButton->setFontColor(0x000000);
			mOGLScreen->enableRemoveButton(true);
		}
		else
		{
			mRemoveButton->setEnabled(false);
			mRemoveButton->setFontColor(0x969696);
			mOGLScreen->enableRemoveButton(false);
		}
	}

	//The parameters that define the behavior of the animation
	if (message.is("initOGLVariables"))
	{
		//PRECISION is used to convert floating point numbers to integers,
		//and then back again. This makes communication between JS and C++
		//more efficient
		PRECISION = message.getParamInt("PRECISION");

		mOGLScreen->initVariables(
				this, //We also pass a reference to this screen
				message.getParamInt("MAX_PARTICLES"),
				message.getParamInt("PARTICLE_LIFETIME"),
				message.getParamInt("GRAVITY_SCALE") / PRECISION,
				message.getParamInt("SCREN_WIDTH"),
				message.getParamInt("SCREEN_HEIGHT")
		);
	}

	//This message is received whenever a new particle is generated
	//by the JS code, and needs to be passed to OpenGL for rendering
	if (message.is("newParticle"))
	{
		mOGLScreen->addNewParticles(
				message.getParamInt("x"),
				message.getParamInt("y"),
				message.getParamInt("z"),
				message.getParamInt("xv") / PRECISION,
				message.getParamInt("yv") / PRECISION,
				message.getParamInt("zv") / PRECISION,
				message.getParamInt("flow")
		);
	}

	// Tell the WebView that we have processed the message, so that
	// it can send the next one.
	mWebView->callJS("bridge.messagehandler.processedMessage()");
}

void HTMLScreen::buttonClicked(Widget* button)
{
	//Notify JS that the user has clicked a button
	if(button == mAddButton){
		mWebView->callJS("increaseFlow()");
	}
	else if(button == mRemoveButton){
		mWebView->callJS("decreaseFlow()");
	}
}

WebView* HTMLScreen::getWebView()
{
	return mWebView;
}

void HTMLScreen::sensorEvent(MASensor a)
{
	//We make a call to JS whenever we have new accelerometer data.
	char buffer[128];
	sprintf(buffer,"setGravity(%f, %f, %f)",a.values[0],a.values[1],a.values[2]);
	mWebView->callJS(buffer);
}

void HTMLScreen::shouldRender(bool render)
{
	//We need to notify JS whether it needs to start or stop rendering
	if(render)
	{
		mWebView->callJS("shouldRender(true)");
	}
	else
	{
		mWebView->callJS("shouldRender(false)");
	}
}


