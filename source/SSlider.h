/*
Copyright (c) 2002, Marcin 'Shard' Konicki
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Name "Marcin Konicki", "Shard" or any combination of them, must not be used to endorse or promote products derived from this software without specific prior written permission from Marcin Konicki.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __SSLIDER_H_
#define __SSLIDER_H_

//----------------------------------------------------------------------------
//
//	Include
//
//----------------------------------------------------------------------------

#include <Slider.h>


//----------------------------------------------------------------------------
//
//	Define
//
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//
//	Classes
//
//----------------------------------------------------------------------------

//---------------------------------------------------
//	SSlider
//---------------------------------------------------
class SSlider : public BSlider
{
	public:
							SSlider(BRect frame, const char *name, const char *label, BMessage *message, int32 minValue, int32 maxValue, orientation posture = B_HORIZONTAL, thumb_style thumbType = B_BLOCK_THUMB, uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);
							~SSlider();
		void				DrawThumb();
		void				UpdateLabel();

	private:
		char				*basicLabel;
};


//----------------------------------------------------------------------------
//
//	Functions
//
//----------------------------------------------------------------------------

#endif /*__SSLIDER_H_*/
