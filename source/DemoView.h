/*
 * Copyright 2017 Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Author:
 *		Marcin 'Shard' Konicki
 *		Owen Pan <owen.pan@yahoo.com>
 */

#ifndef DEMO_VIEW_H
#define DEMO_VIEW_H

#include <Region.h>
#include <View.h>
#include <Window.h>

const int32 WMM_MSG_SET_RADIUS = 'WSrd';
const int32 WMM_MSG_SET_CIRCLES = 'WScr';
const int32 WMM_MSG_SET_LWIDTH = 'WSlw';
const int32 WMM_MSG_SET_LSPACE = 'WSls';
const int32 WMM_MSG_SET_PULSE = 'WSpl';

const int32 WMM_ALPHA_MIN = 55;
const int32 WMM_PULSE_MULTIPLIER = 100000;

struct WMM_SETTINGS
{
	int32		radius;
	int32		circles;
	int32		lwidth;
	int32		lspace;
	int32		pulse;
	rgb_color	color;
};

class DemoView : public BView
{
	public:
						DemoView(float bottom);
						~DemoView();
		void			AttachedToWindow();
		void			Draw(BRect updateRect);
		void			MessageReceived(BMessage *msg);
		void			Pulse();
		int32			radius() const { return fSettings.radius; }
		int32			circles() const { return fSettings.circles; }
		int32			lwidth() const { return fSettings.lwidth; }
		int32			lspace() const { return fSettings.lspace; }
		int32			pulse() const { return fSettings.pulse; }

	private:
		rgb_color		fColor;
		int32			fAlphaMod;
		int32			fCircle;
		int32			fRadius;
		WMM_SETTINGS	fSettings;
		void			ProcMsg(BMessage *msg, int32 &value, bool pulse = false);
		void			RestartDemo();
		void			InitDemo(bool restart = true);
};

void LoadSettings( WMM_SETTINGS *settings);
void SaveSettings( WMM_SETTINGS *settings);

#endif /* DEMO_VIEW_H */
