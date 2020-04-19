#include<stdio.h>
#include<math.h>
/****************************************************************************
  VectorAdd

  目的：向量加法

  编号：01001

  参数:
  size_a    输入向量A的元素个数
  size_b    输入向量B的元素个数
  A         输入向量
  B         输入向量
  C         输出向量 C=A+B
  返回值：1=正常，0=无法求和

****************************************************************************/
int VectorAdd(int size_a,int size_b, double A[], double B[], double C[])
{
    if (size_a != size_b)
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    for (int i = 0; i < size_a; i++)
    {
        C[i] = A[i] + B[i];
    }
    return 1;
}

/****************************************************************************
  VectorMinus

  目的：向量减法

  编号：01002

  参数:
  size_a    输入向量A的元素个数
  size_b    输入向量B的元素个数
  A         输入向量
  B         输入向量
  C         输出向量 C=A-B
  返回值：1=正常，0=无法求差

****************************************************************************/
int VectorMinus(int size_a, int size_b, double A[], double B[], double C[])
{
    if (size_a != size_b)
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    for (int i = 0; i < size_a; i++)
    {
        C[i] = A[i] - B[i];
    }
    return 1;
}

/****************************************************************************
  VectorDot

  目的：向量点乘

  编号：01003

  参数:
  size_a    输入向量A的元素个数
  size_b    输入向量B的元素个数
  A         输入向量
  B         输入向量
  C         输出向量，向量中只有一个元素 C为A和B的点积
  返回值：1=正常，0=无法求点积

****************************************************************************/
int VectorDot(int size_a, int size_b, double A[], double B[], double C[])
{
    if (size_a != size_b)
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    C[0]= 0;
    for (int i = 0; i < size_a; i++)
    {
        C[0] += A[i] * B[i];
    }
    return 1;
}

/****************************************************************************
  VectorCross

  目的：三维向量叉乘

  编号：01004

  参数:
  size_a    输入向量A的元素个数，本函数只支持size_a=3
  size_b    输入向量B的元素个数，本函数只支持size_a=3
  A         输入向量
  B         输入向量
  C         输出向量 C为A和B的叉积
  返回值：1=正常，0=无法求叉积

****************************************************************************/
int VectorCross(int size_a, int size_b, double A[], double B[], double C[])
{
    if (size_a ==3 && size_b==3)
    {
        C[0] = A[1] * B[2] - A[2] * B[1];
        C[1] = A[2] * B[0] - A[0] * B[2];
        C[2] = A[0] * B[1] - A[1] * B[0];
    }
    else 
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    return 1;
}

/****************************************************************************
  MatrixAdd

  目的：矩阵相加

  编号：01011

  参数:
  a1,a2     输入矩阵A的维度
  b1,b2     输入矩阵B的维度
  A         输入矩阵
  B         输入矩阵
  C         输出矩阵 C=A+B
  返回值：1=正常，0=无法求和

****************************************************************************/
int MatrixAdd(int a1, int a2, int b1,int b2,double A[], double B[], double C[])
{
    if (a1 == b1 && a2 == b2)
    {
        for (int i = 0; i < a1 * a2; i++)
        {
            C[i] = A[i] + B[i];
        }
    }
    else
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    return 1;
}

/****************************************************************************
  MatrixMinus

  目的：矩阵相减

  编号：01012

  参数:
  a1,a2     输入矩阵A的维度
  b1,b2     输入矩阵B的维度
  A         输入矩阵
  B         输入矩阵
  C         输出矩阵 C=A-B
  返回值：1=正常，0=无法求差

****************************************************************************/
int MatrixMinus(int a1, int a2, int b1, int b2, double A[], double B[], double C[])
{
    if (a1 == b1 && a2 == b2)
    {
        for (int i = 0; i < a1 * a2; i++)
        {
            C[i] = A[i] - B[i];
        }
    }
    else
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    return 1;
}

