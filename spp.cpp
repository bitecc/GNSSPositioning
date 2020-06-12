#include<math.h>
#include<string.h>
#include"matrix.h"
#include"transformation.h"
#include"decode.h"
#include "spp.h"

double PI = 3.1415926535898;
double c = 2.99792458e8;
double miu = 3.986004415e14;
double Omega_e = 7.2921151467e-5;
double f_L1 = 1575.42e6;
/***************************
SatPosition

���ܣ��������ǹ㲥�������������ǵ�λ��

���������	eph		�������ǵĹ㲥����
			satpos	�������λ�õ�����
			t		��ǰ��Ԫ��ʱ��
			prn		������
����ֵ��0=����ʧ��
****************************/
int SatPosition(Ephem* Eph, SatPosSet* satpos, GPSTIME* t,unsigned long prn)
{
	eph_t* eph = &Eph->eph[prn - 1];
	//��������������ο���Ԫ��ʱ��
	double t_k = (double)(t->Week - eph->Week) * 7 * 86400.0 + t->SecOfWeek - eph->toe;
	if (fabs(t_k) > 7200)
	{
		return 0;//��������
	}
	double A = eph->A;
	//����ƽ���˶����ٶ�
	double n0 = sqrt(miu / pow(A, 3));
	//��ƽ���˶����ٶȽ��и���
	double n = n0 + eph->deltaN;
	//����ƽ�����
	double M_k = eph->M0 + n * t_k;
	//����ƫ�����
	double e = eph->ecc;
	double E0 = 0.02;
	int flag = 0;
	double E_k = M_k + e * sin(E0);
	while (fabs(E_k - E0) > 1e-10 && flag<30)
	{
		E0 = E_k;
		E_k = M_k + e * sin(E0);
		flag++;
	}
	//����������
	double v_k = atan2(sqrt(1 - e * e) * sin(E_k), cos(E_k) - e);
	//���������Ǿ�
	double Phi_k = v_k + eph->omega;
	//������׵��͸�����
	double du_k = eph->cus * sin(2 * Phi_k) + eph->cuc * cos(2 * Phi_k);
	double dr_k = eph->crs * sin(2 * Phi_k) + eph->crc * cos(2 * Phi_k);
	double di_k = eph->cis * sin(2 * Phi_k) + eph->cic * cos(2 * Phi_k);
	//���㾭�������������Ǿࡢ�򾶡�������
	double u_k = Phi_k + du_k;
	double r_k = A * (1 - e * cos(E_k)) + dr_k;
	double i_k = eph->I0 + di_k + eph->Idot * t_k;
	//���������ڹ��ƽ���ϵ�λ��
	double x_k = r_k * cos(u_k);
	double y_k = r_k * sin(u_k);
	//���������������㾭��
	double Omega_k = eph->omega0 + (eph->omegadot - Omega_e) * t_k - Omega_e * eph->toe;
	//�����ڵع�����ϵ�µ�λ��
	double x = x_k * cos(Omega_k) - y_k * cos(i_k) * sin(Omega_k);
	double y = x_k * sin(Omega_k) + y_k * cos(i_k) * cos(Omega_k);
	double z = y_k * sin(i_k);

	//�����Ӳ����
	double F = -4.442807633e-10;
	double dtr = F * e * sqrt(A) * sin(E_k);
	double dtoc= (double)(t->Week - eph->Week) * 7 * 86400.0 + t->SecOfWeek - eph->toc;
	double dtsv = eph->af0 + eph->af1 * dtoc + eph->af2 * pow(dtoc, 2) + dtr - eph->tgd;

	//�����˶��ٶȼ���
	double E_kdot = n / (1 - e * cos(E_k));
	double Phi_kdot = sqrt((1 + e) / (1 - e)) * pow(cos(v_k / 2), 2) / pow(cos(E_k / 2), 2) * E_kdot;
	double u_kdot = 2 * (eph->cus * cos(2 * Phi_k) - eph->cuc * sin(2 * Phi_k)) * Phi_kdot + Phi_kdot;
	double r_kdot = A * e * sin(E_k) * E_kdot + 2 * (eph->crs * cos(2 * Phi_k) - eph->crc * sin(2 * Phi_k)) * Phi_kdot;
	double I_kdot = eph->Idot + 2 * (eph->cis * cos(2 * Phi_k) - eph->cic * sin(2 * Phi_k)) * Phi_kdot;
	double Omega_kdot = eph->omegadot - Omega_e;
	double Rdot[12] = { cos(Omega_k),	-sin(Omega_k) * cos(i_k),	x_k * sin(Omega_k) - y_k * cos(Omega_k) * cos(i_k),	y_k * sin(Omega_k) * sin(i_k),
						sin(Omega_k),	cos(Omega_k) * cos(i_k),	x_k * cos(Omega_k) - y_k * sin(Omega_k) * cos(i_k),	y_k * cos(Omega_k) * sin(i_k),
						0,				sin(i_k),					0,														y_k * cos(i_k) };
	double x_kdot = r_kdot * cos(u_k) - r_k * u_kdot * sin(u_k);
	double y_kdot = r_kdot * sin(u_k) + r_k * u_kdot * cos(u_k);
	double tmp[4] = { x_kdot,y_kdot, Omega_kdot,I_kdot };
	double Vdot[3];
	MatrixMultiply(3, 4, 4, 1, Rdot, tmp, Vdot);

	//�������ٸ���
	double dtrd = F * e * sqrt(A) * cos(E_k) * E_kdot;
	double dtsvd = eph->af1 + 2 * eph->af2 * dtoc + dtrd;

	satpos->satPos[prn - 1].x = x;
	satpos->satPos[prn - 1].y = y;
	satpos->satPos[prn - 1].z = z;
	satpos->satPos[prn - 1].vx = Vdot[0];
	satpos->satPos[prn - 1].vy = Vdot[1];
	satpos->satPos[prn - 1].vz = Vdot[2];
	satpos->satPos[prn - 1].clk = dtsv;
	satpos->satPos[prn - 1].clkd = dtsvd;
	return 1;
}


