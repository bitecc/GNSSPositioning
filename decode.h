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
enum GNSSSYS{GPS,GLONASS,BDS,GALILEO,QZSS};
typedef struct {
	unsigned short prn;//卫星PRN号
	GNSSSYS Sys;
	float cno;//载躁比
	unsigned int track;//
	double L[2];//
	double L_std[2];//
	double P[2];//
	double P_std[2];//
	float D[2];//
}SatObs;//一颗卫星的观测数据
typedef struct {
	GPSTIME Time;
	double lat;
	double lon;
	double hgt;
	float lat_std;
	float lon_std;
	float h_std;
}PsrPos;
typedef struct {
	int prn;
	int iode,iodc;
	double sva;
	int svh;
	int week;
	GPSTIME toe, toc;
	double A, e, i0, omega0, omega, M0, deltan, omegad, idot;
	double crc, crs, cuc, cus, cic, cis;
	double tgd;
}eph_t;

typedef struct {
	GPSTIME Time;
	int SatNum;
	SatObs Obs[MAXCHANNUM];
}EpochObs;//所有卫星观测数据

struct raw_t {
	EpochObs Epoch;
	eph_t eph[NSAT];
	PsrPos Pos;
	PsrPos MyPos;
};
int find_head(unsigned char* buff, unsigned char data);
int DecodeOemstarDatFromBinFile(FILE* fp, raw_t* raw);
int crc32(const unsigned char* buff, int len);
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs);
#endif