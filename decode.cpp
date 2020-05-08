#include<stdio.h>
#include<math.h>
#include<string.h>
#include"decode.h"
#define U1(p) (*((unsigned char *)(p)))
#define I1(p) (*((char *)(p)))
static unsigned short U2(unsigned char* p) { unsigned short u; memcpy(&u, p, 2); return u; };
static unsigned int U4(unsigned char* p) { unsigned int u; memcpy(&u, p, 4); return u; };
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
����ֵ��0=�ļ�������1=�۲����ݣ�2=�����Ͷ�λ�����3=CRC���鲻ͨ��;4=û��֧�����
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
		return 3;
	}

	Time.Week = U2(buff + 14);
	Time.SecOfWeek = U4(buff + 16) * 0.001;

	// ����MsgID������۲�ֵ/����/��λ���
	switch (MsgID)
	{
	case ID_RANGE:
		DecodeRangeb(buff + len - MsgLen - 4, MsgLen, &raw->Epoch);
		raw->Epoch.Time = Time;
		return 1;

	case ID_GPSEPHEM:
		//	DecodeGpsEphemb(buff+len-MsgLen-4, MsgLen, raw->eph);
		return 2;

	case ID_PSRPOS:
		//	DecodePsrPos(buff+len-MsgLen-4, MsgLen, &raw->Pos);
		return 2;

	default:
		return 4;
	}

	return 0;
}

/***************************
DecodeRangeb

���ܣ��Ӷ����ƻ������У�����OEMSTAT rangeb�����ݸ�ʽ������õ�GNSS�۲�ֵ��
	  �ο�OEMSTAR�����ĵ�

���������buff    ����data�Ļ�����
		  len     ���������ݵĳ���
		  Epoch   ��Ԫ�۲�����
����ֵ��0=�ļ�������1=�۲����ݣ�2=�����Ͷ�λ�����3=CRC���鲻ͨ��;4=û��֧�����
****************************/
void DecodeRangeb(unsigned char buff[], int len, EpochObs* Obs)
{
	int i, j;
	unsigned int ChStatus;
	unsigned char* p = buff;

	memset(Obs->Obs, 0, MAXCHANNUM * sizeof(SatObs));
	Obs->SatNum = U4(p);   p += 4;
	for (j = i = 0; i < Obs->SatNum; i++)
	{
		if (j > MAXCHANNUM)   return;

		Obs->Obs[j].prn = U2(p + 44 * i);
		Obs->Obs[j].P[0] = R8(p + 44 * i + 4);
		Obs->Obs[j].P_std[0] = R4(p + 44 * i + 12);
		Obs->Obs[j].L[0] = R8(p + 44 * i + 16);
		Obs->Obs[j].L_std[0] = R4(p + 44 * i + 24);
		Obs->Obs[j].D[0] = R4(p + 44 * i + 28);
		Obs->Obs[j].cno = R4(p + 44 * i + 32);
		ChStatus = U4(p + 44 * i + 40);

		if (((ChStatus >> 16) & 0x0F) == 0) {
			Obs->Obs[j].Sys = GPS;
			j++;
		}
	}
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