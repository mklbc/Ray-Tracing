
#include "framework.h"

#include <math.h>
#include <memory.h>

#include "scene.h"
#include "sce_normal.h"
#include "raytrace.h"
#include "transform.h"

static VERTEX pgn[4];        
static VERTEX res, nrm[4];


static void CreateStartRay_RAYTR(float* M, float* MV, int x, int y);
static void Ray_RAYTR(float* r, float* g, float* b,
                      float* M, long nobj, long ngran, int level);
static void DiffuseRay_RAYTR(float* r, float* g, float* b,
                             float* M, long nobj, long ngran);
static void SpecularRay_RAYTR(float* r, float* g, float* b,
                              float* M, int ng, float p);
static BOOL LightSourceIsVisible_RAYTR(float* r, float* g,
                                       float* b, float* M,
                                       float zl, int srcnum,
                                       int ngran);
static BOOL NearestGranPoint_RAYTR(float* M, float* znear,
                                   long* no, long* ng, long ngr);
static BOOL InObolochkaGran_RAYTR(float* M, int ng);
static BOOL InObolochkaObject_RAYTR(float* M, int no);
static BOOL IntersectPolygon_RAYTR(float* zp, int nv);
static void NormalToGran_RAYTR(float* M, VERTEX* normal, long no, long ng);
static void ReflectionVector_RAYTR(VERTEX* vr, VERTEX* n);



void Rendering_RAYTR(HDC hdc, int cx, int cy)
{
    float M[16], MV[16];            
    
    CameraViewTransform_SCE(MV);    

    for (int y = -cy / 2; y < cy / 2; y++)
        for (int x = -cx / 2; x < cx / 2; x++)
        {
            CreateStartRay_RAYTR(M, MV, x, y);
            float r = 0, g = 0, b = 0;
            Ray_RAYTR(&r, &g, &b, M, -1, -1, 0);   
            r *= 255.0;
            g *= 255.0;
            b *= 255.0;
            SetPixel(hdc, x + cx / 2, y + cy / 2,
                     RGB((BYTE)r, (BYTE)g, (BYTE)b));
        }

}


void CreateStartRay_RAYTR(float* M, float* MV, int x, int y)
{
    VERTEX dir;

    dir.x = x;
    dir.y = y;
    dir.z = zProjectionPlane;  
    SetKoordTransform_TRFM(M, &dir, 0, 0, 0); 
    MatrixAxB_TRFM(M, M, MV);  
}

void Ray_RAYTR(float* r, float* g, float* b,
               float* M, long nobj, long ngran, int level)

{
    long no, ng;
    float z;
    VERTEX normal, dir;
    float Md[16], Mr[16], Ms[16], Mt[16];
    float rres, gres, bres,
          robj, gobj, bobj,
          rdif = 0, gdif = 0, bdif = 0,
          rref = 0, gref = 0, bref = 0,
          rspe = 0, gspe = 0, bspe = 0,
          rtra = 0, gtra = 0, btra = 0;

    if (level > 5)   
    {
        *r = *g = *b = 0.5;
        return;
    }
    if (NearestGranPoint_RAYTR(M, &z, &no, &ng, ngran)) 
    {
        robj = objarray[no].r;         
        gobj = objarray[no].g;
        bobj = objarray[no].b;
        NormalToGran_RAYTR(M, &normal, no, ng);  
        if (objarray[no].diffuse > 0)
        {
            SetKoordTransform_TRFM(Md, &normal, 0, 0, z);
            MatrixAxB_TRFM(Md, Md, M);
            DiffuseRay_RAYTR(&rdif, &gdif, &bdif, Md, no, ng);
        }
        if (objarray[no].reflect > 0)   
        {
            ReflectionVector_RAYTR(&dir, &normal);
            SetKoordTransform_TRFM(Mr, &dir, 0, 0, z);
            MatrixAxB_TRFM(Mr, Mr, M);
            Ray_RAYTR(&rref, &gref, &bref, Mr, no, ng, level + 1);
        }
        if (objarray[no].specular > 0)  
        {
            ReflectionVector_RAYTR(&dir, &normal);
            SetKoordTransform_TRFM(Ms, &dir, 0, 0, z);
            MatrixAxB_TRFM(Ms, Ms, M);
            SpecularRay_RAYTR(&rspe, &gspe, &bspe, Ms, ng, objarray[no].p);
        }
        if (objarray[no].transparency > 0) 
        {
            MatrixShift_TRFM(Mt, M, z);   
            Ray_RAYTR(&rtra, &gtra, &btra, Mt, no, ng, level + 1);
        }
        rres = robj * objarray[no].ambient +
               rdif * objarray[no].diffuse +
               rref * objarray[no].reflect +
               rspe * objarray[no].specular +
               rtra * robj * objarray[no].transparency;
        gres = gobj * objarray[no].ambient +
               gdif * objarray[no].diffuse +
               gref * objarray[no].reflect +
               gspe * objarray[no].specular +
               gtra * gobj * objarray[no].transparency;
        bres = bobj * objarray[no].ambient +
               bdif * objarray[no].diffuse +
               bref * objarray[no].reflect +
               bspe * objarray[no].specular +
               btra * bobj * objarray[no].transparency;
        if (rres > 1) rres = 1;
        if (gres > 1) gres = 1;
        if (bres > 1) bres = 1;
        *r = rres;
        *g = gres;
        *b = bres;
    }
    else   
    {
        *r = SpaceColorR;
        *g = SpaceColorG;
        *b = SpaceColorB;
    }
}


