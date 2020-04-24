#include<stdio.h>
#include<math.h>
#include"transformation.h"
const double PI = 3.1415926535897932384626433832795;

/****************************************************************************
  COMMON2MJD

  Ŀ�ģ�ͨ��ʱת��Ϊ��������

  ��ţ�02001

  ������
  ctime     ͨ��ʱ������ṹ���ָ��
  mjd       �������գ�����ṹ���ָ��
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

  Ŀ�ģ���������ת��Ϊͨ��ʱ

  ��ţ�02002

  ������
  mjd       �������գ�����ṹ���ָ��
  ctime     ͨ��ʱ������ṹ���ָ��
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

  Ŀ�ģ���������ת��ΪGPSʱ

  ��ţ�02003

  ������
  mjd       �������գ�����ṹ���ָ��
  gpst      GPSʱ������ṹ���ָ��
****************************************************************************/
void MJD2GPST(struct MJDTIME* mjd, struct GPSTIME* gpst)
{
    gpst->Week = (int)((mjd->Days + mjd->FracDay - 44244) / 7);
    gpst->SecOfWeek = (mjd->Days + mjd->FracDay - 44244 - (double)gpst->Week * 7) * 86400;
}

/****************************************************************************
  GPST2MJD

  Ŀ�ģ�GPSʱת��Ϊ��������

  ��ţ�02004

  ������
  gpst      GPSʱ������ṹ���ָ��
  mjd       �������գ�����ṹ���ָ��
****************************************************************************/
void GPST2MJD(struct GPSTIME* gpst, struct MJDTIME* mjd)
{
    mjd->Days = 44244 + gpst->Week * 7+(int)(gpst->SecOfWeek/86400);
    mjd->FracDay = gpst->SecOfWeek / 86400 - (int)(gpst->SecOfWeek / 86400);
}

/****************************************************************************
  COMMON2GPST

  Ŀ�ģ�ͨ��ʱת��ΪGPSʱ

  ��ţ�02005

  ������
  ctime     ͨ��ʱ������ṹ���ָ��
  gpst      GPSʱ������ṹ���ָ��
****************************************************************************/
void COMMON2GPST(struct COMMONTIME* ctime, struct GPSTIME* gpst)
{
    struct MJDTIME mjd = { 0,0 };
    COMMON2MJD(ctime, &mjd);
    MJD2GPST(&mjd, gpst);
}

/****************************************************************************
  GPST2COMMON

  Ŀ�ģ�GPSʱת��Ϊͨ��ʱ

  ��ţ�02006

  ������
  gpst      GPSʱ������ṹ���ָ��
  ctime     ͨ��ʱ������ṹ���ָ��
****************************************************************************/
void GPST2COMMON(struct GPSTIME* gpst, struct COMMONTIME* ctime)
{
    struct MJDTIME mjd = { 0,0 };
    GPST2MJD(gpst, &mjd);
    MJD2COMMON(&mjd, ctime);
}

/****************************************************************************
  BLh2XYZ

  Ŀ�ģ��������ת��Ϊ�ѿ�������

  ��ţ�02011

  ������
  blh       ������꣬����ṹ���ָ��
  xyz       �ѿ������꣬����ṹ���ָ��
****************************************************************************/
void BLh2XYZ(struct BLh* blh, struct XYZ* xyz)
{
    double a = 6378137.0; double f = 1.0 / 298.257223563;
    double D2R = PI / 180.0;
    double e2 = f * 2 - f * f;
    double N = a / sqrt(1 - e2 * pow(sin(blh->B * D2R), 2));
    xyz->X = (N + blh->H) * cos(blh->B * D2R) * cos(blh->L * D2R);
    xyz->Y = (N + blh->H) * cos(blh->B * D2R) * sin(blh->L * D2R);
    xyz->Z = (N * (1 - e2) + blh->H) * sin(blh->B * D2R);
}

/****************************************************************************
  XYZ2BLh

  Ŀ�ģ��ѿ�������ת��Ϊ�������

  ��ţ�02012

  ������
  xyz       �ѿ������꣬����ṹ���ָ��
  blh       ������꣬����ṹ���ָ��
****************************************************************************/
void XYZ2BLh(struct XYZ* xyz, struct BLh* blh)
{
    double R2D = 180.0 / PI;
    double a = 6378137.0; double f = 1.0 / 298.257223563;
    double e2 = f * 2 - f * f;
    double R2 = pow(xyz->X, 2) + pow(xyz->Y, 2);
    blh->L = atan2(xyz->Y, xyz->X) * R2D;
    double dZ = e2 * xyz->Z;
    double dZold = dZ;
    double SB = 0;
    double N = 0;
    while (1) 
    {
        SB = (xyz->Z + dZ) / sqrt(R2 + pow(xyz->Z + dZ, 2));
        N = a / sqrt(1 - e2 * SB*SB);
        dZ = N * e2 * SB;
        if (fabs(dZ - dZold) < 1e-6)
            break;
        else
            dZold = dZ;
    }
    blh->B = atan2(xyz->Z + dZ, sqrt(R2)) * R2D;
    blh->H = sqrt(R2 + pow(xyz->Z + dZ, 2)) - N;
}