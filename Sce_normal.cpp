
#include <math.h>
#include "sce_types.h"
#include "sce_normal.h"

void NormalVector_SN(VERTEX* nv, VERTEX v1, VERTEX v2, VERTEX v3)
{
    v2.x -= v1.x;
    v2.y -= v1.y;
    v2.z -= v1.z;
    v3.x -= v1.x;
    v3.y -= v1.y;
    v3.z -= v1.z;
    nv->x = v2.y * v3.z - v3.y * v2.z;
    nv->y = -v2.x * v3.z + v3.x * v2.z;
    nv->z = v2.x * v3.y - v3.x * v2.y;
    Normalize_SN(nv);
}

void Normalize_SN(VERTEX* v)
{
    float R;

    R = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= R;
    v->y /= R;
    v->z /= R;
}

void NearestVector_SN(VERTEX* nv, VERTEX dir)
{
    float rplus, rminus;

    rplus = (dir.x - nv->x) * (dir.x - nv->x) +
            (dir.y - nv->y) * (dir.y - nv->y) +
            (dir.z - nv->z) * (dir.z - nv->z);
    rminus = (dir.x + nv->x) * (dir.x + nv->x) +
             (dir.y + nv->y) * (dir.y + nv->y) +
             (dir.z + nv->z) * (dir.z + nv->z);
    if (rminus < rplus)   
    {
        nv->x = -nv->x;
        nv->y = -nv->y;
        nv->z = -nv->z;
    }
}