void DiffuseRay_RAYTR(float* r, float* g, float* b,
                       float* M, long nobj, long ngran)
{
    VERTEX v, vl;
    int i;
    float ro, go, bo,
        rl, gl, bl,
        rres, gres, bres;
    float Kd;
    float Ml[16];

    ro = objarray[nobj].r;          
    go = objarray[nobj].g;
    bo = objarray[nobj].b;
    rres = gres = bres = 0;
    for (i = 0; i < NumLightSrc; i++)
    {
        TransformKoord_TRFM(M, &v, &lightsrc[i].pos);
        if (v.z >= 0) continue;
        Normalize_SN(&v);          
        SetKoordTransform_TRFM(Ml, &v, 0, 0, 0);
        MatrixAxB_TRFM(Ml, Ml, M);
        rl = lightsrc[i].r;
        gl = lightsrc[i].g;
        bl = lightsrc[i].b;
        TransformKoord_TRFM(Ml, &vl, &lightsrc[i].pos);
        if (LightSourceIsVisible_RAYTR(&rl, &gl, &bl, Ml, vl.z, i, ngran))
        {
            Kd = -v.z;
            if (Kd < 0) Kd = 0;
            if (Kd > 1) Kd = 1;
            rres += Kd * ro * rl;
            gres += Kd * go * gl;
            bres += Kd * bo * bl;
        }
    }
    if (rres > 1) rres = 1;
    if (gres > 1) gres = 1;
    if (bres > 1) bres = 1;
    *r = rres;
    *g = gres;
    *b = bres;
}


void SpecularRay_RAYTR(float* r, float* g, float* b,
    float* M, int ng, float p)
{
    VERTEX v, vl;
    int i;
    float rl, gl, bl, rres, gres, bres;
    float Kd;
    float Ml[16];

    rres = gres = bres = 0;
    for (i = 0; i < NumLightSrc; i++)
    {
        TransformKoord_TRFM(M, &v, &lightsrc[i].pos);
        Normalize_SN(&v);       
        SetKoordTransform_TRFM(Ml, &v, 0, 0, 0);
        MatrixAxB_TRFM(Ml, Ml, M);
        rl = lightsrc[i].r;
        gl = lightsrc[i].g;
        bl = lightsrc[i].b;
        TransformKoord_TRFM(M, &vl, &lightsrc[i].pos);
        if (LightSourceIsVisible_RAYTR(&rl, &gl, &bl, Ml, vl.z, i, ng))
        {
            Kd = -v.z;        
            if (Kd < 0) Kd = 0;
            if (Kd > 1) Kd = 1;
            Kd = pow(Kd, p);   
            rres += Kd * rl;
            gres += Kd * gl;
            bres += Kd * bl;
        }
    }
    if (rres > 1) rres = 1;
    if (gres > 1) gres = 1;
    if (bres > 1) bres = 1;
    *r = rres;
    *g = gres;
    *b = bres;
}


