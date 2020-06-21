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

功能：根据卫星广播星历，计算卫星的位置、速度、钟差和钟速

输入参数：	Eph		所有卫星的广播星历
			satpos	存放卫星位置的数组
			t		当前历元的时间
			prn		卫星数
			psr		伪距
			xyz		测站坐标
返回值：0=计算失败
****************************/
int SatPosition(Ephem* Eph, SatPosSet* satpos, GPSTIME* t,unsigned long prn,double psr,XYZ* xyz)
{
	eph_t* eph = &Eph->eph[prn - 1];//所选卫星的星历
	//计算粗略发射时刻
	double t_tr = t->SecOfWeek - psr / c;
	//计算相对于星历参考历元的时间
	double t_k = (double)(t->Week - eph->Week) * 7 * 86400.0 + t_tr - eph->toe;
	if (fabs(t_k) > 7200)
	{
		return 0;//星历过期
	}
	
	double e = eph->ecc;
	double F = -4.442807633e-10;
	double A = eph->A;
	//计算平均运动角速度
	double n0 = sqrt(miu / pow(A, 3));
	//对平均运动角速度进行改正
	double n = n0 + eph->deltaN;
	double M_k, E0, E_k, dtr, dtoc, dtsv,rho,v_k,Phi_k;
	double du_k, dr_k, di_k, u_k, r_k, i_k, x, y, z;
	double Omega_k, x_k, y_k;
	double t_trold = t_tr;
	int loopn = 0;
	//迭代计算卫星位置和钟差
	do{
		// 计算平近点角
		M_k = eph->M0 + n * t_k;
		//计算偏近点角
		E0 = 0.02;
		int flag = 0;
		E_k = M_k + e * sin(E0);
		while (fabs(E_k - E0) > 1e-10 && flag < 30)
		{
			E0 = E_k;
			E_k = M_k + e * sin(E0);
			flag++;
		}
		//计算真近点角
		v_k = atan2(sqrt(1 - e * e) * sin(E_k), cos(E_k) - e);
		//计算升交角距
		Phi_k = v_k + eph->omega;
		//计算二阶调和改正数
		du_k = eph->cus * sin(2 * Phi_k) + eph->cuc * cos(2 * Phi_k);
		dr_k = eph->crs * sin(2 * Phi_k) + eph->crc * cos(2 * Phi_k);
		di_k = eph->cis * sin(2 * Phi_k) + eph->cic * cos(2 * Phi_k);
		//计算经过改正的升交角距、向径、轨道倾角
		u_k = Phi_k + du_k;
		r_k = A * (1 - e * cos(E_k)) + dr_k;
		i_k = eph->I0 + di_k + eph->Idot * t_k;
		//计算卫星在轨道平面上的位置
		x_k = r_k * cos(u_k);
		y_k = r_k * sin(u_k);
		//计算改正后的升交点经度
		Omega_k = eph->omega0 + (eph->omegadot - Omega_e) * t_k - Omega_e * eph->toe;
		//计算在地固坐标系下的位置
		x = x_k * cos(Omega_k) - y_k * cos(i_k) * sin(Omega_k);
		y = x_k * sin(Omega_k) + y_k * cos(i_k) * cos(Omega_k);
		z = y_k * sin(i_k);
		
		//钟差计算
		dtr = F * e * sqrt(A) * sin(E_k);
		dtoc = (double)(t->Week - eph->Week) * 7 * 86400.0 + t->SecOfWeek - eph->toc;
		dtsv = eph->af0 + eph->af1 * dtoc + eph->af2 * pow(dtoc, 2) + dtr - eph->tgd;
		rho = sqrt(pow(x - xyz->X, 2) + pow(y - xyz->Y, 2) + pow(z - xyz->Z, 2));

		t_tr = t->SecOfWeek -rho/c- dtsv;
		if (fabs(t_tr - t_trold) < 1e-6)
			break;
		else
			t_trold = t_tr;
		t_k = (double)(t->Week - eph->Week) * 7 * 86400.0 + t_tr - eph->toe;
		loopn++;
	} while (loopn < 15);
	
	//卫星运动速度计算
	double E_kdot = n / (1 - e * cos(E_k));
	double Phi_kdot = sqrt((1 + e) / (1 - e)) * pow(cos(v_k / 2), 2) / pow(cos(E_k / 2), 2) * E_kdot;
	double u_kdot = 2 * (eph->cus * cos(2 * Phi_k) - eph->cuc * sin(2 * Phi_k)) * Phi_kdot + Phi_kdot;
	double r_kdot = A * e * sin(E_k) * E_kdot + 2 * (eph->crs * cos(2 * Phi_k) - eph->crc * sin(2 * Phi_k)) * Phi_kdot;
	double I_kdot = eph->Idot + 2 * (eph->cis * cos(2 * Phi_k) - eph->cic * sin(2 * Phi_k)) * Phi_kdot;
	double Omega_kdot = eph->omegadot - Omega_e;
	double x_kdot = r_kdot * cos(u_k) - r_k * u_kdot * sin(u_k);
	double y_kdot = r_kdot * sin(u_k) + r_k * u_kdot * cos(u_k);
	double Rdot[12] = { cos(Omega_k),	-sin(Omega_k) * cos(i_k),	x_k * sin(Omega_k) - y_k * cos(Omega_k) * cos(i_k),	y_k * sin(Omega_k) * sin(i_k),
						sin(Omega_k),	cos(Omega_k) * cos(i_k),	x_k * cos(Omega_k) - y_k * sin(Omega_k) * cos(i_k),	y_k * cos(Omega_k) * sin(i_k),
						0,				sin(i_k),					0,														y_k * cos(i_k) };
	double tmp[4] = { x_kdot,y_kdot, Omega_kdot,I_kdot };
	double Vdot[3] = {0};
	MatrixMultiply(3, 4, 4, 1, Rdot, tmp, Vdot);
	
	//卫星钟速改正
	double dtrd = F * e * sqrt(A) * cos(E_k) * E_kdot;
	double dtsvd = eph->af1 + 2 * eph->af2 * dtoc + dtrd;

	//地球自转改正
	rho = sqrt(pow(x - xyz->X, 2) + pow(y - xyz->Y, 2) + pow(z - xyz->Z, 2));
	double omegatao = Omega_e * rho / c;
	double originpos[3] = { x,y,z };
	double trans_corr[9] = { cos(omegatao),sin(omegatao),0,-sin(omegatao),cos(omegatao),0,0,0,1 };
	double corrresult[3];

	MatrixMultiply(3, 3, 3, 1, trans_corr, originpos, corrresult);
	satpos->satpos[prn-1].x = corrresult[0];
	satpos->satpos[prn - 1].y = corrresult[1];
	satpos->satpos[prn - 1].z = corrresult[2];
	satpos->satpos[prn - 1].vx = Vdot[0];
	satpos->satpos[prn - 1].vy = Vdot[1];
	satpos->satpos[prn - 1].vz = Vdot[2];
	satpos->satpos[prn - 1].clk = dtsv;
	satpos->satpos[prn - 1].clkd = dtsvd;
	return 1;
}


