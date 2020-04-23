#include"transformation.h"

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

  目的：简化儒略日转换为通用时

  编号：02002

  参数：
  mjd       简化儒略日，输入结构体的指针
  ctime     通用时，输出结构体的指针
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

  目的：简化儒略日转换为GPS时

  编号：02003

  参数：
  mjd       简化儒略日，输入结构体的指针
  gpst      GPS时，输出结构体的指针
****************************************************************************/
void MJD2GPST(struct MJDTIME* mjd, struct GPSTIME* gpst)
{
    gpst->Week = (int)((mjd->Days + mjd->FracDay - 44244) / 7);
    gpst->SecOfWeek = (mjd->Days + mjd->FracDay - 44244 - gpst->Week * 7) * 86400;
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
    mjd->Days = 44244 + gpst->Week * 7+(int)(gpst->SecOfWeek/86400);
    mjd->FracDay = gpst->SecOfWeek / 86400 - (int)(gpst->SecOfWeek / 86400);
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