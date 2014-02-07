#include <stdio.h>

void simpleInterest(double principal, double rate, int time)
{
  printf("The interest earned on an account of £%0g at %0g%% over %i years is £%0g\n", principal, rate, time, principal * (rate / 100) * time);
}

int main(int argc, char *argv[])
{
  simpleInterest(100, 1, 5);
  simpleInterest(2000, 5, 10);
  simpleInterest(1, 0.5, 50);
  simpleInterest(0.5, 6, 7);
}


