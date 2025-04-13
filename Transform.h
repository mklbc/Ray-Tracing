#ifndef _TRANSFRM_H_
#define _TRANSFRM_H_
#include "sce_types.h"

extern void SetKoordTransform_TRFM(float* M, VERTEX* rv, float x, float y, float z);
extern void TransformKoord_TRFM(float* M, VERTEX* dest, VERTEX* src);
extern void Rotate_TRFM(float* M, VERTEX* v);
extern void MatrixShift_TRFM(float* dest, float* src, float dz);
extern void MatrixAxB_TRFM(float* dest, float* A, float* B);

#endif
