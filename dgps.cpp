#include"self_defination.h"
/***************************
OEMProcess

功能：处理观测值文件的端口的数据，定位解算

输入参数：Buff		数据缓冲区
		  lenR		数据缓冲区的畅爽
		  raw		存放解码数据
		  p			输出解算结果的文件指针
返回值：剩余缓冲区的长度
****************************/
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

/***************************
RTCMProcess

功能：处理RTCM的数据，输出rtcm信息

输入参数：Buff		数据缓冲区
		  lenR		数据缓冲区的畅爽
		  rtcm		存放解码数据
返回值：剩余缓冲区的长度
****************************/
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