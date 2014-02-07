#include <stdio.h>

int pointSum(int pica1, int pica2, int point1, int point2)
{
  return point1 + point2 + ((pica1+pica2)*12);
}

void printer(int pica1, int pica2, int point1, int point2)
{
  int pointS = pointSum(pica1, pica2, point1, point2);
  int picaS = pointS/12;
  printf("Adding %ip%i and %ip%i together is %ip%i\n", pica1, point1, pica2, point2, picaS, (pointS-(picaS*12)));
}

int main(int argc, char argv[])
{
  printer(0, 6, 3, 11);
  printer(13, 1, 11, 0);
  printer(0, 6, 0, 0);
  printer(6, 6, 0, 0);
  printer(8, 9, 1, 3);
}