/****************************************************************************
  MatrixMultiply

  目的：矩阵相乘

  编号：01013

  参数:
  a1,a2     输入矩阵A的维度
  b1,b2     输入矩阵B的维度
  A         输入矩阵
  B         输入矩阵
  C         输出矩阵 C=A*B
  返回值：1=正常，0=无法求积

****************************************************************************/
int MatrixMultiply(int a1, int a2, int b1, int b2, double A[], double B[], double C[])
{
    if (a2 == b1) 
    {
        for (int i = 0; i < a1; i++)
        {
            for (int j = 0; j < b2; j++)
            {
                C[i * b2 + j] = 0;
                for (int s = 0; s < a2; s++)
                {
                    C[i * b2 + j] += A[i * a2 + s] * B[s * b2 + j];
                }
            }
        }
    }
    else
    {
        printf("Vector dimensions are inconsistent!\n");
        return 0;
    }
    return 1;
}

/****************************************************************************
  MatrixInv

  目的：矩阵求逆，采用全选主元高斯-约当法

  编号：01014

  参数:
  n         输入矩阵a的维度
  a         输入矩阵
  b         输出矩阵 b=a^(-1)
  返回值：1=正常，0=无法求逆

****************************************************************************/
int MatrixInv(int n, double a[], double b[])
{
    int i, j, k, l, u, v, is[20], js[20];   /* matrix dimension <= 10 */
    double d, p;

    /* 将输入矩阵赋值给输出矩阵b，下面对b矩阵求逆，a矩阵不变 */
    //CopyArray(n * n, b, a);
    for (i = 0; i < n * n; i++)
    {
        b[i] = a[i];
    }
    for (k = 0; k < n; k++)
    {
        d = 0.0;
        for (i = k; i < n; i++)   /* 查找右下角方阵中主元素的位置 */
        {
            for (j = k; j < n; j++)
            {
                l = n * i + j;
                p = fabs(b[l]);
                if (p > d)
                {
                    d = p;
                    is[k] = i;
                    js[k] = j;
                }
            }
        }

        if (d < 1.0E-12)
        {
            printf("Divided by 0 in MatrixInv!\n");
            return 0;
        }

        if (is[k] != k)  /* 对主元素所在的行与右下角方阵的首行进行调换 */
        {
            for (j = 0; j < n; j++)
            {
                u = k * n + j;
                v = is[k] * n + j;
                p = b[u];
                b[u] = b[v];
                b[v] = p;
            }
        }

        if (js[k] != k)  /* 对主元素所在的列与右下角方阵的首列进行调换 */
        {
            for (i = 0; i < n; i++)
            {
                u = i * n + k;
                v = i * n + js[k];
                p = b[u];
                b[u] = b[v];
                b[v] = p;
            }
        }

        l = k * n + k;
        b[l] = 1.0 / b[l];  /* 初等行变换 */
        for (j = 0; j < n; j++)
        {
            if (j != k)
            {
                u = k * n + j;
                b[u] = b[u] * b[l];
            }
        }
        for (i = 0; i < n; i++)
        {
            if (i != k)
            {
                for (j = 0; j < n; j++)
                {
                    if (j != k)
                    {
                        u = i * n + j;
                        b[u] = b[u] - b[i * n + k] * b[k * n + j];
                    }
                }
            }
        }
        for (i = 0; i < n; i++)
        {
            if (i != k)
            {
                u = i * n + k;
                b[u] = -b[u] * b[l];
            }
        }
    }

    for (k = n - 1; k >= 0; k--)  /* 将上面的行列调换重新恢复 */
    {
        if (js[k] != k)
        {
            for (j = 0; j < n; j++)
            {
                u = k * n + j;
                v = js[k] * n + j;
                p = b[u];
                b[u] = b[v];
                b[v] = p;
            }
        }
        if (is[k] != k)
        {
            for (i = 0; i < n; i++)
            {
                u = i * n + k;
                v = is[k] + i * n;
                p = b[u];
                b[u] = b[v];
                b[v] = p;
            }
        }
    }

    return 1;
}

/****************************************************************************
  MatrixTrans

  目的：矩阵转置

  编号：01015

  参数:
  a1,a2     输入矩阵A的维度
  A         输入矩阵
  B         输出矩阵 B=Transcope(A)
  返回值：1=正常，0=无法转置

****************************************************************************/
int MatrixTrans(int a1, int a2, double A[], double B[])
{
    for (int i = 0; i < a1; i++)
    {
        for (int j = 0; j < a2; j++)
        {
            B[j * a1 + i] = A[i * a2 + j];
        }
    }
    return 1;
}