BOOL LightSourceIsVisible_RAYTR(float* r, float* g, float* b,
    float* M, float zl,
    int srcnum, int ngran)
{
    long no, ng;
    float z;
    float rres, gres, bres;
    float Mt[16];

    rres = *r;
    gres = *g;
    bres = *b;
    if (NearestGranPoint_RAYTR(M, &z, &no, &ng, ngran)) 
        if (z > zl)     
        {
            if (objarray[no].transparency < 0.01)    
            {                                
                *r = 0;
                *g = 0;
                *b = 0;
                return FALSE;
            }
            else           
            {
                rres *= objarray[no].r * objarray[no].transparency;
                gres *= objarray[no].g * objarray[no].transparency;
                bres *= objarray[no].b * objarray[no].transparency;
                MatrixShift_TRFM(Mt, M, z);  
                LightSourceIsVisible_RAYTR(&rres, &gres, &bres,
                    Mt, zl - z, srcnum, ng);
            }
        }
    *r = rres;
    *g = gres;
    *b = bres;
    return TRUE;     
}


BOOL NearestGranPoint_RAYTR(float* M, float* znear,
                            long* no, long* ng, long ngr)
{
    long i, ii, iii, numgr;
    long objnumber = -1, grannumber = -1;
    float z, zres;
    BOOL first = TRUE;

    for (i = 0; i < ObjectsNum; i++)            
    {
        if (!InObolochkaObject_RAYTR(M, i)) continue;
        for (ii = 0; ii < objarray[i].ng; ii++)  
        {
            numgr = objarray[i].start + ii; 
            if (numgr == ngr) continue;      
            if (!InObolochkaGran_RAYTR(M, numgr)) continue;
            for (iii = 0; iii < grandef[numgr].nv; iii++)
                TransformKoord_TRFM(M, &pgn[iii], &vg[grandef[numgr].start + iii].v);
            if (!IntersectPolygon_RAYTR(&z, grandef[numgr].nv))
                continue;
            if (z >= 0) continue;
            if (first)
            {
                objnumber = i;
                grannumber = numgr;
                zres = z; first = FALSE;
            }
            else
            {
                if (zres < z)
                {
                    zres = z;
                    objnumber = i;
                    grannumber = numgr;
                }
            }
        }
    }
    if (grannumber >= 0)       
    {
        *znear = zres;      
        *no = objnumber;  
        *ng = grannumber; 
        return TRUE;
    }
    return FALSE;
}


BOOL InObolochkaObject_RAYTR(float* M, int no)
{
    VERTEX v;
    float r;

    v.x = objarray[no].xo;
    v.y = objarray[no].yo;
    v.z = objarray[no].zo;
    r = objarray[no].radius;
    TransformKoord_TRFM(M, &v, &v);
    if (v.x * v.x + v.y * v.y <= r * r)
        return TRUE;
    return FALSE;
}


BOOL InObolochkaGran_RAYTR(float* M, int ng)
{
    VERTEX v;
    float r;

    v.x = grandef[ng].xo;
    v.y = grandef[ng].yo;
    v.z = grandef[ng].zo;
    r = grandef[ng].radius;
    TransformKoord_TRFM(M, &v, &v);
    if (v.x * v.x + v.y * v.y <= r * r)
        return TRUE;
    return FALSE;
}


