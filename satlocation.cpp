#include<math.h>
#include"decode.h"
double PI = 3.1415926535898;
double c = 2.99792458e8;
double miu = 3.986004415e14;
double Omega_e = 7.2921151467e-5;
/***************************
Sat

���ܣ��Ӷ�����OEMSTAR�����ļ�����ȡ���ݲ����룬�õ�GNSS�۲�ֵ���㲥�����ͽ��ջ�
	  ��λ��������浽raw�ṹ�塣
	  �ο�OEMSTAR�����ĵ�

���������fp
		  raw     �۲����ݡ��������ݵ�
����ֵ��0=�ļ�������1=�۲����ݣ�2=�����Ͷ�λ�����3=CRC���鲻ͨ����4=û��֧�����
****************************/
int SatPos(eph_t* eph)
{
	//������������
	double A = eph->A;
	//����ƽ���˶����ٶ�
	double n0 = sqrt(miu / pow(A, 3));
	//��������������ο���Ԫ��ʱ��
	double t_k = eph->t - eph->toe;

	return 0;

}