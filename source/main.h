/*
Copyright (c) 2002, Marcin 'Shard' Konicki
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Name "Marcin Konicki", "Shard" or any combination of them, must not be used to endorse or promote products derived from this software without specific prior written permission from Marcin Konicki.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __WMM_H_
#define __WMM_H_

//----------------------------------------------------------------------------
//
//	Include
//
//----------------------------------------------------------------------------

#include <Application.h>
#include <Bitmap.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Region.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <string.h>

#include "SSlider.h"
#include "StripeView.h"
#include "icon.h"

//----------------------------------------------------------------------------
//
//	Define
//
//----------------------------------------------------------------------------

#define WMM_SIGNATURE "application/x-vnd.Shard.WMM"
#define WMM_SETTINGS_FILE "WhereIsMyMouse_settings"

const int32 WMM_MSG_SET_RADIUS = 'WSrd';
const int32 WMM_MSG_SET_CIRCLES = 'WScr';
const int32 WMM_MSG_SET_LWIDTH = 'WSlw';
const int32 WMM_MSG_SET_LSPACE = 'WSls';
const int32 WMM_MSG_SET_PULSE = 'WSpl';

const int32 WMM_RADIUS_MIN = 32;
const int32 WMM_CIRCLES_COUNT = 4;
const int32 WMM_LINE_WIDTH = 10;
const int32 WMM_LINE_SPACE = 7;
const int32 WMM_PULSE_RATE = 1000;
rgb_color WMM_COLOR = { 255, 203, 0, 255};

const int32 WMM_ALPHA_MIN = 55;
//const int32 WMM_ALPHA_MOD = ( 255 - WMM_ALPHA_MIN) / WMM_CIRCLES_COUNT;


struct WMM_SETTINGS
{
	int32		radius;
	int32		circles;
	int32		lwidth;
	int32		lspace;
	int32		pulse;
	rgb_color	color;
};

//----------------------------------------------------------------------------
//
//	Classes
//
//----------------------------------------------------------------------------

//---------------------------------------------------
//	WMM View
//---------------------------------------------------
class WMMSettingsView : public BView
{
	public:
							WMMSettingsView();
							~WMMSettingsView();
		void				AttachedToWindow();
		void				Draw( BRect updateRect);
		void				DrawDemo();
		void				MessageReceived( BMessage *msg);
		void				Pulse();
		void				InitDemo();
		
	private:
		StripeView			*fIconView;

		SSlider				*slidRadius;
		SSlider				*slidCircles;
		SSlider				*slidLineWidth;
		SSlider				*slidLineSpace;
		SSlider				*slidPulseRate;
		
		BView				*fDemoView;
		BStringView			*fAuthorView;

		BRect				demoRect;
		rgb_color			demoColor;
		int32				demoCircle;
		int32				demoRadius;
		int32				demoAlphaMod;

		BBitmap				*Icon;

		WMM_SETTINGS		Settings;
};

//---------------------------------------------------
//	WMM View
//---------------------------------------------------
class WMMView : public BView
{
	public:
							WMMView();
							~WMMView();
		void				AttachedToWindow();
		void				MouseDown( BPoint where);
		void				Pulse();
		
	private:
		BPoint				mousePoint;
		int32				alphaMod;
		int32				radiusMod;
		WMM_SETTINGS		Settings;
};

//---------------------------------------------------
//	WMM Application
//---------------------------------------------------
class WMMApp : public BApplication
{
	public:
							WMMApp( bool showSettingsPanel);
							~WMMApp();
};


//----------------------------------------------------------------------------
//
//	Functions
//
//----------------------------------------------------------------------------

void LoadSettings( WMM_SETTINGS *settings);
void SettingsToDefault( WMM_SETTINGS *settings);
void SaveSettings( WMM_SETTINGS *settings);

#endif /*__WMM_H_*/
