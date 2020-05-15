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
enum GNSSSYS{GPS,GLONASS,BDS,GALILEO,QZSS};
typedef struct {
	unsigned short prn;//����PRN��
	GNSSSYS Sys;
	float cno;//�����
	unsigned int track;//
	double L[2];//
	double L_std[2];//
	double P[2];//
	double P_std[2];//
	float D[2];//
}SatObs;//һ�����ǵĹ۲�����
typedef struct {
	GPSTIME Time;
	int SatNum;
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
}PsrPos;//��λ���
typedef struct {
	unsigned long PRN;
	double tow;
	unsigned long health;
	unsigned long IODE1,IODE2,iodc;
	GPSTIME toe, toc;
	double A,deltaN,M0,ecc,omega,omega0,I0,Idot,omegadot;
	double crc, crs, cuc, cus, cic, cis;
	double tgd;
}eph_t;//������������
typedef struct {
	eph_t eph[NSAT];
}Ephem;//ͬ��Ԫ������������
struct raw_t {
	EpochObs Epoch;
	Ephem Eph;
	PsrPos Pos;
	PsrPos MyPos;
};
int find_head(unsigned char* buff, unsigned char data);
int DecodeOemstarDatFromBinFile(FILE* fp, raw_t* raw);
int crc32(const unsigned char* buff, int len);
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs);
void DecodeGpsEphemb(unsigned char buff[], int len, Ephem* Eph);
void DecodePsrPos(unsigned char buff[], int len, PsrPos* Pos);
#endif