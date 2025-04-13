
#include <math.h>
#include "scene.h"
#include "sce_normal.h"

#define TO_RADIAN 0.0174532925199433

long ObjectsNum = 0;
long NumGran = 0;
long NumVertex = 0;
int  NumLightSrc = 0;
OBJECTPARAM* objarray = 0;
GRANPARAM* grandef = 0;
VERTEXPARAM* vg = 0;
LIGHTSRCPARAM* lightsrc = 0;

VERTEX camerapos;
static float cameraAlpha = 30, cameraBeta = 80;
float zProjectionPlane = -200;

float SpaceColorR = 0.1,
SpaceColorG = 0.1,
SpaceColorB = 0.1;
static long MaximumObjectsNum = 0,
MaximumGranNum = 0,
MaximumVertexNum = 0,
MaximumLightSrc = 0;
static OBJECTPARAM currentparam;  
static long ObjectCopyNum = -1;   

static void EmptyScene_SCE(void);
static int AddTriangleGran_SCE(float x1, float y1, float z1,
                               float x2, float y2, float z2,
                               float x3, float y3, float z3, 
                               int obj);
static int AddQuadGran_SCE(float x1, float y1, float z1,
                           float x2, float y2, float z2,
                           float x3, float y3, float z3,
                           float x4, float y4, float z4, int obj);
static void Obolochka_SCE(float* r, float* x, float* y, float* z, long start, long nv);
static void StoreNormal_SCE(VERTEX v, long pos);
static void SpherePoint_SCE(VERTEX* v, float R, int b, int l);
static void CylinderPoint_SCE(VERTEX* v, float R, float H, int s, int l);


int OpenArrays_SCE(long no, long ng, long nv, int nl)
{
    CloseArrays_SCE();
    EmptyScene_SCE();
    objarray = new OBJECTPARAM[no];
    grandef = new GRANPARAM[ng];
    vg = new VERTEXPARAM[nv];
    lightsrc = new LIGHTSRCPARAM[nl];
    if ((objarray == 0) || (grandef == 0) || (vg == 0) || (lightsrc == 0))
    {
        CloseArrays_SCE();
        return 0;          
    }
    MaximumObjectsNum = no;
    MaximumGranNum = ng;
    MaximumVertexNum = nv;
    MaximumLightSrc = nl;
    return 1;
}

void CloseArrays_SCE(void)
{
    if (lightsrc) delete[]lightsrc;
    if (vg)       delete[]vg;
    if (grandef)  delete[]grandef;
    if (objarray) delete[]objarray;
    objarray = 0;
    grandef = 0;
    vg = 0;
    lightsrc = 0;
}

void EmptyScene_SCE(void)
{
    ObjectsNum = NumGran = NumVertex = NumLightSrc = 0;
    ObjectCopyNum = -1;
    currentparam.start = 0;
    currentparam.ng = 0;
    currentparam.ambient = 0;
    currentparam.diffuse = 1;
    currentparam.reflect = 0;
    currentparam.specular = 0;
    currentparam.p = 20;
    currentparam.transparency = 0;
    currentparam.r = 1;
    currentparam.g = 1;
    currentparam.b = 1;
    currentparam.radius = -1;
}

void SetColor_SCE(float r, float g, float b)
{
    currentparam.r = r;
    currentparam.g = g;
    currentparam.b = b;
}

void SetMaterial_SCE(float ambient,
    float diffuse,
    float reflect,
    float specular,
    float p,
    float transparency)
{
    currentparam.ambient = ambient;
    currentparam.diffuse = diffuse;
    currentparam.reflect = reflect;
    currentparam.specular = specular;
    currentparam.p = p;
    currentparam.transparency = transparency;
}

void SetCameraPosition_SCE(float x, float y, float z,
    float alpha, float beta,
    float zprojplane)
{
    camerapos.x = x;
    camerapos.y = y;
    camerapos.z = z;
    cameraAlpha = alpha;
    cameraBeta = beta;
    zProjectionPlane = -zprojplane;
}

