/*
 * Copyright 2017 Haiku, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Marcin 'Shard' Konicki
 *		Owen Pan <owen.pan@yahoo.com>
 */

#include "DemoView.h"

//---------------------------------------------------
//	Constructor
//---------------------------------------------------
DemoView::DemoView()
:	BView(BRect(0, 0, 190, 50), NULL, 0, B_PULSE_NEEDED | B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE)
{
	LoadSettings(&Settings);

	SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	SetDrawingMode(B_OP_ALPHA);

	fAlphaMod = (255 - WMM_ALPHA_MIN) / Settings.circles;
	fCircle = 0;
	InitDemo();
}

//---------------------------------------------------
//	Destructor
//---------------------------------------------------
DemoView::~DemoView()
{
	SaveSettings(&Settings);
}

//---------------------------------------------------
//	Attached to window - set siblings target to this
//---------------------------------------------------
void
DemoView::AttachedToWindow()
{
	Window()->SetPulseRate(Settings.pulse);
}

//---------------------------------------------------
//	Draw view
//---------------------------------------------------
void
DemoView::Draw(BRect updateRect)
{
	//demo background
	SetHighColor(255, 255, 255, 255);
	FillRoundRect(updateRect, 2, 2);
	SetHighColor(0, 0, 0, 255);
	StrokeRoundRect(updateRect, 2, 2);

	float left = updateRect.left + 1;
	float top = updateRect.top + 1;
	float right = updateRect.right - 1;
	float bottom = updateRect.bottom - 1;

	float oldPen = PenSize();
	SetPenSize(Settings.lwidth);
	BRegion region = BRegion(BRect(left, top, right, bottom));
	ConstrainClippingRegion(&region);

	for (int i = 0; i < fCircle; i++)
	{
		SetHighColor(fColor);
		StrokeEllipse(BPoint((left + right) / 2, (top + bottom) / 2), fRadius, fRadius);
		fRadius += Settings.lwidth / 2 + Settings.lspace;
		fColor.alpha += fAlphaMod;
	}

	ConstrainClippingRegion(NULL);
	SetPenSize(oldPen);
}

//---------------------------------------------------
//	Message handler
//---------------------------------------------------
void
DemoView::MessageReceived(BMessage *msg)
{
	// drag & drop
	if (msg->WasDropped())
	{
		rgb_color*	color;
		ssize_t		size;
		if (msg->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void **) &color, &size) == B_OK && Settings.color != *color)
		{
			Settings.color = *color;
			RestartDemo();
		}
		return;
	}

	switch(msg->what)
	{
		case WMM_MSG_SET_RADIUS:
		{
			ProcMsg(msg, Settings.radius);
			break;
		}
		case WMM_MSG_SET_CIRCLES:
		{
			ProcMsg(msg, Settings.circles);
			break;
		}
		case WMM_MSG_SET_LWIDTH:
		{
			ProcMsg(msg, Settings.lwidth);
			break;
		}
		case WMM_MSG_SET_LSPACE:
		{
			ProcMsg(msg, Settings.lspace);
			break;
		}
		case WMM_MSG_SET_PULSE:
		{
			ProcMsg(msg, Settings.pulse, 100000);
			break;
		}
		default:
			BView::MessageReceived(msg);
	}
}

//---------------------------------------------------
//	Process message
//---------------------------------------------------
void
DemoView::ProcMsg(BMessage *msg, int32 &value, int32 factor)
{
	int32 current = value;

	msg->FindInt32("be:value", &value);
	value *= factor;

	if (value != current) RestartDemo();
}

//---------------------------------------------------
//	Animate demo circles
//---------------------------------------------------
void
DemoView::Pulse()
{
	if (fCircle < Settings.circles) fCircle++;
	else fCircle = 0;

	InitDemo();
	Invalidate();
}

//---------------------------------------------------
//	Restart demo
//---------------------------------------------------
void
DemoView::RestartDemo()
{
	fCircle = 0;
	InitDemo();
	Invalidate();
}

void
DemoView::InitDemo()
{
	fRadius = Settings.radius;
	fColor = Settings.color;
	fColor.alpha = WMM_ALPHA_MIN;
}
