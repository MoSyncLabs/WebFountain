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

HTMLScreen::HTMLScreen(int maxParticles, int minFlow, int maxFlow,
		int particleLifetime, float gravityScale, float initVelocity):Screen()
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

	sprintf(mBuffer,"init(%d, %d, %d, %d, %f, %f)", maxParticles, minFlow, maxFlow,
								particleLifetime, gravityScale, initVelocity);
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

	setTitle("HTML Render/Native Buttons");
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
		}
		else
		{
			mAddButton->setEnabled(false);
			mAddButton->setFontColor(0x969696);
		}

		if(message.getParam("remove") == "true")
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

	if (message.is("pageLoaded"))
	{
		mWebView->callJS(mBuffer);
		shouldRender(mShouldRender);
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


