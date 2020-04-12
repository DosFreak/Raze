
/*
 ** imagehelpers.cpp
 ** Utilities for image conversion - mostly 8 bit paletted baggage
 **
 **---------------------------------------------------------------------------
 ** Copyright 2004-2007 Randy Heit
 ** Copyright 2006-2018 Christoph Oelckers
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions
 ** are met:
 **
 ** 1. Redistributions of source code must retain the above copyright
 **    notice, this list of conditions and the following disclaimer.
 ** 2. Redistributions in binary form must reproduce the above copyright
 **    notice, this list of conditions and the following disclaimer in the
 **    documentation and/or other materials provided with the distribution.
 ** 3. The name of the author may not be used to endorse or promote products
 **    derived from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 ** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 ** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 ** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 ** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 ** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 ** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **---------------------------------------------------------------------------
 **
 **
 */
 
 #include "imagehelpers.h"
#include "palettecontainer.h"
#include "colormatcher.h"

namespace ImageHelpers
{
	uint8_t GrayMap[256];
	int alphaThreshold;
	ColorTable256k RGB256k;

	void SetPalette(const PalEntry* colors)
	{
		// create the RGB666 lookup table
		for (int r = 0; r < 64; r++)
			for (int g = 0; g < 64; g++)
				for (int b = 0; b < 64; b++)
					RGB256k.RGB[r][g][b] = ColorMatcher.Pick((r<<2)|(r>>4), (g<<2)|(g>>4), (b<<2)|(b>>4));
	}
	
	
}