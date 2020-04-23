#pragma once
#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

/* ͨ�ü�ʱ�� */
struct COMMONTIME
{
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Minute;
	double Second;
};

/* �������� */
struct MJDTIME
{
	int Days;
	double FracDay;
};

/* GPSʱ */
struct GPSTIME
{
	unsigned short Week;
	double SecOfWeek;
};

void COMMON2MJD(struct COMMONTIME* ctime, struct MJDTIME* mjd);
void MJD2COMMON(struct MJDTIME* mjd, struct COMMONTIME* ctime);
void MJD2GPST(struct MJDTIME* mjd, struct GPSTIME* gpst);
void GPST2MJD(struct GPSTIME* gpst, struct MJDTIME* mjd);
void COMMON2GPST(struct COMMONTIME* ctime, struct GPSTIME* gpst);
void GPST2COMMON(struct GPSTIME* gpst, struct COMMONTIME* ctime);
#endif