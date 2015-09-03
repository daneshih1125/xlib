#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <X11/Xlib.h>
#include <png.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/*int read_png(const char *path, unsigned char **data,
		int *width, int *height)
{
	int i;
	png_structp png = NULL;
	png_infop info = NULL;
	png_bytep *row_pointers;
	FILE *fp = fopen(path, "rb");

	if (!fp)
		return -1;

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);

	png_init_io(png, fp);
	png_read_info(png, info);

	*width = png_get_image_width(png, info);
	*height = png_get_image_height(png, info);

	*data = (unsigned char *)row_pointers;
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * (*height));

	for (i = 0; i < *height; i++)
                row_pointers[i] = (png_byte*) malloc(png_get_rowbytes(png, info));

	png_read_image(png, row_pointers);

	fclose(fp);

	return png_get_rowbytes(png, info);
}
*/

int get_png_files(const char *path, char ***png_list)
{
	const unsigned char PNG_MAGIC[] = { 0x89, 0x50, 0x4e, 0x47,
					    0x0d, 0x0a, 0x1a, 0x0a };
	unsigned char header[8];
	int nbytes, npngs = 0;
	FILE *fp;
	DIR *dp;
	struct dirent *entry;
	char **png_array;

	dp = opendir(path);
	png_array = (char **) malloc(sizeof(char *) * 10);

	while ((entry = readdir(dp)) != NULL) {
		if ((fp = fopen(entry->d_name, "rb")) == NULL)
			fprintf(stderr, "open %s fail\n", entry->d_name);

		memset(header, 0, sizeof(header));
		nbytes = fread(header, sizeof(unsigned char), sizeof(header), fp);
		if (nbytes == sizeof(header) &&
			!memcmp(header, PNG_MAGIC, sizeof(header))) {
			png_array[npngs] = (char *) malloc(strlen(entry->d_name) + 1);
			strcpy(png_array[npngs], entry->d_name);
			npngs++;
		}
		fclose(fp);

		if (npngs > 0 && npngs % 10 == 0)
			png_array = (char **) realloc(png_array,
					sizeof(char *) * 10 * (npngs / 10 + 1));
	}

	closedir(dp);
	png_array = (char **) realloc(png_array, sizeof(char *) * npngs);
	*png_list = png_array;

	return npngs;
}

static void TeardownPng (png_structp png, png_infop info)
{

	if (png) {

		png_infop *realInfo = (info? &info: NULL);

		png_destroy_read_struct (&png, realInfo, NULL);

	}

}

/*
 * reference http://codex.wiki/question/1446213-9689
 * The png file Displays with incorrect colors on X window
 */
void LoadPng (FILE *file, unsigned char** data, char **clipData, unsigned int *width, unsigned int *height, unsigned int *rowbytes)
{

	size_t size = 0,  clipSize = 0;

	png_structp png = NULL;
	png_infop info = NULL;
	unsigned char **rowPointers = NULL;

	int depth = 0,
	colortype = 0,
	interlace = 0,
	compression = 0,
	filter = 0;
	unsigned clipRowbytes = 0;


	png = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct (png);
	png_init_io (png, file);
	png_read_info (png, info);
	png_get_IHDR (png, info, (png_uint_32*)width, (png_uint_32*)height, &depth, &colortype, &interlace, &compression, &filter);

	*rowbytes = png_get_rowbytes (png, info);

	if (colortype == PNG_COLOR_TYPE_RGB) {
		// X hates 24bit images - pad to RGBA
		png_set_filler (png, 0xff, PNG_FILLER_AFTER);
		*rowbytes = (*rowbytes * 4) / 3;
	}

	png_set_bgr (png);
	*width = png_get_image_width (png, info);
	*height = png_get_image_height (png, info);
	size = *height * *rowbytes;

	clipRowbytes = *rowbytes/32;
	if (*rowbytes % 32)
		++clipRowbytes;
	clipSize = clipRowbytes * *height;
	// This gets freed by XDestroyImage
	*data = (unsigned char*) malloc (sizeof (png_byte) * size);

	rowPointers = (unsigned char**) malloc (*height * sizeof (unsigned char*));

	png_bytep cursor = *data;

	int i=0,x=0,y=0;

	for (i=0; i<*height; ++i, cursor += *rowbytes)
		rowPointers[i] = cursor;

	png_read_image (png, rowPointers);
	*clipData = (char*) calloc (clipSize, sizeof(unsigned char));

	if (colortype == PNG_COLOR_TYPE_RGB) {
		memset (*clipData, 0xff, clipSize);
	} else {
		// Set up bitmask for clipping fully transparent areas
		for (y=0; y<*height; ++y, cursor+=*rowbytes) {

			for (x=0; x<*rowbytes; x+=4) {
				// Set bit in mask when alpha channel is nonzero
				if(rowPointers[y][x+3])
					(*clipData)[(y*clipRowbytes) + (x/32)] |= (1 << ((x/4)%8));
			}
		}
	}
	TeardownPng (png, info);
	free (rowPointers);
}

int main(int argc, char **argv)
{
	Display *dpy;
	Window win;
	int screen;
	int stop = 0, width, height, png_bytes, i, n;
	char text[256] = "";
	GC 	gc;
	XEvent 	event;
	KeySym 	key;
	unsigned char *data;
	char **png_files;

	dpy = XOpenDisplay(NULL);
	screen = DefaultScreen(dpy);
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen),
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		5, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | ButtonPress);
	XMapWindow(dpy, win);

	n = get_png_files(".", &png_files);
#if 0
	png_bytes = read_png("mushroom.png", &data, &width, &height);

	if (png_bytes < 0) {
		fprintf(stderr, "read png fail\n");
		exit(EXIT_FAILURE);
	}
#else

	unsigned long valuemask = 0;
	XGCValues values;
	char *clip = NULL;
	FILE *fp;
	XImage *ximage;



	gc = XCreateGC(dpy, win, valuemask, &values);
	XSetForeground(dpy, gc, BlackPixel(dpy, screen));
	XSetBackground(dpy, gc, WhitePixel(dpy, screen));

	i = 0;
	while (!stop) {
		XNextEvent(dpy, &event);
		XClearArea(dpy, win, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
		fp = fopen(png_files[i], "rb");
		LoadPng(fp, &data, &clip, &width, &height, &png_bytes);
		ximage = XCreateImage (dpy, DefaultVisual(dpy, screen),
				DefaultDepth(dpy, screen), ZPixmap, 0, (char*)data, width, height, 32, png_bytes);
		if (event.type == KeyPress &&
				XLookupString(&event.xkey, text, sizeof(text), &key, 0) == 1) {
			if (text[0] == 'q')
				stop = 1;
		}
		XPutImage(dpy, win, gc, ximage, 0, 0, 0, 0, width, height);
		fclose(fp);
		i++;
		if (i == n)
			i = 0;
		XFree(ximage);
	}
#endif

	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	return 0;
}
