#include"self_defination.h"
#define HEAD1 0xAA /* 同步字节1 */
#define HEAD2 0x44 /* 同步字节2 */
#define HEAD3 0x12 /* 同步字节3 */
#define HEADLEN 28 /* 数据头长度 */
#define POLYCRC32 0xED888320u
#define ID_RANGE 43
#define ID_GPSEPHEM 7
#define ID_PSRPOS 47
#define ID_IONUTC 8
#define U1(p) (*((unsigned char *)(p)))
#define I1(p) (*((char *)(p)))
static unsigned short U2(unsigned char* p) { unsigned short u; memcpy(&u, p, 2); return u; };
static short I2(unsigned char* p) { short i; memcpy(&i, p, 2); return i; };
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
DecodeRangeb

功能：从二进制缓冲区中，根据OEMSTAR rangeb的数据格式，解码得到GNSS观测值。
	  参考OEMSTAR命令文档

输入参数：buff    保存data的缓冲区
		  len     缓冲区数据的长度
		  Epoch   历元观测数据
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

功能：从二进制缓冲区中，根据OEMSTAT GPSEPHEM的数据格式，解码得到GPS星历。
	  参考OEMSTAR命令文档

输入参数：buff    保存data的缓冲区
		  len     缓冲区数据的长度
		  Eph	  星历数据
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

/***************************
DecodeOEMData

功能：从二进制OEMSTAR数据文件，读取数据并解码，得到GNSS观测值、广播星历和接收机定位结果，保存到raw结构体。

输入参数：rawBuff	数据缓冲区
		  raw		观测数据、星历数据等
返回值：0=结束；1=观测数据；2=星历，-1=CRC检验不通过，6=IONUTC
****************************/
int DecodeOEMData(unsigned char* rawBuff, int LenR, int* sIndex, raw_t* raw)
{
	int index = 0;
	GPSTIME Time;
	unsigned char buff[MAXOEMLEN];
	unsigned char headBegin;
	unsigned short headerLen, msgLen, tolLen, msgID;
	while (index < LenR)
	{
		if (index + 1 > LenR)
		{
			*sIndex += index;
			return 0;
		}
		headBegin = rawBuff[index++];
		if (find_head(buff, headBegin)) break;
	}
	if (index + 1 > LenR)
	{
		*sIndex += index;
		return 0;
	}
	headerLen = rawBuff[index++];
	buff[3] = headerLen;
	if (index + headerLen - 4 > LenR)
	{
		*sIndex += index;
		return 0;
	}
	memcpy(buff + 4, rawBuff + index, headerLen - 4);
	index += headerLen - 4;
	msgID = U2(buff + 4);
	msgLen = U2(buff + 8);
	tolLen = headerLen + msgLen + 4;
	if (index + msgLen + 4 > LenR)
	{
		*sIndex += index;
		return 0;
	}
	memcpy(buff + headerLen, rawBuff + index, msgLen + 4);
	index += msgLen + 4;
	*sIndex += index;
	if (crc32(buff, tolLen - 4) != I4(buff + tolLen - 4))
	{
		printf("CRC校验未通过！\n");
		return -1;
	}
	Time.Week = U2(buff + 14);
	Time.SecOfWeek = U4(buff + 16) * 0.001;
	switch (msgID)
	{
	case ID_RANGE:
	{
		raw->Epoch.Time = Time;
		DecodeRangeb(buff + headerLen, msgLen, &raw->Epoch);
		return 1;
	}
	case ID_GPSEPHEM:
	{
		DecodeGpsEphemb(buff + headerLen, msgLen, &raw->Eph);
		return 2;
	}
	case ID_IONUTC:
	{
		DecodeIONUTC(buff + headerLen, msgLen, &raw->ionutc);
		return 6;
	}
	default:
		break;
	}
	return 0;
}

//字节滚动
int decodeRTCMByte(unsigned char bDat)
{
	bDat = bDat << 1;
	bDat = (bDat & 0x55) << 1 | (bDat & 0xAA) >> 1;
	bDat = (bDat & 0x33) << 2 | (bDat & 0xCC) >> 2;
	bDat = (bDat & 0x0F) << 4 | (bDat & 0xF0) >> 4;
	bDat = (bDat >> 1) & 63;
	return bDat;
}