void CameraViewTransform_SCE(float* M)
{
    float a, b;

    a = cameraAlpha * TO_RADIAN;
    b = cameraBeta * TO_RADIAN;
    for (int i = 0; i < 16; i++) M[i] = 0;
    M[0] = M[5] = M[10] = M[15] = 1;
    M[0] = cos(a);
    M[1] = -sin(a);
    M[2] = 0;
    M[3] = -M[0] * camerapos.x - M[1] * camerapos.y;
    M[4] = sin(a) * cos(b);
    M[5] = cos(a) * cos(b);
    M[6] = -sin(b);
    M[7] = -M[4] * camerapos.x - M[5] * camerapos.y - M[6] * camerapos.z;
    M[8] = sin(a) * sin(b);
    M[9] = cos(a) * sin(b);
    M[10] = cos(b);
    M[11] = -M[8] * camerapos.x - M[9] * camerapos.y - M[10] * camerapos.z;
}

int AddLightSource_SCE(float r, float g, float b,
    float x, float y, float z)
{
    if (NumLightSrc + 1 > MaximumLightSrc) return 0;
    lightsrc[NumLightSrc].pos.x = x;
    lightsrc[NumLightSrc].pos.y = y;
    lightsrc[NumLightSrc].pos.z = z;
    lightsrc[NumLightSrc].r = r;
    lightsrc[NumLightSrc].g = g;
    lightsrc[NumLightSrc].b = b;
    NumLightSrc++;
    return 1;
}

int AddTriangle_SCE(float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3)
{
    return AddTriangleGran_SCE(x1, y1, z1,
        x2, y2, z2,
        x3, y3, z3, 1);
}


int AddTriangleGran_SCE(float x1, float y1, float z1,
                        float x2, float y2, float z2,
                        float x3, float y3, float z3,
                        int obj)
{
    if (ObjectsNum + 1 > MaximumObjectsNum) return 0;
    if (NumGran + 1 > MaximumGranNum) return 0;
    if (NumVertex + 3 > MaximumVertexNum) return 0;

    if (obj)
    {
        currentparam.start = NumGran;
        currentparam.ng = 1;
    }
    else currentparam.ng++;
    currentparam.radius = -1;   
    currentparam.fill = 0;        
    objarray[ObjectsNum] = currentparam;
    grandef[NumGran].start = NumVertex;
    grandef[NumGran].nv = 3;
    vg[NumVertex].v.x = x1;
    vg[NumVertex].v.y = y1;
    vg[NumVertex].v.z = z1;
    vg[NumVertex + 1].v.x = x2;
    vg[NumVertex + 1].v.y = y2;
    vg[NumVertex + 1].v.z = z2;
    vg[NumVertex + 2].v.x = x3;
    vg[NumVertex + 2].v.y = y3;
    vg[NumVertex + 2].v.z = z3;

    VERTEX normal;
    NormalVector_SN(&normal, vg[NumVertex].v, vg[NumVertex + 1].v, vg[NumVertex + 2].v);
    vg[NumVertex].normal = normal;
    vg[NumVertex+1].normal = normal;
    vg[NumVertex+2].normal = normal;

    Obolochka_SCE(&grandef[NumGran].radius,
                  &grandef[NumGran].xo,
                  &grandef[NumGran].yo,
                  &grandef[NumGran].zo,
                  grandef[NumGran].start, 3);
    if (obj)
    {
        Obolochka_SCE(&objarray[ObjectsNum].radius,
                      &objarray[ObjectsNum].xo,
                      &objarray[ObjectsNum].yo,
                      &objarray[ObjectsNum].zo,
                      grandef[NumGran].start, 3);
        ObjectCopyNum = ObjectsNum;
        ObjectsNum++;
    }
    NumGran++;
    NumVertex += 3;
    return 1;
}


int AddQuad_SCE(float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4)
{
    return AddQuadGran_SCE(x1, y1, z1,
        x2, y2, z2,
        x3, y3, z3,
        x4, y4, z4, 1);
}