/***************************
Klobutchar

���ܣ�������ӳٸ���

���������	blh		��վ������λ�ã��Ƕ���
			gpst	��ǰ��Ԫ��ʱ��
			ionutc	��������
			E		�߶Ƚǣ��Ƕ�
			A		��λ�ǣ��Ƕ�
����ֵ��0=�޷����㣬��������ֵ��Ϊ������
****************************/
double Klobutchar(BLh* blh,GPSTIME* gpst,IONUTC* ionutc,double E,double A)
{
	if (blh->H > 50000)
	{
		printf("��λ�㳬������������޷�������\n");
		return 0;
	}
	if(ionutc->b0==0)//��ʱ��û�е���������ʹ��Ĭ��
	{
		ionutc->a0 = 0.1118E-07;
		ionutc->a1 = -0.7451E-08;
		ionutc->a2 = -0.5961E-07;
		ionutc->a3 = 0.1192E-06;
		ionutc->b0 = 0.1167E+06;
		ionutc->b1 = -0.2294E+06;
		ionutc->b2 = -0.1311E+06;
		ionutc->b3 = 0.1049E+07;
	}
	double psi = 0.0137 / (E / 180 + 0.11) - 0.022;
	double phi = blh->B / 180 + psi * cos(A);
	if (phi > 0.416)
		phi = 0.416;
	if (phi < -0.416)
		phi = -0.416;
	double lambda = blh->L / 180 + psi * sin(A) / cos(phi * PI);
	phi = phi + 0.064 * cos((lambda - 1.617) * PI);
	double t = 43200 * lambda + gpst->SecOfWeek;
	t -= floor(t / 86400.0) * 86400;
	double amp = ionutc->a0 + phi * (ionutc->a1 + phi * (ionutc->a2 + phi * ionutc->a3));
	double per = ionutc->b0 + phi * (ionutc->b1 + phi * (ionutc->b2 + phi * ionutc->b3));
	amp = amp < 0.0 ? 0.0 : amp;
	per = per < 72000.0 ? 72000.0 : per;
	double X = 2.0 * PI * (t - 50400) / per;
	double F = 1.0 + 16 * pow(0.53 - E / 180, 3);
	double I = F * (fabs(X) < 1.57 ? 5e-9 + amp * (1.0 + X * X * (-0.5 + X * X / 24.0)) : 5e-9);
	return I;
}