//RTCM奇偶校验
int RTCMParityChk(unsigned char lastParity, unsigned char word[4])
{
	int i, parity, d29, d30, checkedNum = 0;
	//拓展为30位
	unsigned char d[30] = { 0 };
	for (i = 0; i < 30; i++)
	{
		d[i] = (word[i / 8] >> (7 - (i % 8))) & 0x1;
		if (i >= 24) d[i] = (word[i / 8] >> (5 - (i % 8))) & 0x1;
	}

	//进行奇偶校验
	d29 = (lastParity >> 1) & 0x1;
	d30 = lastParity & 0x1;

	parity = d[0] ^ d[1] ^ d[2] ^ d[4] ^ d[5] ^ d[9] ^ d[10] ^ d[11] ^ d[12] ^ d[13]
		^ d[16] ^ d[17] ^ d[19] ^ d[22] ^ d29;
	if (parity == d[24]) checkedNum++;

	parity = d[1] ^ d[2] ^ d[3] ^ d[5] ^ d[6] ^ d[10] ^ d[11] ^ d[12] ^ d[13] ^ d[14]
		^ d[17] ^ d[18] ^ d[20] ^ d[23] ^ d30;
	if (parity == d[25]) checkedNum++;

	parity = d[0] ^ d[2] ^ d[3] ^ d[4] ^ d[6] ^ d[7] ^ d[11] ^ d[12] ^ d[13] ^ d[14]
		^ d[15] ^ d[18] ^ d[19] ^ d[21] ^ d29;
	if (parity == d[26]) checkedNum++;

	parity = d[1] ^ d[3] ^ d[4] ^ d[5] ^ d[7] ^ d[8] ^ d[12] ^ d[13] ^ d[14]
		^ d[15] ^ d[16] ^ d[19] ^ d[20] ^ d[22] ^ d30;
	if (parity == d[27]) checkedNum++;

	parity = d[0] ^ d[2] ^ d[4] ^ d[5] ^ d[6] ^ d[8] ^ d[9] ^ d[13] ^ d[14]
		^ d[15] ^ d[16] ^ d[17] ^ d[20] ^ d[21] ^ d[23] ^ d30;
	if (parity == d[28]) checkedNum++;

	parity = d[2] ^ d[4] ^ d[5] ^ d[7] ^ d[8] ^ d[9] ^ d[10] ^ d[12]
		^ d[14] ^ d[18] ^ d[21] ^ d[22] ^ d[23] ^ d29;
	if (parity == d[29]) checkedNum++;

	if (checkedNum == 6) return 1;
	else return 0;
}

//获取一个字
int getRTCMWord(unsigned char word[4], unsigned char lastParity, unsigned char rawword[5])
{
	int i;
	//字节翻转
	for (i = 0; i < 5; i++)
	{
		if (rawword[i] < 64 && rawword[i] > 127) return -1;
		rawword[i] = decodeRTCMByte(rawword[i]);
	}
	//拼接为一个字
	word[0] = (rawword[0] << 2) | (rawword[1] >> 4);
	word[1] = (rawword[1] << 4) | (rawword[2] >> 2);
	word[2] = (rawword[2] << 6) | (rawword[3]);
	word[3] = rawword[4];

	//d30为1时对前24位取反码
	if ((lastParity & 0x1) == 1)
	{
		for (i = 0; i < 3; i++) word[i] = word[i] ^ 0xFF;
	}

	//奇偶校验
	if (!RTCMParityChk(lastParity, word)) return 0;

	return 1;
}

//解码RTCM文件体，电文1
int decodeRTCMType1(unsigned char buff[], unsigned short bodyLgt, double soh, DGPS dgps[])
{
	int i;
	unsigned short PRN;
	float sclFct;
	unsigned char tempPRC[2];
	for (i = 0; i < (bodyLgt * 3) / 5; i++)
	{
		PRN = (unsigned short)(buff[i * 5 + 0] & 0x1F);
		if (PRN == 0) PRN = 32;
		dgps[PRN - 1].UDRE = (unsigned short)(buff[i * 5 + 0] >> 5 & 0x3);
		if ((unsigned short)(buff[i * 5 + 0] >> 7)) sclFct = 0.32;
		else sclFct = 0.02;
		tempPRC[1] = buff[i * 5 + 1]; tempPRC[0] = buff[i * 5 + 2];
		dgps[PRN - 1].hasRead = 1;
		dgps[PRN - 1].soh = soh;
		dgps[PRN - 1].PRC = I2(tempPRC) * sclFct;
		dgps[PRN - 1].RRC = (short)buff[i * 5 + 3] * sclFct * 0.1;
		dgps[PRN - 1].AOD = (unsigned short)buff[i * 5 + 4];
	}
	return 1;
}

