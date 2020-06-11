#include<math.h>
#include"decode.h"
double PI = 3.1415926535898;
double c = 2.99792458e8;
double miu = 3.986004415e14;
double Omega_e = 7.2921151467e-5;
/***************************
Sat

功能：从二进制OEMSTAR数据文件，读取数据并解码，得到GNSS观测值、广播星历和接收机
	  定位结果，保存到raw结构体。
	  参考OEMSTAR命令文档

输入参数：fp
		  raw     观测数据、星历数据等
返回值：0=文件结束；1=观测数据；2=星历和定位结果；3=CRC检验不通过，4=没有支持语句
****************************/
int SatPos(eph_t* eph)
{
	//计算轨道长半轴
	double A = eph->A;
	//计算平均运动角速度
	double n0 = sqrt(miu / pow(A, 3));
	//计算相对于星历参考历元的时间
	double t_k = eph->t - eph->toe;

	return 0;

}