/***************************
Hopfield

���ܣ��������ӳٸ���

���������	blh		��վ������λ�ã��Ƕ���
			E		�߶Ƚǣ��Ƕ�
����ֵ��0=�޷����㣬��������ֵ��Ϊ������
****************************/
double Hopfield(BLh* blh, double E)
{
	if (blh->H < -35000 || blh->H>18000)
		return 0;
	double H0 = 0, T0 = 288.16, P0 = 1013.25, RH0 = 0.5;
	double T, p, RH, e, hd, Kw, Kd;
	double hw = 11000;
	T = T0 - 0.0065 * (blh->H - H0) + 273.16;
	p = P0 * pow(1 - 0.0000266 * (blh->H - H0), 5.225);
	RH = RH0 * exp(-0.0006396 * (blh->H - H0));
	e = RH * exp(-37.2465 + 0.213166 * T - 0.000256908 * T * T);
	hd = 40136 + 148.72 * (T - 273.16);
	Kd = 155.2E-7 * p / T * (hd - blh->H);
	Kw = 155.2E-7 * 4810 / T / T * e * (hw - blh->H);
	double dtrop = Kd / sin(pow(pow(E, 2) + 6.25, 0.5) * PI / 180) + Kw / sin(pow(pow(E, 2) + 2.25, 0.5) * PI / 180);
	return dtrop;
}


int spp(EpochObs* obs,Ephem* ephset,IONUTC* ionutc,PosResult* pos)
{
	pos->Time = obs->Time;
	double D2R = PI / 180;
	double R2D = 180 / PI;
	int num;
	double M;
	BLh blh;//�������
	blh = pos->blh;
	XYZ xyz;
	double cdt=pos->cdt;
	int flag = 0;//������������
	double w[NSAT] = { 0 };
	double B[NSAT * 4] = {0};
	double B1[NSAT * 4] = { 0 };
	double B2[16] = { 0 };
	double B3[16] = {0};
	double B4[NSAT * 4] = {0};
	double B5[NSAT] = {0};
	double Result[4] = { 0 };
	double v[NSAT] = { 0 };
	SatPosSet sat;
	do {
		num = 0;//ͳ�����ڼ����������
		M = 0;
		memset(w, 0, sizeof(w));
		memset(Result, 0, sizeof(Result));
		memset(B, 0, sizeof(B));
		memset(B1, 0, sizeof(B1));
		memset(B2, 0, sizeof(B2));
		memset(B3, 0, sizeof(B3));
		memset(B4, 0, sizeof(B4));
		memset(v, 0, sizeof(v));
		BLh2XYZ(&blh, &xyz);
		for (int i = 0; i < obs->SatNum; i++)
		{
			unsigned long prn = obs->Obs[i].prn;
			GPSTIME t = obs->Time;
			if (ephset->eph[prn - 1].PRN != 0)//��Ӧ����prn����������
			{
				//�����źŷ���ʱ��
				t.SecOfWeek = t.SecOfWeek - obs->Obs[i].Psr / c;
				//printf("%f\n", obs->Obs[i].Psr/c);
				if (SatPosition(ephset, &sat, &t, prn))
				{
					double detpos[3] = { sat.satPos[prn - 1].x - xyz.X,
										sat.satPos[prn - 1].y - xyz.Y,
										sat.satPos[prn - 1].z - xyz.Z };
					double trans_H[9] = { -sin(blh.B * D2R) * cos(blh.L * D2R),-sin(blh.B * D2R) * sin(blh.L * D2R),cos(blh.L * D2R),
										-sin(blh.L * D2R),cos(blh.L * D2R),0,
										cos(blh.B * D2R) * cos(blh.L * D2R),cos(blh.B * D2R) * sin(blh.L * D2R),sin(blh.L * D2R) };
					double sta_cen_coor[3];
					MatrixMultiply(3, 3, 3, 1, trans_H, detpos, sta_cen_coor);
					double azimuth = R2D * atan2(sta_cen_coor[1], sta_cen_coor[0]);
					double ele = fabs(R2D * atan(sta_cen_coor[2] / sqrt(pow(sta_cen_coor[0], 2) + pow(sta_cen_coor[1], 2))));
					if (ele < 15)//�߶Ƚ�С��15�ȵĲ�Ҫ
						continue;
					
					double rho = sqrt(pow(sat.satPos[prn - 1].x - xyz.X, 2) + pow(sat.satPos[prn - 1].y - xyz.Y, 2) + pow(sat.satPos[prn - 1].z - xyz.Z, 2));
					double dion = c * Klobutchar(&blh, &t, ionutc, ele, azimuth);
					double dtrop = Hopfield(&blh, ele);
					w[num] = obs->Obs[i].Psr - (rho + cdt- c * sat.satPos[prn - 1].clk + dion + dtrop);
					B[4 * num] = (xyz.X - sat.satPos[prn - 1].x)/rho;
					B[4 * num+1] = (xyz.Y - sat.satPos[prn - 1].y)/rho;
					B[4 * num+2] = (xyz.Z - sat.satPos[prn - 1].z)/rho;
					B[4 * num+3] = 1;
					num++;
				}
			}
		}
		if (num < 4)
		{
			printf("����������4��\n");
			return 0;
		}
		MatrixTrans(num,4,B,B1);
		MatrixMultiply(4, num, num, 4, B1, B, B2);
		MatrixInv(4,B2,B3);
		MatrixMultiply(4, 4, 4, num, B3, B1, B4);
		MatrixMultiply(4, num, num, 1, B4, w, Result);
		xyz.X = Result[0] + xyz.X;
		xyz.Y = Result[1] + xyz.Y;
		xyz.Z = Result[2] + xyz.Z;
		cdt = Result[3] + cdt;
		for (int i = 0; i < 4; i++)
		{
			if (M < Result[i])
				M = Result[i];
		}
		flag++;
	} while (M > 10e-10 && flag < 15);
	MatrixMultiply(num, 4, 4, 1, B, Result, B5);
	MatrixMinus(num, 1,num,1, B5, w, v);
	double vv[1];
	MatrixMultiply(1, num, num, 1, v, v, vv);
	double sigma0 = sqrt(vv[0] / (num - 4));
	XYZ2BLh(&xyz, &blh);
	pos->blh = blh;
	pos->x = xyz.X;
	pos->y = xyz.Y;
	pos->z = xyz.Z;
	pos->cdt = cdt;
	pos->sigma0 = sigma0;
	return 1;
}

