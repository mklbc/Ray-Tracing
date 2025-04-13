#ifndef _SCE_NORMAL_H_
#define _SCE_NORMAL_H_
#include "sce_types.h"

void NormalVector_SN(VERTEX* nv, VERTEX v1, VERTEX v2, VERTEX v3);
void Normalize_SN(VERTEX* v);
void NearestVector_SN(VERTEX* nv, VERTEX dir);

#endif
