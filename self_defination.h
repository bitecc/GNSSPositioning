#pragma once
#include<stdio.h>

/* ȫ�ֱ��� */

#define NSAT 32 /* ������ */
#define MAXCHANNUM 14
#define MAXRAWLEN 4096
enum GNSSSYS { GPS, GLONASS, BDS, GALILEO, QZSS };
const double PI = 3.1415926535898;
const double c = 2.99792458e8;/* ���� */
const double miu = 3.986004415e14;/* GM */
const double Omega_e = 7.2921151467e-5;/* ������ת���ٶ� */
const double f_L1 = 1575.42e6;/* L1��Ƶ�� */
const double a_WGS84 = 6378137.0;          /* Radius Earth [m]; WGS-84  */
const double f_WGS84 = 1.0 / 298.257223563;

/* �ṹ�����Ͷ��� */

/* ͨ�ü�ʱ�� */
struct COMMONTIME
{
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Minute;
	double Second;
};

/* �������� */
struct MJDTIME
{
	int Days;
	double FracDay;
};

/* GPSʱ */
struct GPSTIME
{
	unsigned short Week;
	double SecOfWeek;//�����룬��������������ֵ�ϴ�������С�����־�����ʧ
	unsigned short Day;//һ�ܵ�����
	double SecOfDay;//ÿ�յ�����
};

/* �ѿ������� */
struct XYZ
{
	double X;
	double Y;
	double Z;
};

/* ������� */
struct BLh
{
	double L;//���ȣ��Ƕ�
	double B;//γ�ȣ��Ƕ�
	double H;
};

/* һ�����ǵĹ۲����� */
typedef struct {
	unsigned short prn;//����PRN��
	GNSSSYS Sys;
	float cno;//�����
	unsigned int track;
	double Psr;
	double PsrStd;
	double Adr;
	double AdrStd;
	float dopp;
}SatObs;

/* ͬ��Ԫ�������ǹ۲����� */
typedef struct {
	GPSTIME Time;
	unsigned int SatNum;
	SatObs Obs[MAXCHANNUM];
}EpochObs;

/* �Ӷ������ļ���ȡ�Ķ�λ��� */
typedef struct {
	GPSTIME Time;
	double lat;
	double lon;
	double hgt;
	float lat_std;
	float lon_std;
	float hgt_std;
}PsrPos;

/* ��λ��� */
typedef struct {
	GPSTIME Time;
	double x, y, z, vx, vy, vz, cdt, vcdt;
	BLh blh;
	double sigma0, vsigma0;
}PosResult;

/* ������������ */
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

/* ͬ��Ԫ������������ */
typedef struct {
	eph_t eph[NSAT];
}Ephem;

/* �������� */
typedef struct {
	double a0, a1, a2, a3;
	double b0, b1, b2, b3;
}IONUTC;

/* ����λ�õ���Ϣ */
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

/* �������ǵ�λ����Ϣ */
typedef struct {
	SatPos satpos[NSAT];
}SatPosSet;

struct raw_t {
	EpochObs Epoch;
	Ephem Eph;
	PsrPos Pos;
	PosResult MyPos;
	IONUTC ionutc;
};


/* �������� */

/* �������� */
int VectorAdd(int size_a, int size_b, double A[], double B[], double C[]);//�����ӷ�
int VectorMinus(int size_a, int size_b, double A[], double B[], double C[]);//��������
int VectorDot(int size_a, int size_b, double A[], double B[], double C[]);//�������
int VectorCross(int size_a, int size_b, double A[], double B[], double C[]);//�������
int MatrixAdd(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//����ӷ�
int MatrixMinus(int a1, int a2, int b1, int b2, double A[], double b[], double C[]);//�������
int MatrixMultiply(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//����˷�
int MatrixInv(int n, double a[], double b[]);//��������
int MatrixTrans(int a1, int a2, double A[], double B[]);//����ת��

/* ����ת����ʱ��ת�� */
int BLh2XYZ(struct BLh* blh, struct XYZ* xyz);
int XYZ2BLh(struct XYZ* xyz, struct BLh* blh);
void COMMON2MJD(struct COMMONTIME* ctime, struct MJDTIME* mjd);
void MJD2COMMON(struct MJDTIME* mjd, struct COMMONTIME* ctime);
void MJD2GPST(struct MJDTIME* mjd, struct GPSTIME* gpst);
void GPST2MJD(struct GPSTIME* gpst, struct MJDTIME* mjd);
void COMMON2GPST(struct COMMONTIME* ctime, struct GPSTIME* gpst);
void GPST2COMMON(struct GPSTIME* gpst, struct COMMONTIME* ctime);

/* ���ݽ��� */
int find_head(unsigned char* buff, unsigned char data);
int DecodeOemstarDatFromBinFile(FILE* fp, raw_t* raw);
int crc32(const unsigned char* buff, int len);
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs);
void DecodeGpsEphemb(unsigned char buff[], int len, Ephem* Eph);
void DecodePsrPos(unsigned char buff[], int len, PsrPos* Pos);
void DecodeIONUTC(unsigned char buff[], int len, IONUTC* ionutc);

/* ��λ */
int SatPosition(Ephem* Eph, SatPos* satpos, GPSTIME* t, unsigned long prn, double psr, XYZ* xyz);
double Klobutchar(BLh* blh, GPSTIME* gpst, IONUTC* ionutc, double E, double A);
double Hopfield(BLh* blh, double E);
int spp(EpochObs* obs, Ephem* ephset, IONUTC* ionutc, PosResult* pos);