#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"matrix.h"
int main()
{
	double A[] = { 1,0,0,0,1,0,0,0,1};
	//double B[] = { 1,1,0.5,-0.3,0,1};
	double C[9] = {0};
	MatrixInv(3, A, C);
	for (int i = 0; i < 9; i++)
		printf("%f\n", C[i]);
	system("pause");
}