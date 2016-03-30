#include "hal.h"
#include "device/video.h"
#include "device/palette.h"

#include <string.h>
#include <stdlib.h>

int get_fps();

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(dst && src);

	/* TODO: Performs a fast blit from the source surface to the 
	 * destination surface. Only the position is used in the
	 * ``dstrect'' (the width and height are ignored). If either
	 * ``srcrect'' or ``dstrect'' are NULL, the entire surface 
	 * (``src'' or ``dst'') is copied. The final blit rectangle 
	 * is saved in ``dstrect'' after all clipping is performed
	 * (``srcrect'' is not modified).
	 */
	SDL_Rect rect;
	if(dstrect==0)
	{
		dstrect=&rect;
		dstrect->x=dstrect->y=0;
		dstrect->w=dst->w;
		dstrect->h=dst->h;
	}
	if(srcrect==0)
	{
		srcrect=&rect;
		srcrect->x=srcrect->y=0;
		srcrect->w=src->w;
		srcrect->h=src->h;
	}
	/*if(dstrect==0||srcrect==0||(dstrect->x==0&&dstrect->y==0&&srcrect->h==SCR_HEIGHT&&srcrect->w==SCR_WIDTH))
	{
		memcpy(dst->pixels,src->pixels,SCR_SIZE);
		return;
	}*/
	//Log("B:%x,%x-%x,%x",srcrect->x,srcrect->y,srcrect->x+srcrect->h,srcrect->x+srcrect->w);
	int y=srcrect->h;
	while(y--)
	{
		int dy=y+dstrect->y,sy=y+srcrect->y;
		memcpy(&dst->pixels[dy*dst->pitch+dstrect->x],
			   &src->pixels[sy*src->pitch+srcrect->x],srcrect->w);
		//asm volatile ("cld; rep movsl" : : "c"(srcrect->w / 4), "S"(&src->pixels[(sx << 8) + (sx << 6)+srcrect->y]), "D"(&dst->pixels[(dx << 8) + (dx << 6)+dstrect->y]));
	}
	return;
	/*int y;
	for(x=0;x<srcrect->h;++x)
	{
		for(y=0;y<srcrect->w;++y)
		{
			int dx=x+dstrect->x,sx=x+srcrect->x;
			int dy=y+dstrect->y,sy=y+srcrect->y;
			dst->pixels[(dx << 8) + (dx << 6) + dy]=
				src->pixels[(sx << 8) + (sx << 6) + sy];
		}
	}
	return;*/
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
	assert(dst);
	assert(color <= 0xff);
	//Log("Fill:*%d,%d +%d,%d\n",dstrect->x,dstrect->y,dstrect->h,dstrect->w);
	/* TODO: Fill the rectangle area described by ``dstrect''
	 * in surface ``dst'' with color ``color''. If dstrect is
	 * NULL, fill the whole surface.
	 */
	SDL_Rect rect;
	if(dstrect==0)
	{
		dstrect=&rect;
		dstrect->x=dstrect->y=0;
		dstrect->w=dst->w;
		dstrect->h=dst->h;
	}
	int y=dstrect->h;
	while(y--)
	{
		int dy=y+dstrect->y;
		memset(&dst->pixels[dy*dst->pitch+dstrect->x],
			   color,dstrect->w);
		//asm volatile ("cld; rep movsl" : : "c"(srcrect->w / 4), "S"(&src->pixels[(sx << 8) + (sx << 6)+srcrect->y]), "D"(&dst->pixels[(dx << 8) + (dx << 6)+dstrect->y]));
	}
	return;
}

void SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h) {
	assert(screen);
	assert(screen->pitch == 320);
	if(screen->flags & SDL_HWSURFACE) {
		if(x == 0 && y == 0) {
			/* Draw FPS */
			vmem = VMEM_ADDR;
			char buf[80];
			sprintf(buf, "%dFPS", get_fps());
			draw_string(buf, 0, 0, 10);
		}
		return;
	}

	/* TODO: Copy the pixels in the rectangle area to the screen. */
	Log("Show:*%d,%d +%d,%d\n",x,y,h,w);
	while(h--)
	{
		int dy=y+h;
		memcpy(&screen->pixels[(dy << 8) + (dy << 6) + x],
			   VMEM_ADDR+(dy << 8) + (dy << 6) + x, w);
		//asm volatile ("cld; rep movsl" : : "c"(srcrect->w / 4), "S"(&src->pixels[(sx << 8) + (sx << 6)+srcrect->y]), "D"(&dst->pixels[(dx << 8) + (dx << 6)+dstrect->y]));
	}
	//asm volatile ("cld; rep movsl" : : "c"(SCR_SIZE / 4), "S"(vmem), "D"(VMEM_ADDR));

}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, 
		int firstcolor, int ncolors) {
	assert(s);
	assert(s->format);
	assert(s->format->palette);
	assert(firstcolor == 0);

	if(s->format->palette->colors == NULL || s->format->palette->ncolors != ncolors) {
		if(s->format->palette->ncolors != ncolors && s->format->palette->colors != NULL) {
			/* If the size of the new palette is different 
			 * from the old one, free the old one.
			 */
			free(s->format->palette->colors);
		}

		/* Get new memory space to store the new palette. */
		s->format->palette->colors = malloc(sizeof(SDL_Color) * ncolors);
		assert(s->format->palette->colors);
	}

	/* Set the new palette. */
	s->format->palette->ncolors = ncolors;
	memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

	if(s->flags & SDL_HWSURFACE) {
		/* TODO: Set the VGA palette by calling write_palette(). */
		write_palette(s->format->palette->colors,ncolors);
		//assert(0);
	}
}

/* ======== The following functions are already implemented. ======== */

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *scrrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(src && dst);
	int x = (scrrect == NULL ? 0 : scrrect->x);
	int y = (scrrect == NULL ? 0 : scrrect->y);
	int w = (scrrect == NULL ? src->w : scrrect->w);
	int h = (scrrect == NULL ? src->h : scrrect->h);

	assert(dstrect);
	if(w == dstrect->w && h == dstrect->h) {
		/* The source rectangle and the destination rectangle
		 * are of the same size. If that is the case, there
		 * is no need to stretch, just copy. */
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_BlitSurface(src, &rect, dst, dstrect);
	}
	else {
		/* No other case occurs in NEMU-PAL. */
		assert(0);
	}
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
		uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
	SDL_Surface *s = malloc(sizeof(SDL_Surface));
	assert(s);
	s->format = malloc(sizeof(SDL_PixelFormat));
	assert(s);
	s->format->palette = malloc(sizeof(SDL_Palette));
	assert(s->format->palette);
	s->format->palette->colors = NULL;

	s->format->BitsPerPixel = depth;

	s->flags = flags;
	s->w = width;
	s->h = height;
	s->pitch = (width * depth) >> 3;
	s->pixels = (flags & SDL_HWSURFACE ? (void *)VMEM_ADDR : malloc(s->pitch * height));
	assert(s->pixels);

	return s;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
	return SDL_CreateRGBSurface(flags,  width, height, bpp,
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void SDL_FreeSurface(SDL_Surface *s) {
	if(s != NULL) {
		if(s->format != NULL) {
			if(s->format->palette != NULL) {
				if(s->format->palette->colors != NULL) {
					free(s->format->palette->colors);
				}
				free(s->format->palette);
			}

			free(s->format);
		}
		
		if(s->pixels != NULL) {
			free(s->pixels);
		}

		free(s);
	}
}

