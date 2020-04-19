#pragma once
#ifndef MATRIX_H
#define MATRIX_H
#endif
int VectorAdd(int size_a, int size_b, double A[], double B[], double C[]);//�����ӷ�
int VectorMinus(int size_a, int size_b, double A[], double B[], double C[]);//��������
int VectorDot(int size_a,int size_b, double A[], double B[], double C[]);//�������
int VectorCross(int size_a,int size_b, double A[], double B[], double C[]);//�������
int MatrixAdd(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//����ӷ�
int MatrixMinus(int a1, int a2, int b1, int b2, double A[], double b[], double C[]);//�������
int MatrixMultiply(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//����˷�
int MatrixInv(int n, double a[], double b[]);//��������
int MatrixTrans(int a1, int a2, double A[], double B[]);//����ת��