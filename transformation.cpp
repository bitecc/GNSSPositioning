#include<stdio.h>
#include<math.h>
#include"self_defination.h"

/****************************************************************************
  COMMON2MJD

  目的：通用时转换为简化儒略日

  编号：02001

  参数：
  ctime     通用时，输入结构体的指针
  mjd       简化儒略日，输出结构体的指针
****************************************************************************/
void COMMON2MJD(struct COMMONTIME* ctime, struct MJDTIME* mjd)
{
    int y;
    int m;
    if (ctime->Month <= 2)
    {
        y = ctime->Year - 1;
        m = ctime->Month + 12;
    }
    else
    {
        y = ctime->Year;
        m = ctime->Month;
    }
    mjd->Days = (int)(365.25 * y) + (int)(30.6001 * ((double)m + 1)) + ctime->Day -679019;
    mjd->FracDay = ((double)ctime->Hour + ctime->Minute / 60.0 + ctime->Second / 3600) / 24;
}

/****************************************************************************
  MJD2COMMON

  目的：简化儒略日转换为通用时

  编号：02002

  参数：
  mjd       简化儒略日，输入结构体的指针
  ctime     通用时，输出结构体的指针
****************************************************************************/
void MJD2COMMON(struct MJDTIME* mjd, struct COMMONTIME* ctime)
{
    int a = (int)(mjd->Days + 2400001);
    int b = a + 1537;
    int c = (int)((b - 122.1) / 365.25);
    int d = (int)(c * 365.25);
    int e = (int)(((double)b - (double)d) / 30.6001);
    ctime->Day = b - d - (int)(30.60001 * e);
    ctime->Month = e - 1 - 12 * (int)(e / 14);
    ctime->Year = c - 4715 - (int)((7 + ctime->Month) / 10);
    ctime->Hour = (int)(mjd->FracDay * 24);
    ctime->Minute = (int)((mjd->FracDay - ctime->Hour / 24.0)*24*60);
    ctime->Second = (mjd->FracDay - ctime->Hour / 24.0 - ctime->Minute / 24.0 / 60.0) * 24 * 3600;
}

/****************************************************************************
  MJD2GPST

  目的：简化儒略日转换为GPS时

  编号：02003

  参数：
  mjd       简化儒略日，输入结构体的指针
  gpst      GPS时，输出结构体的指针
****************************************************************************/
void MJD2GPST(struct MJDTIME* mjd, struct GPSTIME* gpst)
{
    gpst->Week = (int)((mjd->Days + mjd->FracDay - 44244) / 7);
    gpst->Day = (unsigned short)(mjd->Days - 44244 - (double)gpst->Week * 7);
    gpst->SecOfDay = mjd->FracDay * 86400;
    gpst->SecOfWeek = (double)gpst->Day * 86400 + gpst->SecOfDay;
}

/****************************************************************************
  GPST2MJD

  目的：GPS时转换为简化儒略日

  编号：02004

  参数：
  gpst      GPS时，输入结构体的指针
  mjd       简化儒略日，输出结构体的指针
****************************************************************************/
void GPST2MJD(struct GPSTIME* gpst, struct MJDTIME* mjd)
{
    //mjd->Days = 44244 + gpst->Week * 7+(int)(gpst->SecOfWeek/86400);
    //mjd->FracDay = gpst->SecOfWeek / 86400 - (int)(gpst->SecOfWeek / 86400);
    mjd->Days = 44244 + gpst->Week * 7 + gpst->Day;
    mjd->FracDay = gpst->SecOfDay / 86400.0;
}

/****************************************************************************
  COMMON2GPST

  目的：通用时转换为GPS时

  编号：02005

  参数：
  ctime     通用时，输入结构体的指针
  gpst      GPS时，输出结构体的指针
****************************************************************************/
void COMMON2GPST(struct COMMONTIME* ctime, struct GPSTIME* gpst)
{
    struct MJDTIME mjd = { 0,0 };
    COMMON2MJD(ctime, &mjd);
    MJD2GPST(&mjd, gpst);
}

/****************************************************************************
  GPST2COMMON

  目的：GPS时转换为通用时

  编号：02006

  参数：
  gpst      GPS时，输入结构体的指针
  ctime     通用时，输出结构体的指针
****************************************************************************/
void GPST2COMMON(struct GPSTIME* gpst, struct COMMONTIME* ctime)
{
    struct MJDTIME mjd = { 0,0 };
    GPST2MJD(gpst, &mjd);
    MJD2COMMON(&mjd, ctime);
}

/****************************************************************************
  BLh2XYZ

  目的：大地坐标转换为笛卡尔坐标

  编号：02011

  参数：
  blh       大地坐标，输入结构体的指针
  xyz       笛卡尔坐标，输出结构体的指针

  返回值：0为转换失败，1为转换成功
****************************************************************************/
int BLh2XYZ(struct BLh* blh, struct XYZ* xyz)
{
    if(fabs(blh->B)>90||fabs(blh->L>180))
    {
        return 0;
    }
    double D2R = PI / 180.0;
    double e2 = f_WGS84 * 2 - f_WGS84 * f_WGS84;
    double N = a_WGS84 / sqrt(1 - e2 * pow(sin(blh->B * D2R), 2));
    xyz->X = (N + blh->H) * cos(blh->B * D2R) * cos(blh->L * D2R);
    xyz->Y = (N + blh->H) * cos(blh->B * D2R) * sin(blh->L * D2R);
    xyz->Z = (N * (1 - e2) + blh->H) * sin(blh->B * D2R);
    return 1;
}

/****************************************************************************
  XYZ2BLh

  目的：笛卡尔坐标转换为大地坐标

  编号：02012

  参数：
  xyz       笛卡尔坐标，输入结构体的指针
  blh       大地坐标，输出结构体的指针
  
  返回值：0为转换失败，1为转换成功
****************************************************************************/
int XYZ2BLh(struct XYZ* xyz, struct BLh* blh)
{
    
    double R2D = 180.0 / PI;
    double e2 = f_WGS84 * 2 - f_WGS84 * f_WGS84;
    double R2 = pow(xyz->X, 2) + pow(xyz->Y, 2);
    if (R2 + pow(xyz->Z, 2) < 1e-6)
    {
        return 0;
    }
    blh->L = atan2(xyz->Y, xyz->X) * R2D;
    double dZ = e2 * xyz->Z;
    double dZold = dZ;
    double SB = 0;
    double N = 0;
    int i = 0;//记录循环次数
    while (i < 25)//最多循环25次
    {
        i++;
        SB = (xyz->Z + dZ) / sqrt(R2 + pow(xyz->Z + dZ, 2));
        N = a_WGS84 / sqrt(1 - e2 * SB*SB);
        dZ = N * e2 * SB;
        if (fabs(dZ - dZold) < 1e-6)
            break;
        else
            dZold = dZ;
    }
    blh->B = atan2(xyz->Z + dZ, sqrt(R2)) * R2D;
    blh->H = sqrt(R2 + pow(xyz->Z + dZ, 2)) - N;
    return 1;
}