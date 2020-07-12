#include"self_defination.h"
#include"sockets.h"


int main()
{
	/*
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
	system("pause");*/
	//原始数据定义与初始化
	raw_t raw;
	memset(&raw.Eph, 0, NSAT * sizeof(eph_t));
	memset(&raw.ionutc, 0, sizeof(IONUTC));
	memset(&raw.MyPos, 0, sizeof(PosResult));
	memset(&raw.Epoch, 0, sizeof(EpochObs));
	memset(&raw.rtcm, 0, sizeof(RTCM));
	raw.MyPos.blh.B = 30;
	raw.MyPos.blh.L = 114;
	raw.MyPos.blh.H = 40;
	//定义两个端口
	SOCKET sockOEM; SOCKET sockRTCM;
	char IP[] = "47.114.134.129";
	unsigned short portOEM = 4000;
	unsigned short portRTCM = 5000;
	bool OEMFlag; bool RTCMFlag;
	unsigned char OEMBuff[MAXOEMLEN] = { 0 };
	unsigned char RTCMBuff[MAXRTCMLEN] = { 0 };
	int lenR_OEM, lenL_OEM, lenR_RTCM, lenL_RTCM;
	lenL_OEM = 0; lenL_RTCM = 0;
	int rtn; int sIndex;
	//文件
	FILE* pos; FILE* bindat;
	bindat = fopen("result.bin", "wb+");
	pos = fopen("result.pos", "w+");
	// 打开socket端口
	OEMFlag = OpenSocket(sockOEM, IP, portOEM);
	RTCMFlag = OpenSocket(sockRTCM, IP, portRTCM);
	// 网络通信读数据方法
	while (OEMFlag&&RTCMFlag) 
	{
		if ((lenR_OEM = recv(sockOEM, (char*)OEMBuff+lenL_OEM, MAXOEMLEN-lenL_OEM, 0)) < 3)
		{
			printf("OEM SSR Info fail!\r\n");
			CloseSocket(sockOEM);
			OEMFlag = OpenSocket(sockOEM, IP, portOEM);
		}
		else if ((lenR_RTCM = recv(sockRTCM, (char*)RTCMBuff + lenL_RTCM, MAXOEMLEN - lenL_RTCM, 0)) < 3)
		{
			printf("OEM SSR Info fail!\r\n");
			CloseSocket(sockOEM);
			OEMFlag = OpenSocket(sockOEM, IP, portOEM);
		}
		else
		{
			Sleep(1200);
			//调用相应的处理函数
			lenL_RTCM = RTCMProcess(RTCMBuff, lenR_RTCM + lenL_RTCM, &raw.rtcm);
			lenL_OEM = OEMProcess(OEMBuff, lenR_OEM + lenL_OEM,&raw,pos);
			//printf("%f\n", raw.Epoch.Time.SecOfWeek);
			fwrite(OEMBuff + lenL_OEM, sizeof(unsigned char), lenR_OEM, bindat);
		}
	}
	
	// 关闭端口
	CloseSocket(sockOEM);
	CloseSocket(sockRTCM);
	return 0;
}