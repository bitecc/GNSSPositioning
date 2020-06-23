#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"self_defination.h"

int main()
{
	raw_t raw;
	memset(&raw.Eph, 0, NSAT * sizeof(eph_t));
	memset(&raw.ionutc, 0, sizeof(IONUTC));
	memset(&raw.MyPos, 0, sizeof(PosResult));
	raw.MyPos.blh.B = 30;
	raw.MyPos.blh.L = 114;
	raw.MyPos.blh.H = 40;
	FILE* fp;
	FILE* flog1;
	FILE* flog2;
	//201502071
	//Nov20160903_1030
	if ((fp = fopen("201502071.bin", "rb")) == NULL)
	{
		printf("The file was not opened.\n");
		return 0;
	}
	if ((flog1 = fopen("MyPos.pos", "w+")) == NULL)
	{
		printf("The file was not opened.\n");
		return 0;
	}
	if ((flog2 = fopen("RefPos.pos", "w+")) == NULL)
	{
		printf("The file was not opened.\n");
		return 0;
	}
	fprintf(flog1, "Week,SecOfWeek,X,Y,Z,B,L,H,VX,VY,VZ,sigma0,vsigma0\n");
	fprintf(flog2, "Week,SecOfWeek,B,L,H\n");
	while (!feof(fp))
	{
		if (DecodeOemstarDatFromBinFile(fp, &raw) == 3)
		{
			fprintf(flog2, "%d,%f,%f,%f,%f\n", raw.Pos.Time.Week, raw.Pos.Time.SecOfWeek, raw.Pos.lat, raw.Pos.lon, raw.Pos.hgt);
		}
		if (DecodeOemstarDatFromBinFile(fp, &raw) == 1)
		{
			spp(&raw.Epoch, &raw.Eph, &raw.ionutc, &raw.MyPos);
			printf("%f\tX=%f Y=%f Z=%f vx=%f vy=%f vz=%f\n", raw.MyPos.Time.SecOfWeek,raw.MyPos.blh.B,raw.MyPos.blh.L,raw.MyPos.blh.H,raw.MyPos.vx, raw.MyPos.vy, raw.MyPos.vz);
			fprintf(flog1, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", raw.MyPos.Time.Week, raw.MyPos.Time.SecOfWeek, raw.MyPos.x, raw.MyPos.y, raw.MyPos.z,
				raw.MyPos.blh.B, raw.MyPos.blh.L, raw.MyPos.blh.H, raw.MyPos.vx, raw.MyPos.vy, raw.MyPos.vz, raw.MyPos.sigma0, raw.MyPos.vsigma0);
		}
	}
	fclose(fp);
	fclose(flog1);
	fclose(flog2);
	system("pause");
	return 0;
}