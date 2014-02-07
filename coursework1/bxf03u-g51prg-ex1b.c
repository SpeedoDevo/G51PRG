#include <stdio.h>

double cmToInch(double centi)
{
  return centi / 2.54;
}

double inchToPoint(double inch)
{
  return inch * 72;
}

int inchToPica(double inch)
{
  return inch * 6;
}

void printer(double centi)
{
  double inch;
  int pica;
  double point;
  inch = cmToInch(centi);
  point = inchToPoint(inch);
  pica = inchToPica(inch);
  printf("%0gcm is %i picas and %0g points\n", centi, pica, (point - (pica * 12)));
}

int main(int argc, char *argv[])
{
  printer(21.0);
  printer(0.42333);
  printer(2.54);
  printer(84.1);
  printer(118.9);
}
