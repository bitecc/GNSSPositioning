#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"matrix.h"
#include"transformation.h"
int main()
{
	struct COMMONTIME ctime1 = { 2020,4,23,20,10,37.55784378329};
	struct MJDTIME mjd = { 0,0 };
	struct COMMONTIME ctime2 = { 0,0,0,0,0,0 };
	struct GPSTIME gpst = { 0,0,0,0 };
	//COMMON2MJD(&ctime1, &mjd);
	//MJD2GPST(&mjd, &gpst);
	//MJD2COMMON(&mjd, &ctime2);
	COMMON2GPST(&ctime1, &gpst);
	//GPST2MJD(&gpst, &mjd);
	//MJD2COMMON(&mjd, &ctime2);
	GPST2COMMON(&gpst, &ctime2);
	/*struct BLh blh1 = {120.565712893,80.2347892489,128.681374};
	struct BLh blh2 = { 0,0,0 };
	struct XYZ xyz = { 0,0,0 };
	BLh2XYZ(&blh1, &xyz);
	XYZ2BLh(&xyz, &blh2);
	printf("%f,%f,%f\n", blh2.L, blh2.B, blh2.H);*/
	printf("%d\t%f\n", mjd.Days,mjd.FracDay);
	printf("%d-%d-%d,%d:%d:%.12f\n", ctime2.Year, ctime2.Month, ctime2.Day, ctime2.Hour, ctime2.Minute, ctime2.Second);
	printf("%d\t%f\n", gpst.Week, gpst.SecOfWeek);
	system("pause");
}