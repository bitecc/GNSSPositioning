#include<stdio.h>
#include<math.h>
#include<string.h>
#include"self_defination.h"
#define HEAD1 0xAA /* ͬ���ֽ�1 */
#define HEAD2 0x44 /* ͬ���ֽ�2 */
#define HEAD3 0x12 /* ͬ���ֽ�3 */
#define HEADLEN 28 /* ����ͷ���� */
#define POLYCRC32 0xED888320u
#define ID_RANGE 43
#define ID_GPSEPHEM 7
#define ID_PSRPOS 47
#define ID_IONUTC 8
#define U1(p) (*((unsigned char *)(p)))
#define I1(p) (*((char *)(p)))
static unsigned short U2(unsigned char* p) { unsigned short u; memcpy(&u, p, 2); return u; };
static unsigned int U4(unsigned char* p) { unsigned int u; memcpy(&u, p, 4); return u; };
static long L4(unsigned char* p) { long l; memcpy(&l, p, 4); return l; };
static int I4(unsigned char* p) { int i; memcpy(&i, p, 4); return i; };
static float R4(unsigned char* p) { float r; memcpy(&r, p, 4); return r; };
static double R8(unsigned char* p) { double r; memcpy(&r, p, 8); return r; };
#define POLYCRC32 0xEDB88320u /* CRC32 polynomial */
int find_head(unsigned char* buff, unsigned char data)
{
	buff[0] = buff[1]; buff[1] = buff[2]; buff[2] = data;
	return buff[0] == HEAD1 && buff[1] == HEAD2 && buff[2] == HEAD3;
}

/***************************
DecodeOemstarDatFromBinFile

���ܣ��Ӷ�����OEMSTAR�����ļ�����ȡ���ݲ����룬�õ�GNSS�۲�ֵ���㲥�����ͽ��ջ�
	  ��λ��������浽raw�ṹ�塣
	  �ο�OEMSTAR�����ĵ�

���������fp
		  raw     �۲����ݡ��������ݵ�
����ֵ��0=�ļ�������1=�۲����ݣ�2=������3=��λ�����4=CRC���鲻ͨ����5=û��֧����䣬6=IONUTC
****************************/
int DecodeOemstarDatFromBinFile(FILE* fp, raw_t* raw)
{
	GPSTIME Time;
	unsigned short len, MsgID, MsgLen;
	unsigned char dat, buff[MAXRAWLEN];

	memset(buff, 0, MAXRAWLEN * sizeof(unsigned char));

	// ����ͬ���ַ�
	while (!feof(fp))
	{
		if ((dat = fgetc(fp)) == EOF)
			return 0;
		if (find_head(buff, dat) == true)
			break;
	}

	// ��25���ַ������header
	dat = fgetc(fp);    // Header����
	if (dat > MAXRAWLEN)  return 2;
	buff[3] = dat;
	if (fread(buff + 4, sizeof(unsigned char), dat - 4, fp) != dat - 4)  return 0;
	len = dat;

	// ��header��MsgLen MsgID
	MsgID = U2(buff + 4);
	MsgLen = U2(buff + 8);

	// ��MsgLen+4�ַ�����header���һ�����
	if ((len + MsgLen + 4) > MAXRAWLEN)  return 2;
	if (fread(buff + len, sizeof(unsigned char), MsgLen + 4, fp) != MsgLen + 4)  return 0;
	len = len + MsgLen + 4;

	// CRC���飬��ͨ���򷵻�
	if (crc32(buff, len - 4) != I4(buff + len - 4)) {
		printf("CRC check fail.\n");
		return 4;
	}

	Time.Week = U2(buff + 14);
	Time.SecOfWeek = U4(buff + 16) * 0.001;
	unsigned long prn;
	// ����MsgID������۲�ֵ/����/��λ���
	switch (MsgID)
	{
	case ID_RANGE:
		raw->Epoch.Time = Time;
		DecodeRangeb(buff + len - MsgLen - 4, MsgLen, &raw->Epoch);
		return 1;

	case ID_GPSEPHEM:
		DecodeGpsEphemb(buff + len - MsgLen - 4, MsgLen, &raw->Eph);
		return 2;

	case ID_PSRPOS:
		DecodePsrPos(buff + len - MsgLen - 4, MsgLen, &raw->Pos);
		raw->Pos.Time = Time;
		return 3;

	case ID_IONUTC:
		DecodeIONUTC(buff + len - MsgLen - 4, MsgLen, &raw->ionutc);
		return 6;

	default:
		return 5;
	}

	return 0;
}

