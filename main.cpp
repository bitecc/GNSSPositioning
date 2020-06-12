#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"matrix.h"
#include"transformation.h"
#include"decode.h"
#include"spp.h"


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
	memset(&raw.Eph, 0, NSAT * sizeof(eph_t));
	memset(&raw.ionutc, 0, sizeof(IONUTC));
	memset(&raw.MyPos, 0, sizeof(PosResult));
	raw.MyPos.blh.B = 30;
	raw.MyPos.blh.L = 114;
	raw.MyPos.blh.H = 40;
	FILE* fp;
	//201502071
	//Nov20160903_1030
	if ((fp = fopen("201502071.bin", "rb")) == NULL)
	{
		printf("The file was not opened.\n");
		return 0;
	}
	while (!feof(fp))
	{
		if (DecodeOemstarDatFromBinFile(fp, &raw) == 1)
		{
			spp(&raw.Epoch, &raw.Eph, &raw.ionutc, &raw.MyPos);
			spv(&raw.Epoch, &raw.Eph, &raw.ionutc, &raw.MyPos);
			printf("%f\tB=%f L=%f h=%f vx=%f vy=%f vz=%f\n", raw.MyPos.Time.SecOfWeek,raw.MyPos.blh.B, raw.MyPos.blh.L, raw.MyPos.blh.H,raw.MyPos.vx,raw.MyPos.vy,raw.MyPos.vz);
		}
	}

	system("pause");
	return 0;
}