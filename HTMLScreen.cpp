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
using namespace Wormhole; // Class WebAppMoblet

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
	mAddButton = new Button();
	mAddButton->fillSpaceHorizontally();
	mAddButton->wrapContentVertically();
	mAddButton->setText("Increase");
	mAddButton->addButtonListener(this);

	mRemoveButton = new Button();
	mRemoveButton->fillSpaceHorizontally();
	mRemoveButton->wrapContentVertically();
	mRemoveButton->setText("Decrease");
	mRemoveButton->addButtonListener(this);

	HorizontalLayout* hLayout = new HorizontalLayout();
	hLayout->wrapContentVertically();
	hLayout->addChild(mAddButton);
	hLayout->addChild(mRemoveButton);

	mWebView = new WebView();
	mWebView->fillSpaceHorizontally();
	mWebView->fillSpaceVertically();

	VerticalLayout* vLayout = new VerticalLayout();
	vLayout->addChild(hLayout);
	vLayout->addChild(mWebView);

	setMainWidget(vLayout);

	setTitle("HTML Rendering");
}

/**
 * This method handles messages sent from the WebView.
 * @param webView The WebView that sent the message.
 * @param urlData Data object that holds message content.
 * Note that the data object will be valid only during
 * the life-time of the call of this method, then it
 * will be deallocated.
 */
void HTMLScreen::webViewHookInvoked(WebView* webView, int hookType, MAHandle urlData)
{
	// Create message object. This parses the message.
	WebViewMessage message(webView, urlData);
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

	if (message.is("initOGLVariables"))
	{
		mOGLScreen->initVariables(
				this,
				message.getParamInt("MAX_PARTICLES"),
				message.getParamInt("PARTICLE_LIFETIME"),
				message.getParamInt("GRAVITY_SCALE")/(float)1000,
				message.getParamInt("SCREN_WIDTH"),
				message.getParamInt("SCREEN_HEIGHT")
		);
	}

	if (message.is("newParticle"))
	{
		mOGLScreen->addNewParticles(
				message.getParamInt("x"),
				message.getParamInt("y"),
				message.getParamInt("z"),
				message.getParamInt("xv")/(float)1000,
				-message.getParamInt("yv")/(float)1000,
				message.getParamInt("zv")/(float)1000
		);
	}

	// Tell the WebView that we have processed the message, so that
	// it can send the next one.
	mWebView->callJS("bridge.messagehandler.processedMessage()");
}

void HTMLScreen::buttonClicked(Widget* button)
{
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
	char buffer[128];
	sprintf(buffer,"setGravity(%f, %f, %f)",a.values[0],a.values[1],a.values[2]);
	mWebView->callJS(buffer);
}

void HTMLScreen::shouldRender(bool render)
{
	mShouldRender = render;
	if(render)
	{
		mWebView->callJS("shouldRender(true)");
	}
	else
	{
		mWebView->callJS("shouldRender(false)");
	}
}