/***************************
Klobutchar

功能：电离层延迟改正

输入参数：	blh		测站的坐标位置，角度制
			gpst	当前历元的时间
			ionutc	电离层参数
			E		高度角，角度
			A		方位角，角度
返回值：0=无法计算，其他返回值即为改正数
****************************/
double Klobutchar(BLh* blh,GPSTIME* gpst,IONUTC* ionutc,double E,double A)
{
	if (blh->H > 50000)
	{
		printf("定位点超过电离层区域，无法改正。\n");
		return 0;
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

功能：对流层延迟改正

输入参数：	blh		测站的坐标位置，角度制
			E		高度角，角度
返回值：0=无法计算，其他返回值即为改正数
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

/***************************
spp

功能：SPP单点定位和SPV单点测速

输入参数：	obs		观测值文件
			ephset	星历文件
			ionutc	电离层改正参数，用于电离层改正
			pos		定位结果文件（后续历元以上一历元的结果为输入值）
返回值：0=定位失败，1=定位成功
****************************/
int spp(EpochObs* obs,Ephem* ephset,IONUTC* ionutc,PosResult* pos)
{
	pos->Time = obs->Time;
	double D2R = PI / 180;
	double R2D = 180 / PI;
	int num;
	double M;
	BLh blh;//大地坐标
	blh = pos->blh;
	XYZ xyz;
	double cdt=pos->cdt;
	int flag = 0;//迭代次数限制
	double w[NSAT] = { 0 };
	double B[NSAT * 4] = {0};
	double B1[NSAT * 4] = { 0 };
	double B2[16] = { 0 };
	double B3[16] = {0};
	double B4[NSAT * 4] = {0};
	double B5[NSAT] = {0};
	double Result[4] = { 0 };
	double v[NSAT] = { 0 };
	int satUsed[NSAT] = { 0 };//存放所用观测值的索引
	SatPosSet satpos;//存放星历
	do {
		num = 0;//统计用于计算的卫星数
		M = 0;
		memset(w, 0, sizeof(w));
		memset(Result, 0, sizeof(Result));
		memset(B, 0, sizeof(B));
		memset(B1, 0, sizeof(B1));
		memset(B2, 0, sizeof(B2));
		memset(B3, 0, sizeof(B3));
		memset(B4, 0, sizeof(B4));
		memset(v, 0, sizeof(v));
		memset(satUsed, 0, sizeof(satUsed));
		BLh2XYZ(&blh, &xyz);
		for (int i = 0; i < obs->SatNum; i++)
		{
			unsigned long prn = obs->Obs[i].prn;
			GPSTIME t = obs->Time;
			if (ephset->eph[prn - 1].PRN != 0)//对应卫星prn的星历存在
			{
				//计算信号发射时刻
				//t.SecOfWeek = t.SecOfWeek - obs->Obs[i].Psr / c;
				//printf("%f\n", obs->Obs[i].Psr/c);
				if (SatPosition(ephset, &satpos, &t, prn,obs->Obs[i].Psr,&xyz))//卫星位置计算失败则跳过
				{
					SatPos sat = satpos.satpos[prn - 1];
					//计算卫星的高度角和方位角
					double detpos[3] = { sat.x - xyz.X,
										sat.y - xyz.Y,
										sat.z - xyz.Z };
					double trans_H[9] = { -sin(blh.B * D2R) * cos(blh.L * D2R),-sin(blh.B * D2R) * sin(blh.L * D2R),cos(blh.L * D2R),
										-sin(blh.L * D2R),cos(blh.L * D2R),0,
										cos(blh.B * D2R) * cos(blh.L * D2R),cos(blh.B * D2R) * sin(blh.L * D2R),sin(blh.L * D2R) };
					double sta_cen_coor[3];
					MatrixMultiply(3, 3, 3, 1, trans_H, detpos, sta_cen_coor);
					double azimuth = R2D * atan2(sta_cen_coor[1], sta_cen_coor[0]);
					double ele = fabs(R2D * atan(sta_cen_coor[2] / sqrt(pow(sta_cen_coor[0], 2) + pow(sta_cen_coor[1], 2))));
					if (ele < 15)//高度角小于15度的不要
						continue;

					double rho = sqrt(pow(sat.x - xyz.X, 2) + pow(sat.y - xyz.Y, 2) + pow(sat.z - xyz.Z, 2));
					double dion = c * Klobutchar(&blh, &t, ionutc, ele, azimuth);
					double dtrop = Hopfield(&blh, ele);
					w[num] = obs->Obs[i].Psr - (rho + cdt- c * sat.clk + dion + dtrop);
					B[4 * num] = (xyz.X - sat.x)/rho;
					B[4 * num+1] = (xyz.Y - sat.y)/rho;
					B[4 * num+2] = (xyz.Z - sat.z)/rho;
					B[4 * num+3] = 1;
					satUsed[num] = i;
					num++;
				}
			}
		}
		if (num < 4)
		{
			printf("卫星数不足4。\n");
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
	double sigma0 = sqrt(vv[0] / ((double)num - 4));
	XYZ2BLh(&xyz, &blh);
	//输出定位结果
	pos->blh = blh;
	pos->x = xyz.X;
	pos->y = xyz.Y;
	pos->z = xyz.Z;
	pos->cdt = cdt;
	pos->sigma0 = sigma0;
	
	//单点测速
	double w_v[NSAT] = { 0 };
	double H[NSAT * 4] = { 0 };
	double H1[NSAT * 4] = { 0 };
	double H2[16] = { 0 };
	double H3[16] = { 0 };
	double H4[NSAT * 4] = { 0 };
	double H5[NSAT] = { 0 };
	double Result_v[4] = { 0 };
	for (int i = 0; i < num; i++)
	{
		//读取对应卫星的定位结果
		unsigned long prn = obs->Obs[satUsed[i]].prn;
		SatPos sat = satpos.satpos[prn - 1];
		double rho = sqrt(pow(sat.x - pos->x, 2) + pow(sat.y - pos->y, 2) + pow(sat.z - pos->z, 2));
		H[4 * i] = (pos->x - sat.x) / rho;
		H[4 * i + 1] = (pos->y - sat.y) / rho;
		H[4 * i + 2] = (pos->z - sat.z) / rho;
		H[4 * i + 3] = 1;
		double rhodot = ((pos->x - sat.x) * sat.vx
			+ (pos->y - sat.y) * sat.vy
			+ (pos->z - sat.z) * sat.vz) / rho;
		w_v[i] = -obs->Obs[satUsed[i]].dopp * c / f_L1 - (rhodot - sat.clkd * c);
	}
	MatrixTrans(num, 4, H, H1);
	MatrixMultiply(4, num, num, 4, H1, H, H2);
	MatrixInv(4, H2, H3);
	MatrixMultiply(4, 4, 4, num, H3, H1, H4);
	MatrixMultiply(4, num, num, 1, H4, w_v, Result_v);
	pos->vx = Result[0];
	pos->vy = Result[1];
	pos->vz = Result[2];
	pos->vcdt = Result[3];
	double v_v[NSAT] = { 0 };
	MatrixMultiply(num, 4, 4, 1, H, Result_v, H5);
	MatrixMinus(num, 1, num, 1, H5, w_v, v_v);
	double vv_v[1] = { 0 };
	MatrixMultiply(1, num, num, 1, v_v, v_v, vv_v);
	pos->vsigma0 = sqrt(vv[0] / ((double)num - 4));
	return 1;
}
/*
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
		GPSTIME t = pos->Time;
		if (ephset->eph[prn - 1].PRN != 0)
		{
			t.SecOfWeek = t.SecOfWeek - obs->Obs[i].Psr / c;
			if (SatPosition(ephset, &sat, &t, prn,obs->Obs[i].Psr))
			{
				double rho = sqrt(pow(sat.satPos[prn - 1].x - pos->x, 2) 
								+ pow(sat.satPos[prn - 1].y - pos->y, 2) 
								+ pow(sat.satPos[prn - 1].z - pos->z, 2));
				H[4 * num] = (pos->x - sat.satPos[prn - 1].x) / rho;
				H[4 * num+1] = (pos->y - sat.satPos[prn - 1].y) / rho;
				H[4 * num+2] = (pos->z - sat.satPos[prn - 1].z) / rho;
				H[4 * num+3] = 1;
				double rhodot = ((sat.satPos[prn - 1].x - pos->x) * sat.satPos[prn - 1].vx
					+ (sat.satPos[prn - 1].y - pos->y) * sat.satPos[prn - 1].vy
					+ (sat.satPos[prn - 1].z - pos->z) * sat.satPos[prn - 1].vz) / rho;
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
	pos->vz = Result[2];
	pos->vcdt = Result[3];
	MatrixMultiply(num, 4, 4, 1, H, Result, H5);
	MatrixMinus(num, 1, num, 1, H5, w, v);
	double vv[1] = { 0 };
	MatrixMultiply(1, num, num, 1, v, v, vv);
	pos->vsigma0 = sqrt(vv[0] / ((double)num - 4));
	return 1;
}*/