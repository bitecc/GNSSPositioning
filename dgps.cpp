#include"self_defination.h"

int OEMProcess(unsigned char* Buff, int lenR, raw_t* raw,FILE* p)
{
	int sIndex = 0;
	int rtn;
	while (1)
	{
		rtn = DecodeOEMData(Buff + sIndex, lenR - sIndex, &sIndex, raw);
		//printf("%d\n", rtn);
		switch (rtn)
		{
		case 1:
		{
			//解算接收机位置、速度及钟差等
			//calRecPV(epoDat, pntRst, satsol);
			spp(&raw->Epoch, &raw->Eph, &raw->ionutc, &raw->MyPos,&raw->rtcm);
			printf("%f\t%f\t%f\t%f\t%.4f\t%.4f\t%.4f\n", raw->MyPos.Time.SecOfWeek, raw->MyPos.x, raw->MyPos.y, raw->MyPos.z,raw->MyPos.vx,raw->MyPos.vy,raw->MyPos.vz);
			fprintf(p, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", raw->MyPos.Time.Week, raw->MyPos.Time.SecOfWeek, raw->MyPos.x, raw->MyPos.y, raw->MyPos.z,
				raw->MyPos.blh.B, raw->MyPos.blh.L, raw->MyPos.blh.H, raw->MyPos.vx, raw->MyPos.vy, raw->MyPos.vz);
			break;
		}
		default:
			break;
		}
		if (rtn == 0)
		{
			//将剩余解拷贝至Buff开头
			memcpy(Buff, Buff + sIndex, lenR - sIndex);
			break;
		}
	}
	return lenR - sIndex;
}

int RTCMProcess(unsigned char* Buff, int lenR, RTCM* rtcm)
{
	int sIndex = 0;
	int rtn;
	while (1)
	{
		rtn = DecodeRTCMData(Buff + sIndex, lenR - sIndex, &sIndex, rtcm);
		if (rtn == 0)
		{
			memcpy(Buff, Buff + sIndex, lenR - sIndex);
			break;
		}
	}
	return lenR - sIndex;
}