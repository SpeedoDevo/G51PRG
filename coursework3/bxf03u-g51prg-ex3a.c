/********************************************************************************************************
*                                                                                                       *
*important information:                                                                                 *
*   this program has two modes:                                                                         *
*       ¤if you compile it simply it will work just fine                                                *
*       ¤but if you compile it with -lm -lreadline -DLIBRARIES it will                                  *
*           ×optimize: determine the size of the converted number via logarithm                         *
*           ×be more userfriendly: in the extra part you can use the arrow keys to get previous input   *
*                                                                                                       *
********************************************************************************************************/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h> //compile with -lm -lreadline for awesomeness


double nlog(int base, int x);
void toBase(int base, int x);

int main(int argc, char *argv[])
{
	int userint;
	printf("The integer you want to convert: ");
	scanf("%i", &userint);
	if(userint == 0)
		return 0;
	toBase(2, userint);
	toBase(8, userint);
	toBase(16, userint);



	//extra part	
	printf("This program can do a lot more\nWanna see? y/n ");
	char answer;
	int valid=0;
	while(valid!=1) //I only accept yes or no
	{	if((answer=getchar())=='y')
			{
				valid=1;
			}
			else if(answer=='n')
			{
				valid=1;
				printf(":(\n");
			}
	}
	if(answer=='y')
	{
		printf("0 to exit\nThis program can convert any positive integer to any base.\n");
		printf("There is no theoretical limit, however after base 36 it runs out of letters to represent numbers making the output garbage, but it actually works.\n");
		printf("Try base 26. It's an only letter base.\n");

		#ifdef LIBRARIES 
		printf("Note: you can get the previous input with the arrow keys\n");
		#endif

		for(;;)
		{
			#ifdef LIBRARIES

			char *input;
			int base;
			input = readline("The integer you want to convert and the base you want to convert it to (format: int!base): ");
			sscanf(input, "%i!%i", &userint, &base);
			if(userint==0)
				return 0;
			add_history(input);
			toBase(base, userint);
			free(input);

			#else

			int base;
			printf("The integer you want to convert and the base you want to convert it to (format: int!base): ");
			scanf("%i!%i", &userint, &base);
			if(userint == 0)
				return 0;
			toBase(base, userint);

			#endif
		}
		printf("%i\n", userint);
	}
}

#ifdef LIBRARIES

void toBase(int base, int x)
{
	int size = (log(x)/log(base)) + 1;
	char cvtd[size];
	int i;
	for(i=1;i<=size;i++)
	{
		cvtd[size-i] = (char)(48+(x % base));
		x /= base;
	}
	printf("Your integer in base %i: ", base);
	if(base>=10)
	{
		for(i=0;i<=size;i++)
		{
			if(base==26)
			{
				cvtd[i]=(char)(17+(cvtd[i]));
			}
			else if(cvtd[i]>=58)
			{
				cvtd[i]=(char)(7+(cvtd[i]));
			}
		}
	}
	for (i = 0; i<size; i++)
	{
		printf("%c", cvtd[i]);
	}
	printf("\n");
}

#else

void toBase(int base, int x)
{
	char cvtd[50];
	int i;
	int size = 0;
	do{
		cvtd[size] = (char)(48+(x % base));
		x /= base;
		size++;
	}while(x!=0);
	printf("Your integer in base %i: ", base);
	if(base>=10)
	{
		for(i=0;i<=size;i++)
		{
			if(base==26)
			{
				cvtd[i]=(char)(17+(cvtd[i]));
			}
			else if(cvtd[i]>=58)
			{
				cvtd[i]=(char)(7+(cvtd[i]));
			}
		}
	}
	for (i = size; i!=0; i--)
	{
		printf("%c", cvtd[i-1]);
	}
	printf("\n");
}

#endif