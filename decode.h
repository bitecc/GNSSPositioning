#pragma once
#ifndef DECODE_H
#define DECODE_H
#include"transformation.h"
#include<stdio.h>
#define HEAD1 0xAA /* 同步字节1 */
#define HEAD2 0x44 /* 同步字节2 */
#define HEAD3 0x12 /* 同步字节3 */
#define MAXRAWLEN 4096
#define MAXCHANNUM 14
#define HEADLEN 28 /* 数据头长度 */
#define NSAT 32 /* 卫星数 */
#define POLYCRC32 0xED888320u
#define ID_RANGE 43
#define ID_GPSEPHEM 7
#define ID_PSRPOS 47
#define ID_IONUTC 8
enum GNSSSYS{GPS,GLONASS,BDS,GALILEO,QZSS};
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
}SatObs;//一颗卫星的观测数据
typedef struct {
	GPSTIME Time;
	unsigned int SatNum;
	SatObs Obs[MAXCHANNUM];
}EpochObs;//同历元所有卫星观测数据
typedef struct {
	GPSTIME Time;
	double lat;
	double lon;
	double hgt;
	float lat_std;
	float lon_std;
	float hgt_std;
}PsrPos;//从二进制文件提取的定位结果
typedef struct {
	GPSTIME Time;
	double x, y, z, vx, vy, vz, cdt, vcdt;
	BLh blh;
	double sigma0, vsigma0;
}PosResult;//定位结果
typedef struct {
	unsigned long PRN;
	double tow;
	unsigned long health;
	unsigned long IODE1,IODE2,iodc,Week;
	double toe, toc;
	double A,deltaN,M0,ecc,omega,omega0,I0,Idot,omegadot;
	double crc, crs, cuc, cus, cic, cis;
	double tgd,af0,af1,af2;
}eph_t;//卫星星历数据
typedef struct {
	eph_t eph[NSAT];
}Ephem;//同历元所有星历数据
typedef struct {
	double a0, a1, a2, a3;
	double b0, b1, b2, b3;
}IONUTC;//电离层参数
typedef struct {
	double x;
	double y;
	double z;
	double vx;
	double vy;
	double vz;
	double clk;
	double clkd;
}SatPos;//卫星位置等信息
typedef struct {
	SatPos satpos[NSAT];
}SatPosSet;//所有卫星的位置信息
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