/***************************
DecodeRTCMData

功能：读取RTCM的文件头和电文1

输入参数：buff		数据缓冲区
		  len		数据缓冲区的长度
		  sIndex	标记每一组电文的开始位置
返回值：0=结束；1=电文1解码成功；-1=奇偶检验不通过，-2=空间不足
****************************/
int DecodeRTCMData(unsigned char* buff, int len, int* sIndex, RTCM* rtcm)
{
	int a, i, flag1, flag2, index = 0;
	unsigned char lastParity, firstDat, secondDat, bodyRst[MAXRTCMLEN], rawWord[5], word[4];
	DGPS tDgps[NSAT];
	firstDat = rtcm->lastP;
	//字节扫描
	if (firstDat == 0)
	{
		while ((firstDat < 64 || firstDat>127))
		{
			if (index + 1 > len)
			{
				*sIndex += index;
				return 0;
			}
			firstDat = buff[index++];
		}
		firstDat = decodeRTCMByte(firstDat);
	}
	lastParity = 2;
	while (1)
	{
		while (1)
		{
			if (index + 1 > len)
			{
				rtcm->lastP = firstDat;
				*sIndex += index;
				return 0;
			}
			secondDat = buff[index++];
			secondDat = decodeRTCMByte(secondDat);
			if (((lastParity & 0x1) == 0) && (firstDat == 0x19) && (secondDat >= 0x20 && secondDat < 0x30)) break;
			if (((lastParity & 0x1) == 1) && (firstDat == 0x26) && (secondDat >= 0x16 && secondDat < 0x20)) break;
			lastParity = firstDat; firstDat = secondDat;
		}
		//获取文件头的第一个字
		index -= 2;
		if (index + 5 > len)
		{
			rtcm->lastP = lastParity;
			*sIndex += index;
			return 0;
		}
		memcpy(rawWord, buff + index, 5);
		index += 5;
		flag1 = getRTCMWord(word, lastParity, rawWord);
		lastParity = word[3];
		if (flag1 < 0)continue;
		memcpy(bodyRst, word, 3);
		//获取第二个字
		if (index + 5 > len)
		{
			rtcm->lastP = word[3];
			*sIndex += index;
			return 0;
		}
		memcpy(rawWord, buff + index, 5);
		index = index + 5;
		flag2 = getRTCMWord(word, lastParity, rawWord);
		lastParity = word[3];
		if (flag2 < 0) continue;
		memcpy(bodyRst + 3, word, 3);

		//两个字的奇偶校验通过则结束寻找
		if (flag1 && flag2) break;
	}
	unsigned char tempBuff[2];
	unsigned short msgLgt;
	unsigned short MsgID = (unsigned short)(bodyRst[1] >> 2);
	//计算基准站数量
	tempBuff[1] = bodyRst[1] & 0x3; tempBuff[0] = bodyRst[2];
	rtcm->BSNum = U2(tempBuff);
	//工作状态
	rtcm->workState = bodyRst[5] & 0x7;
	//周内秒
	tempBuff[0] = ((bodyRst[3] & 0x7) << 5) | (bodyRst[4] >> 3); tempBuff[1] = bodyRst[3] >> 3;
	rtcm->soh = U2(tempBuff) * 0.6;

	msgLgt = (unsigned short)(bodyRst[5] >> 3);
	rtcm->len = msgLgt;
	//读取后续文件体的数据
	for (i = 0; i < msgLgt - 2; i++)
	{
		if (index + 5 > len)
		{
			rtcm->lastP = word[3];
			*sIndex += index;
			return 0;
		}
		memcpy(rawWord, buff + index, 5);
		index = index + 5;
		flag1 = getRTCMWord(word, word[3], rawWord);
		//校验未通过
		if (flag1 != 1)
		{
			*sIndex += index;
			rtcm->lastP = word[3];
			return -1;
		}
		//buff空间不足
		if (9 + i * 3 > MAXRTCMLEN)
		{
			*sIndex += index;
			rtcm->lastP = word[3];
			return -2;
		}
		//拷贝后续数据至buff
		memcpy(bodyRst + 6 + i * 3, word, 3);
	}
	switch (MsgID)
	{
	case 1:
		decodeRTCMType1(bodyRst + 6, msgLgt - 2, rtcm->soh, tDgps);
		//将new中IOD不同的值DGPS值赋给old
		for (i = 0; i < NSAT; i++) {
			if (tDgps[i].hasRead == 1)
				rtcm->dGPS[i] = tDgps[i];
		}
		break;
	default:
		//printf("暂不支持该类型的解码！\n");
		break;
	}

	*sIndex += index;
	rtcm->lastP = word[3];
	return 1;
}