/***************************
DecodeRangeb

���ܣ��Ӷ����ƻ������У�����OEMSTAR rangeb�����ݸ�ʽ������õ�GNSS�۲�ֵ��
	  �ο�OEMSTAR�����ĵ�

���������buff    ����data�Ļ�����
		  len     ���������ݵĳ���
		  Epoch   ��Ԫ�۲�����
****************************/
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs)
{
	int i, j;
	unsigned int ChStatus;
	unsigned char* p = buff;

	memset(Obs->Obs, 0, MAXCHANNUM * sizeof(SatObs));
	Obs->SatNum = L4(p);   p += 4;
	for (j = i = 0; i < Obs->SatNum; i++)
	{
		if (j > MAXCHANNUM)   return;

		Obs->Obs[j].prn = U2(p + 44 * i);
		
		Obs->Obs[j].Psr = R8(p + 44 * i + 4);
		Obs->Obs[j].PsrStd = R4(p + 44 * i + 12);
		Obs->Obs[j].Adr = R8(p + 44 * i + 16);
		Obs->Obs[j].AdrStd = R4(p + 44 * i + 24);
		Obs->Obs[j].dopp = R4(p + 44 * i + 28);
		Obs->Obs[j].cno = R4(p + 44 * i + 32);
		ChStatus = U4(p + 44 * i + 40);

		if (((ChStatus >> 16) & 0x0F) == 0) {
			Obs->Obs[j].Sys = GPS;
			j++;
		}
	}
}

/***************************
DecodeGpsEphemb

���ܣ��Ӷ����ƻ������У�����OEMSTAT GPSEPHEM�����ݸ�ʽ������õ�GPS������
	  �ο�OEMSTAR�����ĵ�

���������buff    ����data�Ļ�����
		  len     ���������ݵĳ���
		  Eph	  ��������
****************************/
void DecodeGpsEphemb(unsigned char buff[], int len, Ephem* Eph)
{
	unsigned char* p = buff;
	unsigned long prn = U4(p);
	if (prn <= 0 || prn > NSAT)
		return;
	Eph->eph[prn-1].PRN = prn;
	Eph->eph[prn - 1].tow = R8(p + 4);
	Eph->eph[prn - 1].health = U4(p  + 12);
	Eph->eph[prn - 1].IODE1 = U4(p + 16);
	Eph->eph[prn - 1].IODE2 = U4(p + 20);
	Eph->eph[prn - 1].Week = U4(p + 24);
	Eph->eph[prn - 1].toe = R8(p + 32);
	Eph->eph[prn - 1].A = R8(p + 40);
	Eph->eph[prn - 1].deltaN = R8(p + 48);
	Eph->eph[prn - 1].M0 = R8(p + 56);
	Eph->eph[prn - 1].ecc = R8(p + 64);
	Eph->eph[prn - 1].omega = R8(p + 72);
	Eph->eph[prn - 1].cuc = R8(p + 80);
	Eph->eph[prn - 1].cus = R8(p + 88);
	Eph->eph[prn - 1].crc = R8(p + 96);
	Eph->eph[prn - 1].crs = R8(p + 104);
	Eph->eph[prn - 1].cic = R8(p + 112);
	Eph->eph[prn - 1].cis = R8(p + 120);
	Eph->eph[prn - 1].I0 = R8(p + 128);
	Eph->eph[prn - 1].Idot = R8(p + 136);
	Eph->eph[prn - 1].omega0 = R8(p + 144);
	Eph->eph[prn - 1].omegadot = R8(p + 152);
	Eph->eph[prn - 1].iodc = U4(p + 160);
	Eph->eph[prn - 1].toc = R8(p + 164);
	Eph->eph[prn - 1].tgd = R8(p + 172);
	Eph->eph[prn - 1].af0 = R8(p + 180);
	Eph->eph[prn - 1].af1 = R8(p + 188);
	Eph->eph[prn - 1].af2 = R8(p + 196);
}

/***************************
DecodePsrPos

���ܣ��Ӷ����ƻ������У�����OEMSTAR PSRPOS�����ݸ�ʽ������õ�GPS��λ�����
	  �ο�OEMSTAR�����ĵ�

���������buff    ����data�Ļ�����
		  len     ���������ݵĳ���
		  Pos	  ��λ���
****************************/
void DecodePsrPos(unsigned char buff[], int len, PsrPos* Pos)
{
	unsigned char* p = buff;
	memset(Pos, 0, sizeof(PsrPos));
	Pos->lat = R8(p + 8);
	Pos->lon = R8(p + 16);
	Pos->hgt = R8(p + 24);
	Pos->lat_std = R4(p + 40);
	Pos->lon_std = R4(p + 44);
	Pos->hgt_std = R4(p + 48);
}

void DecodeIONUTC(unsigned char buff[], int len, IONUTC* ionutc)
{
	unsigned char* p = buff;
	ionutc->a0 = R8(p);
	ionutc->a1 = R8(p + 8);
	ionutc->a2 = R8(p + 16);
	ionutc->a3 = R8(p + 24);
	ionutc->b0 = R8(p + 32);
	ionutc->b1 = R8(p + 40);
	ionutc->b2 = R8(p + 48);
	ionutc->b3 = R8(p + 56);
}

int crc32(const unsigned char* buff, int len)
{
	int i, j;
	unsigned int crc = 0;
	for (i = 0; i < len; i++)
	{
		crc ^= buff[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 1) crc = (crc >> 1) ^ POLYCRC32;
			else crc >>= 1;
		}
	}
	return crc;
}