int AddQuadGran_SCE(float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    int obj)
{
    if (ObjectsNum + 1 > MaximumObjectsNum) return 0;
    if (NumGran + 1 > MaximumGranNum) return 0;
    if (NumVertex + 4 > MaximumVertexNum) return 0;

    if (obj)
    {
        currentparam.start = NumGran;
        currentparam.ng = 1;
    }
    else currentparam.ng++;
    currentparam.radius = -1;
    currentparam.fill = 0;    
    objarray[ObjectsNum] = currentparam;
    grandef[NumGran].start = NumVertex;
    grandef[NumGran].nv = 4;
    vg[NumVertex].v.x = x1;
    vg[NumVertex].v.y = y1;
    vg[NumVertex].v.z = z1;
    vg[NumVertex + 1].v.x = x2;
    vg[NumVertex + 1].v.y = y2;
    vg[NumVertex + 1].v.z = z2;
    vg[NumVertex + 2].v.x = x3;
    vg[NumVertex + 2].v.y = y3;
    vg[NumVertex + 2].v.z = z3;
    vg[NumVertex + 3].v.x = x4;
    vg[NumVertex + 3].v.y = y4;
    vg[NumVertex + 3].v.z = z4;

    VERTEX normal;
    NormalVector_SN(&normal, vg[NumVertex].v, vg[NumVertex + 1].v, vg[NumVertex + 2].v);
    vg[NumVertex].normal = normal;
    vg[NumVertex + 1].normal = normal;
    vg[NumVertex + 2].normal = normal;
    vg[NumVertex + 3].normal = normal;

    Obolochka_SCE(&grandef[NumGran].radius,
        &grandef[NumGran].xo,
        &grandef[NumGran].yo,
        &grandef[NumGran].zo,
        grandef[NumGran].start, 4);
    if (obj)
    {
        Obolochka_SCE(&objarray[ObjectsNum].radius,
            &objarray[ObjectsNum].xo,
            &objarray[ObjectsNum].yo,
            &objarray[ObjectsNum].zo,
            grandef[NumGran].start, 4);
        ObjectCopyNum = ObjectsNum;
        ObjectsNum++;
    }
    NumGran++;
    NumVertex += 4;
    return 1;
}


int AddPyramid_SCE(float x0, float y0, float z0,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4)
{
    long st;

    st = NumVertex;
    currentparam.start = NumGran;
    currentparam.ng = 0;
    AddQuadGran_SCE(x1, y1, z1,
        x2, y2, z2,
        x3, y3, z3,
        x4, y4, z4, 0);
    AddTriangleGran_SCE(x0, y0, z0,
        x1, y1, z1,
        x2, y2, z2, 0);
    AddTriangleGran_SCE(x0, y0, z0,
        x2, y2, z2,
        x3, y3, z3, 0);
    AddTriangleGran_SCE(x0, y0, z0,
        x3, y3, z3,
        x4, y4, z4, 0);
    AddTriangleGran_SCE(x0, y0, z0,
        x4, y4, z4,
        x1, y1, z1, 0);
    Obolochka_SCE(&objarray[ObjectsNum].radius,
        &objarray[ObjectsNum].xo,
        &objarray[ObjectsNum].yo,
        &objarray[ObjectsNum].zo,
        grandef[objarray[ObjectsNum].start].start,
        NumVertex - st);
    ObjectCopyNum = ObjectsNum;
    ObjectsNum++;
    return 1;
}

int AddPrisma4_SCE(float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    float x4, float y4,
    float zbottom, float ztop)
{
    long st;

    st = NumVertex;
    currentparam.start = NumGran;
    currentparam.ng = 0;
    AddQuadGran_SCE(x1, y1, zbottom,
        x2, y2, zbottom,
        x2, y2, ztop,
        x1, y1, ztop, 0);
    AddQuadGran_SCE(x2, y2, zbottom,
        x3, y3, zbottom,
        x3, y3, ztop,
        x2, y2, ztop, 0);
    AddQuadGran_SCE(x3, y3, zbottom,
        x4, y4, zbottom,
        x4, y4, ztop,
        x3, y3, ztop, 0);
    AddQuadGran_SCE(x4, y4, zbottom,
        x1, y1, zbottom,
        x1, y1, ztop,
        x4, y4, ztop, 0);
    AddQuadGran_SCE(x1, y1, zbottom,
        x2, y2, zbottom,
        x3, y3, zbottom,
        x4, y4, zbottom, 0);
    AddQuadGran_SCE(x1, y1, ztop,
        x2, y2, ztop,
        x3, y3, ztop,
        x4, y4, ztop, 0);
    Obolochka_SCE(&objarray[ObjectsNum].radius,
        &objarray[ObjectsNum].xo,
        &objarray[ObjectsNum].yo,
        &objarray[ObjectsNum].zo,
        grandef[objarray[ObjectsNum].start].start,
        NumVertex - st);
    ObjectCopyNum = ObjectsNum;
    ObjectsNum++;
    return 1;
}

