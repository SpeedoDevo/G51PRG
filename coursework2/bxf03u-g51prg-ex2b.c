#include <stdio.h>
#include <sys/ioctl.h>

int fractalCheck(double x, double y)
{
	int it;
	double r, i;
	double r2, i2;
	double tmp;
	it = 4096;
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


int main()
{
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	int width = w.ws_col;
	int height = w.ws_row-1;
	int i=0, j=0;
	double passx, passy;
	system("clear");
	int it;
	for(i; height > i; i++)
		{
			for(j; width > j;j++)
			{
				passx = (((j+0.0)/width)*2.0)-1.5;
				passy = (((i+0.0)/height)*2.0)-1.0;
				it = fractalCheck(passx,passy);
				if(it == 0)
					printf(" ");
				else
					printf("*");
			}
			printf("\n");
			j=0;
		}
}
			
