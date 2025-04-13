#include <math.h>
#include <memory.h>
#include "sce_types.h"

static float tmp[12];

void SetKoordTransform_TRFM(float* M, VERTEX* rv, float x, float y, float z)
{
    float R, r;
    float csA, snA, csB, snB;

 
    for (int i = 0; i < 16; i++) M[i] = 0;
    M[0] = M[5] = M[10] = M[15] = 1;
    if ((rv->x == 0) && (rv->y == 0))   
    {
        if (rv->z <= 0)
        {
            M[11] = -z;
            return;
        }
        else
        {
            M[5] = M[10] = -1;
            M[11] = z;
            return;
        }
    }
    R = sqrt(rv->x * rv->x + rv->y * rv->y + rv->z * rv->z);
    r = sqrt(rv->x * rv->x + rv->y * rv->y);
    csB = -rv->z / R;
    snB = r / R;
    csA = -rv->y / r;
    snA = -rv->x / r;
    M[0] = csA;
    M[1] = -snA;
    M[2] = 0;
    M[3] = -M[0] * x - M[1] * y;
    M[4] = snA * csB;
    M[5] = csA * csB;
    M[6] = -snB;
    M[7] = -M[4] * x - M[5] * y - M[6] * z;
    M[8] = snA * snB;
    M[9] = csA * snB;
    M[10] = csB;
    M[11] = -M[8] * x - M[9] * y - M[10] * z;
}
void TransformKoord_TRFM(float* M, VERTEX* dest, VERTEX* src)
{
	VERTEX tmp;

	tmp.x = M[0] * src->x + M[1] * src->y + M[2] * src->z + M[3];
	tmp.y = M[4] * src->x + M[5] * src->y + M[6] * src->z + M[7];
	tmp.z = M[8] * src->x + M[9] * src->y + M[10] * src->z + M[11];
	*dest = tmp;
}
void Rotate_TRFM(float* M, VERTEX* v)
{
	VERTEX tmp;

	tmp.x = M[0] * v->x + M[1] * v->y + M[2] * v->z;
	tmp.y = M[4] * v->x + M[5] * v->y + M[6] * v->z;
	tmp.z = M[8] * v->x + M[9] * v->y + M[10] * v->z;
	*v = tmp;
}
void MatrixShift_TRFM(float* dest, float* src, float dz)
{
    memcpy(dest, src, 16 * sizeof(float));
    dest[11] -= dz;
}
void MatrixAxB_TRFM(float* dest, float* A, float* B)
{
    int indx, i, j, k;

    for (indx = 0; indx < 12; indx++)
    {
        i = indx & 0xfc;
        j = indx & 3;
        tmp[indx] = 0;
        for (k = 0; k < 4; k++)
        {
            tmp[indx] += A[i] * B[j];
            i++;
            j += 4;
        }
    }
    memcpy(dest, tmp, 12 * sizeof(float));
    dest[12] = dest[13] = dest[14] = 0; dest[15] = 1;
}