int AddSphere_SCE(float x, float y, float z, float r, int db, int dl)
{
    int b, l;
    long st, stv;
    VERTEX v[4];
    VERTEX norm;

    st = NumVertex;
    currentparam.start = NumGran;
    currentparam.ng = 0;
   
    for (b = -90+db; b < 90-db; b += db)
        for (l = 0; l < 360; l += dl)
        {
            SpherePoint_SCE(&v[0], r, b, l);
            SpherePoint_SCE(&v[1], r, b + db, l);
            SpherePoint_SCE(&v[2], r, b + db, l + dl);
            SpherePoint_SCE(&v[3], r, b, l + dl);
            stv = NumVertex;
            AddQuadGran_SCE(
                v[0].x + x, v[0].y + y, v[0].z + z,
                v[1].x + x, v[1].y + y, v[1].z + z,
                v[2].x + x, v[2].y + y, v[2].z + z,
                v[3].x + x, v[3].y + y, v[3].z + z,  0);

            StoreNormal_SCE(v[0], stv);
            StoreNormal_SCE(v[1], stv + 1);
            StoreNormal_SCE(v[2], stv + 2);
            StoreNormal_SCE(v[3], stv + 3);
        }

    b = 90 - db;
    for (l = 0; l < 360; l += dl)
    {
        SpherePoint_SCE(&v[0], r, b, l);
        SpherePoint_SCE(&v[1], r, b + db, l+dl/2);
        SpherePoint_SCE(&v[2], r, b, l + dl);
        stv = NumVertex;
        AddTriangleGran_SCE(
            v[0].x + x, v[0].y + y, v[0].z + z,
            v[1].x + x, v[1].y + y, v[1].z + z,
            v[2].x + x, v[2].y + y, v[2].z + z,  0);
         
        StoreNormal_SCE(v[0], stv);
        StoreNormal_SCE(v[1], stv + 1);
        StoreNormal_SCE(v[2], stv + 2);
    }
 
    objarray[ObjectsNum].fill = FILL_GRAN_PHONG;

    Obolochka_SCE(&objarray[ObjectsNum].radius,
        &objarray[ObjectsNum].xo,
        &objarray[ObjectsNum].yo,
        &objarray[ObjectsNum].zo,
        grandef[objarray[ObjectsNum].start].start,
        NumVertex - st);
    ObjectCopyNum = ObjectsNum;
    ObjectsNum++;
    return 1;
}

void StoreNormal_SCE(VERTEX normal, long pos)
{
    Normalize_SN(&normal);
    vg[pos].normal = normal;
}

int AddCylinder_SCE(float x, float y, float z,
    float r, float h, int dl)
{
    int l;
    long st;
    VERTEX v[4];

    st = NumVertex;
    currentparam.start = NumGran;
    currentparam.ng = 0;
    for (l = 0; l < 360; l += dl)
    {
        CylinderPoint_SCE(&v[0], r, h, 0, l);
        CylinderPoint_SCE(&v[1], r, h, 100, l);
        CylinderPoint_SCE(&v[2], r, h, 100, l + dl);
        CylinderPoint_SCE(&v[3], r, h, 0, l + dl);
        AddQuadGran_SCE(v[0].x + x, v[0].y + y, v[0].z + z,
            v[1].x + x, v[1].y + y, v[1].z + z,
            v[2].x + x, v[2].y + y, v[2].z + z,
            v[3].x + x, v[3].y + y, v[3].z + z,
            0);
    }
    Obolochka_SCE(&objarray[ObjectsNum].radius,
        &objarray[ObjectsNum].xo,
        &objarray[ObjectsNum].yo,
        &objarray[ObjectsNum].zo,
        grandef[objarray[ObjectsNum].start].start,
        NumVertex - st);
    ObjectCopyNum = ObjectsNum;
    ObjectsNum++;
    return 1;
}


