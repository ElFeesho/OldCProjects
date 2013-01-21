/*
 * BAYER2RGB24 ROUTINE TAKEN FROM:
 *
 * Sonix SN9C101 based webcam basic I/F routines
 * Copyright (C) 2004 Takafumi Mizuno <taka-qce@ls-a.jp>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

void bayer2rgb24(unsigned char *dst, unsigned char *src, long int width, long int height)
{
    long int i;
    unsigned char *rawpt, *scanpt;
    long int size;

    rawpt = src;
    scanpt = dst;
    size = width*height;

    for ( i = 0; i < size; i++ ) {
	if ( (i/width) % 2 == 0 ) {
	    if ( (i % 2) == 0 ) {
		/* B */
		if ( (i > width) && ((i % width) > 0) ) {
		    *scanpt++ = (*(rawpt-width-1)+*(rawpt-width+1)+
				 *(rawpt+width-1)+*(rawpt+width+1))/4;	/* R */
		    *scanpt++ = (*(rawpt-1)+*(rawpt+1)+
				 *(rawpt+width)+*(rawpt-width))/4;	/* G */
		    *scanpt++ = *rawpt;					/* B */
		} else {
		    /* first line or left column */
		    *scanpt++ = *(rawpt+width+1);		/* R */
		    *scanpt++ = (*(rawpt+1)+*(rawpt+width))/2;	/* G */
		    *scanpt++ = *rawpt;				/* B */
		}
	    } else {
		/* (B)G */
		if ( (i > width) && ((i % width) < (width-1)) ) {
		    *scanpt++ = (*(rawpt+width)+*(rawpt-width))/2;	/* R */
		    *scanpt++ = *rawpt;					/* G */
		    *scanpt++ = (*(rawpt-1)+*(rawpt+1))/2;		/* B */
		} else {
		    /* first line or right column */
		    *scanpt++ = *(rawpt+width);	/* R */
		    *scanpt++ = *rawpt;		/* G */
		    *scanpt++ = *(rawpt-1);	/* B */
		}
	    }
	} else {
	    if ( (i % 2) == 0 ) {
		/* G(R) */
		if ( (i < (width*(height-1))) && ((i % width) > 0) ) {
		    *scanpt++ = (*(rawpt-1)+*(rawpt+1))/2;		/* R */
		    *scanpt++ = *rawpt;					/* G */
		    *scanpt++ = (*(rawpt+width)+*(rawpt-width))/2;	/* B */
		} else {
		    /* bottom line or left column */
		    *scanpt++ = *(rawpt+1);		/* R */
		    *scanpt++ = *rawpt;			/* G */
		    *scanpt++ = *(rawpt-width);		/* B */
		}
	    } else {
		/* R */
		if ( i < (width*(height-1)) && ((i % width) < (width-1)) ) {
		    *scanpt++ = *rawpt;					/* R */
		    *scanpt++ = (*(rawpt-1)+*(rawpt+1)+
				 *(rawpt-width)+*(rawpt+width))/4;	/* G */
		    *scanpt++ = (*(rawpt-width-1)+*(rawpt-width+1)+
				 *(rawpt+width-1)+*(rawpt+width+1))/4;	/* B */
		} else {
		    /* bottom line or right column */
		    *scanpt++ = *rawpt;				/* R */
		    *scanpt++ = (*(rawpt-1)+*(rawpt-width))/2;	/* G */
		    *scanpt++ = *(rawpt-width-1);		/* B */
		}
	    }
	}
	rawpt++;
    }

}

