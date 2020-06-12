#pragma once
#pragma once
#ifndef SPP_H
#define SPP_H
#define NSAT 32 /* ÎÀÐÇÊý */
int SatPosition(Ephem* Eph, SatPosSet* satpos, GPSTIME* t,unsigned long prn);
double Klobutchar(BLh* blh, GPSTIME* gpst, IONUTC* ionutc, double E, double A);
double Hopfield(BLh* blh, double E);
int spp(EpochObs* obs, Ephem* ephset, IONUTC* ionutc, PosResult* pos);
int spv(EpochObs* obs, Ephem* ephset, IONUTC* ionutc, PosResult* pos);
#endif