/*
Copyright (c) 2002, Marcin 'Shard' Konicki
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Name "Marcin Konicki", "Shard" or any combination of them, must not be used to endorse or promote products derived from this software without specific prior written permission from Marcin Konicki.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//----------------------------------------------------------------------------
//
//	Include
//
//----------------------------------------------------------------------------

#include "main.h"

//----------------------------------------------------------------------------
//
//	Main Functions
//
//----------------------------------------------------------------------------

//---------------------------------------------------
//	Load settings to given settings struct
//---------------------------------------------------
void
LoadSettings( WMM_SETTINGS *settings)
{
	BPath settings_file_path;
	if( find_directory( B_USER_SETTINGS_DIRECTORY, &settings_file_path) != B_OK)
		settings_file_path.SetTo( WMM_SETTINGS_FILE);
	else
		settings_file_path.Append( WMM_SETTINGS_FILE);

	FILE *file;
	if( (file = fopen( settings_file_path.Path(), "rb+")))
	{
		if( !fread( settings, sizeof( WMM_SETTINGS), 1, file))
		{
			// settings file was smaller, make default settings first and then load again
			// this prevents from older settings lost, when some new will be added
			SettingsToDefault( settings);
			fseek( file, 0, SEEK_SET);
			fread( settings, sizeof( WMM_SETTINGS), 1, file);
			fclose( file);
			// save modified settings
			SaveSettings( settings);
			// return since file is closed now
			return;
		}
		fclose( file);
	}
	else
	{
		SettingsToDefault( settings);
		SaveSettings( settings);
	}
}

//---------------------------------------------------
//	Set given settings struct to defaults
//---------------------------------------------------
void
SettingsToDefault( WMM_SETTINGS *settings)
{
	settings->radius = WMM_RADIUS_MIN;
	settings->circles = WMM_CIRCLES_COUNT;
	settings->lwidth = WMM_LINE_WIDTH;
	settings->lspace = WMM_LINE_SPACE;
	settings->pulse = WMM_PULSE_RATE;
	settings->color = WMM_COLOR;
}

//---------------------------------------------------
//	Save settings from given settings struct
//---------------------------------------------------
void
SaveSettings( WMM_SETTINGS *settings)
{
	BPath settings_file_path;
	if( find_directory( B_USER_SETTINGS_DIRECTORY, &settings_file_path) != B_OK)
		settings_file_path.SetTo( WMM_SETTINGS_FILE);
	else
		settings_file_path.Append( WMM_SETTINGS_FILE);

	FILE *file;
	if( (file = fopen( settings_file_path.Path(), "wb+")))
	{
		fwrite( settings, sizeof( WMM_SETTINGS), 1, file);
		fclose( file);
	}
}

//---------------------------------------------------
//	WMM main function
//---------------------------------------------------
void
usage()
{
	printf("'Where is my Mouse' shows a short animation to easily locate the "
		"mouse pointer.\n"
		"Use the Shortcuts preferences to assign your preferred key combo.\n"
		"usage:\tWhereIsMyMouse [s|-s|--set]\n"
		"\ts, -s, --set\tShow the settings panel\n\n");
}

int
main(int argc, char** argv)
{
	bool showSettingsPanel = false;
	if( argc > 1) {
		if (!strcmp(argv[1], "s") || !strcmp(argv[1], "-s") || !strcmp(argv[1], "--set"))
			showSettingsPanel = true;
		else {
			usage();
			return B_ERROR;
		}
	}
		
	WMMApp* App = new WMMApp( showSettingsPanel);
	App->Run();
	delete App;

	return B_NO_ERROR;
}



//----------------------------------------------------------------------------
//
//	Functions :: WMMApp
//
//----------------------------------------------------------------------------

//---------------------------------------------------
//	Constructor
//---------------------------------------------------
WMMApp::WMMApp( bool showSettingsPanel)
:	BApplication( WMM_SIGNATURE)
{
	BWindow *window;

	if( showSettingsPanel)
	{
		window = new BWindow( BRect( 200, 200, 200, 200), "What would You like?", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_QUIT_ON_WINDOW_CLOSE, B_ALL_WORKSPACES);
		window->AddChild( new WMMSettingsView());
	}
	else
	{
		window = new BWindow( BRect( 0, 0, 0, 0), "Here it is!", B_NO_BORDER_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL, B_NOT_MOVABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_QUIT_ON_WINDOW_CLOSE, B_ALL_WORKSPACES);
		window->AddChild( new WMMView());
	}
	
	window->Show();
}

//---------------------------------------------------
//	Destructor
//---------------------------------------------------
WMMApp::~WMMApp()
{
}


//----------------------------------------------------------------------------
//
//	Functions :: WMMView
//
//----------------------------------------------------------------------------

//---------------------------------------------------
//	Constructor
//---------------------------------------------------
WMMView::WMMView()
:	BView( BRect( 0, 0, 0, 0), "Circles, circles...", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_PULSE_NEEDED | B_FULL_UPDATE_ON_RESIZE)
{
	LoadSettings( &Settings);

	SetBlendingMode( B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	SetDrawingMode( B_OP_ALPHA);

	SetViewColor( B_TRANSPARENT_32_BIT);
	SetPenSize( Settings.lwidth);

	Settings.color.alpha = WMM_ALPHA_MIN;

	alphaMod = ( 255 - WMM_ALPHA_MIN) / Settings.circles;
	radiusMod = ( Settings.lwidth / 2) + Settings.lspace;
}

//---------------------------------------------------
//	Destructor
//---------------------------------------------------
WMMView::~WMMView()
{
}

//---------------------------------------------------
//	Draw gets mouse point
//---------------------------------------------------
void
WMMView::AttachedToWindow()
{
	int32 size = (Settings.radius + ( Settings.circles * ( (Settings.lwidth / 2) + Settings.lspace))) * 2;
	Window()->ResizeTo( size, size);
	ResizeTo( size, size);

	uint32 buttons;
	GetMouse( &mousePoint, &buttons);
	size /= 2;
	Window()->MoveTo( mousePoint.x-size, mousePoint.y-size);
	ConvertFromScreen( &mousePoint);

	Window()->SetPulseRate( Settings.pulse);
}

//---------------------------------------------------
//	On mouse down quit
//---------------------------------------------------
void
WMMView::MouseDown( BPoint where)
{
	Window()->PostMessage( B_QUIT_REQUESTED);
}

//---------------------------------------------------
//	Pulse - draw circles around mouse cursor
//---------------------------------------------------
void
WMMView::Pulse()
{
	if( Settings.circles-- < 1)
		Window()->PostMessage( B_QUIT_REQUESTED);
	else
	{
		SetHighColor( Settings.color);
		StrokeEllipse( mousePoint, Settings.radius, Settings.radius);

		Settings.radius += radiusMod;
		Settings.color.alpha += alphaMod;
	}
}


//----------------------------------------------------------------------------
//
//	Functions :: WMMSettingsView
//
//----------------------------------------------------------------------------

//---------------------------------------------------
//	Constructor
//---------------------------------------------------
WMMSettingsView::WMMSettingsView()
:	BView( BRect( 0, 0, 0, 0), "Maybe this, or that, or...", B_FOLLOW_ALL_SIDES, B_PULSE_NEEDED | B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE)
{
	LoadSettings( &Settings);

	SetBlendingMode( B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
	SetDrawingMode( B_OP_ALPHA);

	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR));

	Icon = new BBitmap( BRect( 0, 0, 31, 31), B_RGBA32);
	memcpy( Icon->Bits(), WMM_ICON_DATA, 32*32*4);
	
	float left = 54;

	preferredWidth = left;
	preferredHeight = 10;
	BRect rect;
	rgb_color color = ui_color(B_MENU_SELECTION_BACKGROUND_COLOR);

	//
	slidRadius = new SSlider( BRect( left, preferredHeight, left+200, preferredHeight), "slidRadius", "Circle radius at start", new BMessage( WMM_MSG_SET_RADIUS), 16, 64);
	slidRadius->SetHashMarks( B_HASH_MARKS_BOTTOM);
	slidRadius->SetHashMarkCount( 48);
	//slidRadius->SetLimitLabels("16", "64");
	slidRadius->UseFillColor( true, &color);
	slidRadius->SetValue( Settings.radius);
	slidRadius->ResizeToPreferred();

	rect = slidRadius->Frame();
	if( rect.right > preferredWidth) preferredWidth = rect.right;
	if( rect.bottom > preferredHeight) preferredHeight = rect.bottom + 5;

	//
	slidCircles = new SSlider( BRect( left, preferredHeight, left+200, preferredHeight), "slidRadius", "How many circles?", new BMessage( WMM_MSG_SET_CIRCLES), 1, 15);
	slidCircles->SetHashMarks( B_HASH_MARKS_BOTTOM);
	slidCircles->SetHashMarkCount( 15);
	slidCircles->UseFillColor( true, &color);
	slidCircles->SetValue( Settings.circles);
	slidCircles->ResizeToPreferred();

	rect = slidCircles->Frame();
	if( rect.right > preferredWidth) preferredWidth = rect.right;
	if( rect.bottom > preferredHeight) preferredHeight = rect.bottom + 5;

	//
	slidLineWidth = new SSlider( BRect( left, preferredHeight, left+200, preferredHeight), "slidRadius", "Circle's width", new BMessage( WMM_MSG_SET_LWIDTH), 1, 32);
	slidLineWidth->SetHashMarks( B_HASH_MARKS_BOTTOM);
	slidLineWidth->SetHashMarkCount( 32);
	slidLineWidth->UseFillColor( true, &color);
	slidLineWidth->SetValue( Settings.lwidth);
	slidLineWidth->ResizeToPreferred();

	rect = slidLineWidth->Frame();
	if( rect.right > preferredWidth) preferredWidth = rect.right;
	if( rect.bottom > preferredHeight) preferredHeight = rect.bottom + 5;

	//
	slidLineSpace = new SSlider( BRect( left, preferredHeight, left+200, preferredHeight), "slidRadius", "Space width between circles", new BMessage( WMM_MSG_SET_LSPACE), 1, 32);
	slidLineSpace->SetHashMarks( B_HASH_MARKS_BOTTOM);
	slidLineSpace->SetHashMarkCount( 32);
	slidLineSpace->UseFillColor( true, &color);
	slidLineSpace->SetValue( Settings.lspace);
	slidLineSpace->ResizeToPreferred();

	rect = slidLineSpace->Frame();
	if( rect.right > preferredWidth) preferredWidth = rect.right;
	if( rect.bottom > preferredHeight) preferredHeight = rect.bottom + 5;


	//
	slidPulseRate = new SSlider( BRect( left, preferredHeight, left+200, preferredHeight), "slidRadius", "Animation speed", new BMessage( WMM_MSG_SET_PULSE), 1, 5);
	slidPulseRate->SetHashMarks( B_HASH_MARKS_BOTTOM);
	slidPulseRate->SetHashMarkCount( 5);
	slidPulseRate->UseFillColor( true, &color);
	slidPulseRate->SetValue( Settings.pulse / 100000);
	slidPulseRate->ResizeToPreferred();

	rect = slidPulseRate->Frame();
	if( rect.right > preferredWidth) preferredWidth = rect.right;
	if( rect.bottom > preferredHeight) preferredHeight = rect.bottom+15;
	
	//
	demoRect = BRect( left, preferredHeight, left+190, preferredHeight+50);
	demoPoint = BPoint( demoRect.left + ( demoRect.Width() / 2), demoRect.top + ( demoRect.Height() / 2));
	demoAlphaMod = ( 255 - WMM_ALPHA_MIN) / Settings.circles;
	InitDemo();

	if( demoRect.right > preferredWidth) preferredWidth = demoRect.right;
	if( demoRect.bottom > preferredHeight) preferredHeight = demoRect.bottom;

	preferredHeight += 8;
	ResizeToPreferred();

	AddChild( slidRadius);
	AddChild( slidCircles);
	AddChild( slidLineWidth);
	AddChild( slidLineSpace);
	AddChild( slidPulseRate);
}

//---------------------------------------------------
//	Destructor
//---------------------------------------------------
WMMSettingsView::~WMMSettingsView()
{
	slidRadius->RemoveSelf();
	delete slidRadius;
	slidCircles->RemoveSelf();
	delete slidCircles;
	slidLineWidth->RemoveSelf();
	delete slidLineWidth;
	slidLineSpace->RemoveSelf();
	delete slidLineSpace;
	slidPulseRate->RemoveSelf();
	delete slidPulseRate;
	
	delete Icon;

	SaveSettings( &Settings);
}

//---------------------------------------------------
//	Attached to window - set siblings target to this
//---------------------------------------------------
void
WMMSettingsView::AttachedToWindow()
{
	slidRadius->SetTarget( this);
	slidCircles->SetTarget( this);
	slidLineWidth->SetTarget( this);
	slidLineSpace->SetTarget( this);
	slidPulseRate->SetTarget( this);
	Window()->ResizeTo( preferredWidth, preferredHeight);
	Window()->SetPulseRate( Settings.pulse);
}

//---------------------------------------------------
//	Draw view
//---------------------------------------------------
void
WMMSettingsView::Draw( BRect updateRect)
{
	rgb_color color = tint_color( ViewColor(), B_HIGHLIGHT_BACKGROUND_TINT);
	color.alpha = 255;
	SetHighColor( color);
	FillRect( BRect( 0, 0, 32, preferredHeight));
	DrawBitmap( Icon, BPoint( 16, 8));
	
	//demo background
	SetHighColor( 255, 255, 255, 255);
	FillRoundRect( demoRect, 2, 2);
	SetHighColor( 0, 0, 0, 255);
	StrokeRoundRect( demoRect, 2, 2);
	
	InitDemo();

	SetFontSize( 5);
	DrawString( "by Shard", BPoint( demoRect.right - StringWidth("by Shard") + 2, demoRect.bottom + 5));
}

//---------------------------------------------------
//	Draw demo
//---------------------------------------------------
void
WMMSettingsView::DrawDemo()
{
	if( demoCircle < Settings.circles)
	{
		float oldPen = PenSize();
		SetPenSize( Settings.lwidth);
		BRegion region = BRegion(BRect(demoRect.left+1, demoRect.top+1, demoRect.right-1, demoRect.bottom-1));
		ConstrainClippingRegion(&region);
		
		SetHighColor( demoColor);
		StrokeEllipse( demoPoint, demoRadius, demoRadius);
		demoRadius += ( Settings.lwidth / 2) + Settings.lspace;
		demoColor.alpha += demoAlphaMod;
		demoCircle++;

		ConstrainClippingRegion( NULL);
		SetPenSize( oldPen);
	}
	else
	{
		//demo background
		SetHighColor( 255, 255, 255, 255);
		FillRoundRect( BRect( demoRect.left+1, demoRect.top+1, demoRect.right-1, demoRect.bottom-1), 2, 2);
		//SetHighColor( 0, 0, 0, 255);
		//StrokeRoundRect( demoRect, 2, 2);
	
		InitDemo();
	}

}

//---------------------------------------------------
//	set arguments to values needed for all siblings be visible
//---------------------------------------------------
void
WMMSettingsView::GetPreferredSize( float *width, float *height)
{
	*width = preferredWidth;
	*height = preferredHeight;
}

//---------------------------------------------------
//	Message handler
//---------------------------------------------------
void
WMMSettingsView::MessageReceived( BMessage *msg)
{
	// drag & drop
	if( msg->WasDropped())
	{
		rgb_color*	color;
		ssize_t		size;
		if( (msg->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void **)&color, &size) == B_OK))
			Settings.color = *color;
		return;
	}

	switch( msg->what)
	{
		case WMM_MSG_SET_RADIUS:
		{
			msg->FindInt32( "be:value", &Settings.radius);
			Invalidate( demoRect);
			break;
		}
		case WMM_MSG_SET_CIRCLES:
		{
			msg->FindInt32( "be:value", &Settings.circles);
			Invalidate( demoRect);
			break;
		}
		case WMM_MSG_SET_LWIDTH:
		{
			msg->FindInt32( "be:value", &Settings.lwidth);
			Invalidate( demoRect);
			break;
		}
		case WMM_MSG_SET_LSPACE:
		{
			msg->FindInt32( "be:value", &Settings.lspace);
			Invalidate( demoRect);
			break;
		}
		case WMM_MSG_SET_PULSE:
		{
			msg->FindInt32( "be:value", &Settings.pulse);
			Settings.pulse = Settings.pulse * 100000;
			Window()->SetPulseRate( Settings.pulse);
			Invalidate( demoRect);
			break;
		}
		default:
			BView::MessageReceived( msg);
	}
}

//---------------------------------------------------
//	Animate demo circles
//---------------------------------------------------
void
WMMSettingsView::Pulse()
{
	DrawDemo();
}

//---------------------------------------------------
//	Resize view so all siblings will be visible
//---------------------------------------------------
void
WMMSettingsView::ResizeToPreferred()
{
	ResizeTo( preferredWidth, preferredHeight);
}

//---------------------------------------------------
//	Restart demo
//---------------------------------------------------
void
WMMSettingsView::InitDemo()
{
	demoCircle = 0;
	demoRadius = Settings.radius;
	demoColor = Settings.color;
	demoColor.alpha = WMM_ALPHA_MIN;
}
