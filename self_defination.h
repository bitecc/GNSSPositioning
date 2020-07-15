#pragma once
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<windows.h>

/* 全局变量 */

#define NSAT 32 /* 卫星数 */
#define MAXCHANNUM 14
#define MAXRAWLEN 4096
#define MAXOEMLEN 5000
#define MAXRTCMLEN 500
enum GNSSSYS { GPS, GLONASS, BDS, GALILEO, QZSS };
const double PI = 3.1415926535898;
const double c = 2.99792458e8;/* 光速 */
const double miu = 3.986004415e14;/* GM */
const double Omega_e = 7.2921151467e-5;/* 地球自转角速度 */
const double f_L1 = 1575.42e6;/* L1的频率 */
const double a_WGS84 = 6378137.0;          /* Radius Earth [m]; WGS-84  */
const double f_WGS84 = 1.0 / 298.257223563;

/* 结构体类型定义 */

/* 通用计时法 */
struct COMMONTIME
{
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Minute;
	double Second;
};

/* 简化儒略日 */
struct MJDTIME
{
	int Days;
	double FracDay;
};

/* GPS时 */
struct GPSTIME
{
	unsigned short Week;
	double SecOfWeek;//周内秒，由于整数部分数值较大可能造成小数部分精度损失
	unsigned short Day;//一周的天数
	double SecOfDay;//每日的秒数
};

/* 笛卡尔坐标 */
struct XYZ
{
	double X;
	double Y;
	double Z;
};

/* 大地坐标 */
struct BLh
{
	double L;//经度，角度
	double B;//纬度，角度
	double H;
};

/* 一颗卫星的观测数据 */
typedef struct {
	unsigned short prn;//卫星PRN号
	GNSSSYS Sys;
	float cno;//载躁比
	unsigned int track;
	double Psr;
	double PsrStd;
	double Adr;
	double AdrStd;
	float dopp;
}SatObs;

/* 同历元所有卫星观测数据 */
typedef struct {
	GPSTIME Time;
	unsigned int SatNum;
	SatObs Obs[MAXCHANNUM];
}EpochObs;

/* 定位结果 */
typedef struct {
	GPSTIME Time;
	double x, y, z, vx, vy, vz, cdt, vcdt;
	BLh blh;
	double sigma0, vsigma0;
}PosResult;

/* 卫星星历数据 */
typedef struct {
	unsigned long PRN;
	double tow;
	unsigned long health;
	unsigned long IODE1, IODE2, iodc, Week;
	double toe, toc;
	double A, deltaN, M0, ecc, omega, omega0, I0, Idot, omegadot;
	double crc, crs, cuc, cus, cic, cis;
	double tgd, af0, af1, af2;
}eph_t;

/* 同历元所有星历数据 */
typedef struct {
	eph_t eph[NSAT];
}Ephem;

/* 电离层参数 */
typedef struct {
	double a0, a1, a2, a3;
	double b0, b1, b2, b3;
}IONUTC;

/* 卫星位置等信息 */
typedef struct {
	double x;
	double y;
	double z;
	double vx;
	double vy;
	double vz;
	double clk;
	double clkd;
}SatPos;

/* 所有卫星的位置信息 */
typedef struct {
	SatPos satpos[NSAT];
}SatPosSet;

typedef struct
{
	unsigned short hour;
	double secOfHour;
}SecOfHour;

/* 差分改正数 */
typedef struct
{
	int hasRead;
	SecOfHour soh;
	unsigned short AOD;
	unsigned short UDRE;
	float PRC;
	float RRC;
}DGPS;

/* RTCM解码信息 */
typedef struct
{
	SecOfHour soh;
	unsigned short len;
	unsigned short BSNum;
	unsigned short workState;
	unsigned short MsgID;
	unsigned char lastP;
	DGPS newDgps[NSAT];
	DGPS oldDgps[NSAT];
}RTCM;


struct raw_t {
	EpochObs Epoch;
	Ephem Eph;
	//PsrPos Pos;
	PosResult MyPos;
	IONUTC ionutc;
	RTCM rtcm;
};



/* 函数声明 */

/* 矩阵运算 */
int VectorAdd(int size_a, int size_b, double A[], double B[], double C[]);//向量加法
int VectorMinus(int size_a, int size_b, double A[], double B[], double C[]);//向量减法
int VectorDot(int size_a, int size_b, double A[], double B[], double C[]);//向量点乘
int VectorCross(int size_a, int size_b, double A[], double B[], double C[]);//向量叉乘
int MatrixAdd(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//矩阵加法
int MatrixMinus(int a1, int a2, int b1, int b2, double A[], double b[], double C[]);//矩阵减法
int MatrixMultiply(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//矩阵乘法
int MatrixInv(int n, double a[], double b[]);//矩阵求逆
int MatrixTrans(int a1, int a2, double A[], double B[]);//矩阵转置

/* 坐标转换与时间转换 */
int BLh2XYZ(struct BLh* blh, struct XYZ* xyz);
int XYZ2BLh(struct XYZ* xyz, struct BLh* blh);
void COMMON2MJD(struct COMMONTIME* ctime, struct MJDTIME* mjd);
void MJD2COMMON(struct MJDTIME* mjd, struct COMMONTIME* ctime);
void MJD2GPST(struct MJDTIME* mjd, struct GPSTIME* gpst);
void GPST2MJD(struct GPSTIME* gpst, struct MJDTIME* mjd);
void COMMON2GPST(struct COMMONTIME* ctime, struct GPSTIME* gpst);
void GPST2COMMON(struct GPSTIME* gpst, struct COMMONTIME* ctime);

/* 数据解码 */
int find_head(unsigned char* buff, unsigned char data);
int crc32(const unsigned char* buff, int len);
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs);
void DecodeGpsEphemb(unsigned char buff[], int len, Ephem* Eph);
void DecodeIONUTC(unsigned char buff[], int len, IONUTC* ionutc);
int DecodeOEMData(unsigned char* rawBuff, int LenR, int* sIndex, raw_t* raw);
int DecodeRTCMData(unsigned char* buff, int len, int* sIndex, RTCM* rtcm);

/* 定位 */
int SatPosition(Ephem* Eph, SatPosSet* satpos, GPSTIME* t, unsigned long prn, double psr, XYZ* xyz);
double Klobutchar(BLh* blh, GPSTIME* gpst, IONUTC* ionutc, double E, double A);
double Hopfield(BLh* blh, double E);
int spp(EpochObs* obs, Ephem* ephset, IONUTC* ionutc, PosResult* pos,RTCM* rtcm);

/* 差分GPS */
int RTCMProcess(unsigned char* Buff,int lenR,RTCM* rtcm);
int OEMProcess(unsigned char* Buff,int lenR,raw_t* raw,FILE* p);