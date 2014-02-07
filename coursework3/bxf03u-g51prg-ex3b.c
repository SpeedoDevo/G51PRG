#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h> //to get window size
#include <stdlib.h> // to open image after generating via system() // it's not a good practice but pretty handy

int fractalCheck(double x, double y);

void loadBar(int x, int n, int r, int w);

void HSL2RGB(double h, double sl, double l, unsigned char *pr, unsigned char *pg, unsigned char *pb);

int main(int argc, char *argv[])
{
	//declarations, inits
	int width, height;
	char imagefile[50];
	FILE *image;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int wwidth = w.ws_col; //getting window width for load bar
	double passx, passy;
	int it;
	int i=0, j;
	double maxx=-0.743627135, maxy=0.131837963, minx=-0.743659135, miny=0.131813963;



	// image size setting
	if(argc!=4)
	{
		printf("You can change the size of the image if you enter %s <width> <height>.\n", imagefile);
		width = 1000, height = 600;
	}
	else //set image size based on input
	{

		width = strtol(argv[2], NULL, 10);
		height = strtol(argv[3], NULL, 10);
	}



	//to don't get segfault if there is no image file specified
	if(argc==1)
	{
		image = fopen("fractal.tga", "w+");
		strcpy(imagefile, "fractal.tga");
	}
	else
	{
		image = fopen(argv[1], "w+");
		strcpy(imagefile, argv[1]);
	}



	//some userfriendlyness
	printf("Generating your image (%s)... please wait\n", imagefile);



	//header is static, apart from width and height :(
	char header[14] = {0,0,2,0,0,0,0,0,0,0,0,0,24,0};
	short size[2] = {width,height};
	fwrite(header, 1, 12, image); //first part of header
	fwrite(size, 2, 2, image); //image size
	fwrite(&header[12], 1, 2, image); //rest of header
	//printf("%i\n", sizeof(header));


	
	//some more declaration, inits
	unsigned char pixels[width][height][3];
	int pixelnum = width*height; //only for load bar
	int pixelcount = 0;
	unsigned char r,g,b;



	//pixel data generator
	for(;i<height;++i)
	{
		for(j=0;j<width;++j)
		{
			passx = ((((double) j/(double) width)*(maxx-minx))+minx);
			passy = -1*((((double) i/(double) height)*(maxy-miny))+miny);
			it = fractalCheck(passx, passy); //where the magic is happenning
			if(it==0)
			{
				pixels[j][i][0] = 255;
				pixels[j][i][1] = 255;
				pixels[j][i][2] = 255;
			}
			else
			{
				HSL2RGB(((double) it)/6200, ((double) it)/4100, 0.6, &r, &g, &b); //magic #2
				pixels[j][i][0] = b; //this is changed up for a reason not mistake
				pixels[j][i][1] = g;
				pixels[j][i][2] = r;
			}
			loadBar(pixelcount+1, pixelnum, 1000, wwidth-10); //more userfriendlyness
			pixelcount++;
			fwrite(pixels[j][i],1,3,image);

		}
	}




	printf("Closing file...\n");
	fclose(image);	



	//opening with system()
	char opencmd[50];
	int checkconvert = system("convert > /dev/null"); //checking convert availability
	if(checkconvert==0)
	{
		printf("Convering to .png...\n");
		char cnvtcmd[50];
		sprintf(cnvtcmd, "convert %s %s.png", imagefile, imagefile);
		system(cnvtcmd);
		sprintf(opencmd, "xview %s.png > /dev/null", imagefile);
	}
	else
	{
		sprintf(opencmd, "xview %s > /dev/null", imagefile);
	}
	printf("Opening...\n");
	system(opencmd);
}

int fractalCheck(double x, double y)
{
	int it;
	double r, i;
	double r2, i2;
	double tmp;
	it = 4095;
	r = i = 0; r2= 0; i2 =0;
	do
		{
			tmp = r2 - i2 + x;
			i = 2 * r * i + y;
			r = tmp;
			r2 = r*r; i2 = i*i;
		} while((r2 + i2) <= 4.0 && --it);
	return it;
}

void loadBar(int x, int n, int r, int w)
{
	if ( x % (n/r) != 0 ) return; 
	float ratio = x/(float)n;
	int c = ratio * w; 
	printf("%3d%% [", (int)(ratio*100) ); 
	for (x=0; x<c; x++)
	   printf("="); 
	for (x=c; x<w; x++)
	   printf(" "); 
	if(ratio == 1)
	{
		printf("]\033[E");
	}
	else
	{
		printf("]\n\033[F\033[J");
	}
}

void HSL2RGB(double h, double sl, double l, unsigned char *pr, unsigned char *pg, unsigned char *pb)
{
	double v;
	double r,g,b;

	r = l; // default to gray
	g = l;
	b = l;
	v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
	if (v > 0)
	{
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = (int)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant)
		{
				case 0:
					r = v;
					g = mid1;
					b = m;
					break;
				case 1:
					r = mid2;
					g = v;
					b = m;
					break;
				case 2:
					r = m;
					g = v;
					b = mid1;
					break;
				case 3:
					r = m;
					g = mid2;
					b = v;
					break;
				case 4:
					r = mid1;
					g = m;
					b = v;
					break;
				case 5:
					r = v;
					g = m;
					b = mid2;
					break;
		}
	}
	*pr = r*255;
	*pg = g*255;
	*pb = b*255;
}
