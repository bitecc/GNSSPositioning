#pragma once
#ifndef DECODE_H
#define DECODE_H
#include"transformation.h"
#include<stdio.h>
#define HEAD1 0xAA /* ͬ���ֽ�1 */
#define HEAD2 0x44 /* ͬ���ֽ�2 */
#define HEAD3 0x12 /* ͬ���ֽ�3 */
#define MAXRAWLEN 4096
#define MAXCHANNUM 14
#define HEADLEN 28 /* ����ͷ���� */
#define NSAT 32 /* ������ */
#define POLYCRC32 0xED888320u
#define ID_RANGE 43
#define ID_GPSEPHEM 7
#define ID_PSRPOS 47
#define ID_IONUTC 8
enum GNSSSYS{GPS,GLONASS,BDS,GALILEO,QZSS};
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
}SatObs;//һ�����ǵĹ۲�����
typedef struct {
	GPSTIME Time;
	unsigned int SatNum;
	SatObs Obs[MAXCHANNUM];
}EpochObs;//ͬ��Ԫ�������ǹ۲�����
typedef struct {
	GPSTIME Time;
	double lat;
	double lon;
	double hgt;
	float lat_std;
	float lon_std;
	float hgt_std;
}PsrPos;//�Ӷ������ļ���ȡ�Ķ�λ���
typedef struct {
	GPSTIME Time;
	double x, y, z, vx, vy, vz, cdt, vcdt;
	BLh blh;
	double sigma0, vsigma0;
}PosResult;//��λ���
typedef struct {
	unsigned long PRN;
	double tow;
	unsigned long health;
	unsigned long IODE1,IODE2,iodc,Week;
	double toe, toc;
	double A,deltaN,M0,ecc,omega,omega0,I0,Idot,omegadot;
	double crc, crs, cuc, cus, cic, cis;
	double tgd,af0,af1,af2;
}eph_t;//������������
typedef struct {
	eph_t eph[NSAT];
}Ephem;//ͬ��Ԫ������������
typedef struct {
	double a0, a1, a2, a3;
	double b0, b1, b2, b3;
}IONUTC;//��������
typedef struct {
	double x;
	double y;
	double z;
	double vx;
	double vy;
	double vz;
	double clk;
	double clkd;
}SatPos;//����λ�õ���Ϣ
typedef struct {
	SatPos satpos[NSAT];
}SatPosSet;//�������ǵ�λ����Ϣ
struct raw_t {
	EpochObs Epoch;
	Ephem Eph;
	PsrPos Pos;
	PosResult MyPos;
	IONUTC ionutc;
};
int find_head(unsigned char* buff, unsigned char data);
int DecodeOemstarDatFromBinFile(FILE* fp, raw_t* raw);
int crc32(const unsigned char* buff, int len);
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs);
void DecodeGpsEphemb(unsigned char buff[], int len, Ephem* Eph);
void DecodePsrPos(unsigned char buff[], int len, PsrPos* Pos);
void DecodeIONUTC(unsigned char buff[], int len, IONUTC* ionutc);
#endif