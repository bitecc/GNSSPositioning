#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"matrix.h"
#include"transformation.h"
#include"decode.h"
int main()
{
	struct COMMONTIME ctime1 = { 2020,4,23,20,10,37.55784378329};
	struct MJDTIME mjd = { 0,0 };
	struct COMMONTIME ctime2 = { 0,0,0,0,0,0 };
	struct GPSTIME gpst = { 0,0,0,0 };
	//COMMON2MJD(&ctime1, &mjd);
	//MJD2GPST(&mjd, &gpst);
	//MJD2COMMON(&mjd, &ctime2);
	//COMMON2GPST(&ctime1, &gpst);
	//GPST2MJD(&gpst, &mjd);
	//MJD2COMMON(&mjd, &ctime2);
	//GPST2COMMON(&gpst, &ctime2);
	//struct BLh blh1 = {120.565712893,80.2347892489,128.681374};
	//struct BLh blh2 = { 0,0,0 };
	//struct XYZ xyz = { 0,0,0 };
	//int s=BLh2XYZ(&blh1, &xyz);
	//int t = XYZ2BLh(&xyz, &blh2);
	//printf("%f,%f,%f\n", blh2.L, blh2.B, blh2.H);
	//printf("%d\t%f\n", mjd.Days,mjd.FracDay);
	//printf("%d-%d-%d,%d:%d:%.12f\n", ctime2.Year, ctime2.Month, ctime2.Day, ctime2.Hour, ctime2.Minute, ctime2.Second);
	//printf("%d\t%f\n", gpst.Week, gpst.SecOfWeek);

	raw_t raw;
	FILE* fp;
	if ((fp = fopen("20150207-1.bin", "rb")) == NULL)
	{
		printf("The file was not opened.\n");
		return 0;
	}
	while (!feof(fp))
	{
		if (DecodeOemstarDatFromBinFile(fp, &raw) == 1)
		{
			//printf("%4d %10.1f\n", raw.Epoch.Time.Week, raw.Epoch.Time.SecOfWeek);
		}
	}

	system("pause");
	return 0;
}