BOOL IntersectPolygon_RAYTR(float* zp, int nv)
{
    int i, nhor, next, xmin, xmax, ymin, ymax;
    float x[4], z[4];
    float y1, y2;

    xmin = xmax = pgn[0].x;
    ymin = ymax = pgn[0].y;
    for (i = 1; i < nv; i++)
    {
        if (xmin > pgn[i].x) xmin = pgn[i].x;
        if (ymin > pgn[i].y) ymin = pgn[i].y;
        if (xmax < pgn[i].x) xmax = pgn[i].x;
        if (ymax < pgn[i].y) ymax = pgn[i].y;
    }
    if ((xmin > 0) || (xmax < 0) || (ymin > 0) || (ymax < 0))
        return FALSE;
    nhor = 0;
    for (i = 0; i < nv; i++)
    {
        next = i + 1;
        if (next >= nv) next = 0;
        y1 = pgn[i].y;
        y2 = pgn[next].y;
        if ((0 >= y1) && (0 < y2) || (0 <= y1) && (0 > y2))
        {
            x[nhor] = pgn[i].x - (pgn[next].x - pgn[i].x) * y1 / (y2 - y1);
            z[nhor] = pgn[i].z - (pgn[next].z - pgn[i].z) * y1 / (y2 - y1);
            nhor++;
        }
    }
    if (nhor != 2) return FALSE;
    if (x[1] == x[0])
    {
        if (x[1] == 0)
        {
            *zp = 0.5 * (z[0] + z[1]);
            return TRUE;
        }
        return FALSE;
    }
    if (((x[0] <= 0) && (x[1] >= 0)) ||
        ((x[0] >= 0) && (x[1] <= 0)))
    {
        *zp = z[0] - x[0] * (z[1] - z[0]) / (x[1] - x[0]);
        return TRUE;
    }
    return FALSE;
}

void InterpolateNormal_RAYTR(float* M, VERTEX* normal, VERTEX normalg, long ng)
{
    int i, nv, nhor, next;
    float x[4], z[4], nx[4], ny[4], nz[4];
    float y1, y2;

    res = normalg;  
    nv = grandef[ng].nv;
    for (i = 0; i < nv; i++)
    {
        TransformKoord_TRFM(M, &pgn[i],  &vg[grandef[ng].start + i].v);
        nrm[i] = vg[grandef[ng].start + i].normal;
        Rotate_TRFM(M, &nrm[i]);
        NearestVector_SN(&nrm[i], normalg);
    }
    nhor = 0;
    for (i = 0; i < nv; i++)
    {
        next = i + 1;
        if (next >= nv) next = 0;
        y1 = pgn[i].y;
        y2 = pgn[next].y;
        if ((0 >= y1) && (0 < y2) || (0 <= y1) && (0 > y2))
        {
            x[nhor] = pgn[i].x - (pgn[next].x - pgn[i].x) * y1 / (y2 - y1);
            z[nhor] = pgn[i].z - (pgn[next].z - pgn[i].z) * y1 / (y2 - y1);
            nx[nhor] = nrm[i].x - (nrm[next].x - nrm[i].x) * y1 / (y2 - y1);
            ny[nhor] = nrm[i].y - (nrm[next].y - nrm[i].y) * y1 / (y2 - y1);
            nz[nhor] = nrm[i].z - (nrm[next].z - nrm[i].z) * y1 / (y2 - y1);
            nhor++;
        }
    }
    if (nhor == 2)
    {
        if (x[1] == x[0])
        {
            res.x = 0.5 * (nx[0] + nx[1]);
            res.y = 0.5 * (ny[0] + ny[1]);
            res.z = 0.5 * (nz[0] + nz[1]);
        }
        else
        {
            res.x = nx[0] - x[0] * (nx[1] - nx[0]) / (x[1] - x[0]);
            res.y = ny[0] - x[0] * (ny[1] - ny[0]) / (x[1] - x[0]);
            res.z = nz[0] - x[0] * (nz[1] - nz[0]) / (x[1] - x[0]);
        }
    }
    Normalize_SN(&res);
    *normal = res;
}


void NormalToGran_RAYTR(float* M, VERTEX* normal, long no, long ng)
{
    VERTEX norm = vg[grandef[ng].start].normal;
    Rotate_TRFM(M, &norm);
    Normalize_SN(&norm);
    *normal = norm;
    if (objarray[no].fill == FILL_GRAN_FLAT) return;
    InterpolateNormal_RAYTR(M, normal, norm, ng);
}

void ReflectionVector_RAYTR(VERTEX* vr, VERTEX* n)
{
    vr->x = n->x * n->z;
    vr->y = n->y * n->z;
    vr->z = n->z * n->z - 0.5;
}
