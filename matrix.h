#pragma once
#ifndef MATRIX_H
#define MATRIX_H
#endif
int VectorAdd(int size_a, int size_b, double A[], double B[], double C[]);//向量加法
int VectorMinus(int size_a, int size_b, double A[], double B[], double C[]);//向量减法
int VectorDot(int size_a,int size_b, double A[], double B[], double C[]);//向量点乘
int VectorCross(int size_a,int size_b, double A[], double B[], double C[]);//向量叉乘
int MatrixAdd(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//矩阵加法
int MatrixMinus(int a1, int a2, int b1, int b2, double A[], double b[], double C[]);//矩阵减法
int MatrixMultiply(int a1, int a2, int b1, int b2, double A[], double B[], double C[]);//矩阵乘法
int MatrixInv(int n, double a[], double b[]);//矩阵求逆
int MatrixTrans(int a1, int a2, double A[], double B[]);//矩阵转置