int spv(EpochObs* obs, Ephem* ephset, IONUTC* ionutc, PosResult* pos)
{
	if (pos->x == 0)
		return 0;
	SatPosSet sat;
	double w[NSAT] = { 0 };
	double H[NSAT * 4] = { 0 };
	double H1[NSAT * 4] = { 0 };
	double H2[16] = { 0 };
	double H3[16] = { 0 };
	double H4[NSAT * 4] = { 0 };
	double H5[NSAT] = { 0 };
	double v[NSAT] = { 0 };
	double Result[4] = { 0 };
	int num = 0;
	for (int i = 0; i < obs->SatNum; i++)
	{
		unsigned long prn = obs->Obs[i].prn;
		GPSTIME t = obs->Time;
		if (ephset->eph[prn - 1].PRN != 0)
		{
			t.SecOfWeek = t.SecOfWeek - obs->Obs[i].Psr / c;
			if (SatPosition(ephset, &sat, &t, prn))
			{
				double rho = sqrt(pow(sat.satPos[prn - 1].x - pos->x, 2) 
								+ pow(sat.satPos[prn - 1].y - pos->y, 2) 
								+ pow(sat.satPos[prn - 1].z - pos->z, 2));
				H[4 * num] = (pos->x - sat.satPos[prn - 1].x) / rho;
				H[4 * num+1] = (pos->y - sat.satPos[prn - 1].y) / rho;
				H[4 * num+2] = (pos->z - sat.satPos[prn - 1].z) / rho;
				H[4 * num+3] = 1;
				double rhodot=((sat.satPos[prn-1].x-pos->x)*sat.satPos[prn-1].vx
							+ (sat.satPos[prn - 1].y - pos->y) * sat.satPos[prn - 1].vy
							+ (sat.satPos[prn - 1].z - pos->z) * sat.satPos[prn - 1].vz)/rho;
				w[num] = -obs->Obs[i].dopp * c / f_L1 - (rhodot - sat.satPos[prn - 1].clkd * c);
				num++;
			}
		}
	}
	MatrixTrans(num, 4, H, H1);
	MatrixMultiply(4, num, num, 4, H1, H, H2);
	MatrixInv(4, H2, H3);
	MatrixMultiply(4, 4, 4, num, H3, H1, H4);
	MatrixMultiply(4, num, num, 1, H4, w, Result);
	pos->vx = Result[0];
	pos->vy = Result[1];
	pos->vz = Result[3];
	pos->vcdt = Result[4];
	MatrixMultiply(num, 4, 4, 1, H, Result, H5);
	MatrixMinus(32, 1, 32, 1, H5, w, v);
	double vv[1] = { 0 };
	MatrixMultiply(1, 32, 32, 1, v, v, vv);
	pos->vsigma0 = sqrt(vv[0] / (num - 4));
	return 1;
}