#include <stdio.h>
#include <sys/ioctl.h>

int main()
{
	int width, height;
	int i=0, j=0;
	int valid=0;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	do{
			printf("Please enter the width and height for your rectangle (separated by a comma, e.g. 3,4): ");
			scanf("%i,%i", &width, &height);
			if(height <= (w.ws_row-1) && width <= (w.ws_col/2))
				valid = 1;
			else
				printf("Rectangle too big (maxwidth: %i, maxheight: %i)\n", (w.ws_col/2), (w.ws_row-1)); 
	}while(valid == 0);
	system("clear");
	for(i;height > i;i++)
		{
			for(j;width > j;j++)
				{
					printf("* ");
				}
			printf("\n");
			j = 0;
		}
}
