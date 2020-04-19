#include<stdio.h>
#include<math.h>
/****************************************************************************
  VectorAdd

  Ŀ�ģ������ӷ�

  ��ţ�01001

  ����:
  size_a    ��������A��Ԫ�ظ���
  size_b    ��������B��Ԫ�ظ���
  A         ��������
  B         ��������
  C         ������� C=A+B
  ����ֵ��1=������0=�޷����

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

  Ŀ�ģ���������

  ��ţ�01002

  ����:
  size_a    ��������A��Ԫ�ظ���
  size_b    ��������B��Ԫ�ظ���
  A         ��������
  B         ��������
  C         ������� C=A-B
  ����ֵ��1=������0=�޷����

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

  Ŀ�ģ��������

  ��ţ�01003

  ����:
  size_a    ��������A��Ԫ�ظ���
  size_b    ��������B��Ԫ�ظ���
  A         ��������
  B         ��������
  C         ���������������ֻ��һ��Ԫ�� CΪA��B�ĵ��
  ����ֵ��1=������0=�޷�����

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

  Ŀ�ģ���ά�������

  ��ţ�01004

  ����:
  size_a    ��������A��Ԫ�ظ�����������ֻ֧��size_a=3
  size_b    ��������B��Ԫ�ظ�����������ֻ֧��size_a=3
  A         ��������
  B         ��������
  C         ������� CΪA��B�Ĳ��
  ����ֵ��1=������0=�޷�����

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

  Ŀ�ģ��������

  ��ţ�01011

  ����:
  a1,a2     �������A��ά��
  b1,b2     �������B��ά��
  A         �������
  B         �������
  C         ������� C=A+B
  ����ֵ��1=������0=�޷����

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

  Ŀ�ģ��������

  ��ţ�01012

  ����:
  a1,a2     �������A��ά��
  b1,b2     �������B��ά��
  A         �������
  B         �������
  C         ������� C=A-B
  ����ֵ��1=������0=�޷����

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

  Ŀ�ģ��������

  ��ţ�01013

  ����:
  a1,a2     �������A��ά��
  b1,b2     �������B��ά��
  A         �������
  B         �������
  C         ������� C=A*B
  ����ֵ��1=������0=�޷����

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

  Ŀ�ģ��������棬����ȫѡ��Ԫ��˹-Լ����

  ��ţ�01014

  ����:
  n         �������a��ά��
  a         �������
  b         ������� b=a^(-1)
  ����ֵ��1=������0=�޷�����

****************************************************************************/
int MatrixInv(int n, double a[], double b[])
{
    int i, j, k, l, u, v, is[20], js[20];   /* matrix dimension <= 10 */
    double d, p;

    /* ���������ֵ���������b�������b�������棬a���󲻱� */
    //CopyArray(n * n, b, a);
    for (i = 0; i < n * n; i++)
    {
        b[i] = a[i];
    }
    for (k = 0; k < n; k++)
    {
        d = 0.0;
        for (i = k; i < n; i++)   /* �������½Ƿ�������Ԫ�ص�λ�� */
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

        if (is[k] != k)  /* ����Ԫ�����ڵ��������½Ƿ�������н��е��� */
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

        if (js[k] != k)  /* ����Ԫ�����ڵ��������½Ƿ�������н��е��� */
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
        b[l] = 1.0 / b[l];  /* �����б任 */
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

    for (k = n - 1; k >= 0; k--)  /* ����������е������»ָ� */
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

  Ŀ�ģ�����ת��

  ��ţ�01015

  ����:
  a1,a2     �������A��ά��
  A         �������
  B         ������� B=Transcope(A)
  ����ֵ��1=������0=�޷�ת��

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