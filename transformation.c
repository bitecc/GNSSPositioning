#include"transformation.h"

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
    unsigned short y;
    unsigned short m;
    int JDDays;
    double JDFracDay;
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
    JDDays = (int)(365.25 * y) + (int)(30.6001 * (m + 1)) + ctime->Day + 1720981;
    JDFracDay = ((double)ctime->Hour + ctime->Minute / 60.0 + ctime->Second / 3600) / 24+0.5;
    mjd->Days = JDDays-2400000;
    mjd->FracDay = JDFracDay-0.5;
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
    int JDDays;
    double JDFracDay;
    JDDays = mjd->Days + 2400000;
    JDFracDay = mjd->FracDay + 0.5;
    if (JDFracDay >= 1)
    {
        JDFracDay -= 1;
        JDDays += 1;
    }
    int a = (int)(JDDays + JDFracDay + 0.5);
    int b = a + 1537;
    int c = (int)((b - 122.1) / 365.25);
    int d = (int)(c * 365.25);
    int e = (int)((b - d) / 30.6001);
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
    gpst->SecOfWeek = (mjd->Days + mjd->FracDay - 44244 - gpst->Week * 7) * 86400;
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