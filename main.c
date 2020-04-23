#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"matrix.h"
#include"transformation.h"
int main()
{
	struct COMMONTIME ctime1 = { 2020,4,23,22,10,37.55};
	struct MJDTIME mjd = { 0,0 };
	struct COMMONTIME ctime2 = { 0,0,0,0,0,0 };
	struct GPSTIME gpst = { 0,0 };
	//COMMON2MJD(&ctime1, &mjd);
	//MJD2GPST(&mjd, &gpst);
	//MJD2COMMON(&mjd, &ctime2);
	COMMON2GPST(&ctime1, &gpst);
	GPST2MJD(&gpst, &mjd);
	GPST2COMMON(&gpst, &ctime2);
	printf("%d\t%f\n", mjd.Days,mjd.FracDay);
	printf("%d-%d-%d,%d:%d:%f\n", ctime2.Year, ctime2.Month, ctime2.Day, ctime2.Hour, ctime2.Minute, ctime2.Second);
	printf("%d\t%f\n", gpst.Week, gpst.SecOfWeek);
	system("pause");
}