int CopyObjectAndShift_SCE(float sx, float sy, float sz)
{
    long startgran, numgran, startvertex, numvertex, i, ii;

    if ((ObjectCopyNum < 0) || (ObjectCopyNum >= ObjectsNum)) return 0;
    objarray[ObjectsNum] = objarray[ObjectCopyNum];
    objarray[ObjectsNum].start = NumGran;  
    objarray[ObjectsNum].xo += sx;    
    objarray[ObjectsNum].yo += sy;
    objarray[ObjectsNum].zo += sz;
    startgran = objarray[ObjectCopyNum].start;                                          
    numgran = objarray[ObjectCopyNum].ng;     
    for (i = 0; i < numgran; i++)
    {
        grandef[NumGran] = grandef[startgran + i];
        grandef[NumGran].start = NumVertex; 
        grandef[NumGran].xo += sx;          
        grandef[NumGran].yo += sy;
        grandef[NumGran].zo += sz;
        NumGran++;
        startvertex = grandef[startgran + i].start;
        numvertex = grandef[startgran + i].nv;
        for (ii = 0; ii < numvertex; ii++)      
        {
            vg[NumVertex] = vg[startvertex + ii];
            vg[NumVertex].v.x += sx;
            vg[NumVertex].v.y += sy;
            vg[NumVertex].v.z += sz;
            NumVertex++;
        }
    }
    ObjectsNum++;
    return 1;
}


void SpherePoint_SCE(VERTEX* v, float R, int b, int l)
{
    float B, L;

    B = (float)b * TO_RADIAN;
    L = (float)l * TO_RADIAN;
    v->x = R * cos(B) * sin(L);
    v->y = R * cos(B) * cos(L);
    v->z = R * sin(B);
}

void CylinderPoint_SCE(VERTEX* v, float R, float H, int s, int l)
{
    float L;

    L = (float)l * TO_RADIAN;
    v->x = R * sin(L);
    v->y = R * cos(L);
    v->z = (float)s * H / 100.0;
}


void Obolochka_SCE(float* r, float* x, float* y, float* z,
    long start, long nv)
{
    long i;
    float xo, yo, zo, rad, r1;
    float minx, maxx, miny, maxy, minz, maxz;

    minx = maxx = vg[start].v.x;
    miny = maxy = vg[start].v.y;
    minz = maxz = vg[start].v.z;
    for (i = 1; i < nv; i++)
    {
        if (minx > vg[start + i].v.x) minx = vg[start + i].v.x;
        if (maxx < vg[start + i].v.x) maxx = vg[start + i].v.x;
        if (miny > vg[start + i].v.y) miny = vg[start + i].v.y;
        if (maxy < vg[start + i].v.y) maxy = vg[start + i].v.y;
        if (minz > vg[start + i].v.z) minz = vg[start + i].v.z;
        if (maxz < vg[start + i].v.z) maxz = vg[start + i].v.z;
    }

    xo = 0.5 * (maxx + minx);
    yo = 0.5 * (maxy + miny);
    zo = 0.5 * (maxz + minz);
    rad = 0.5 * (maxx - minx);
    r1 = 0.5 * (maxy - miny);
    if (rad < r1) rad = r1;
    r1 = 0.5 * (maxz - minz);
    if (rad < r1) rad = r1;
    for (i = 0; i < nv; i++)
    {
        r1 = sqrt((xo - vg[start + i].v.x) * (xo - vg[start + i].v.x) +
                  (yo - vg[start + i].v.y) * (yo - vg[start + i].v.y) +
                  (zo - vg[start + i].v.z) * (zo - vg[start + i].v.z));
        if (rad < r1) rad = r1;
    }
    *x = xo;
    *y = yo;
    *z = zo;
    *r = rad;
}
