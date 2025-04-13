#ifndef _SCENE_H_
#define _SCENE_H_

#include "sce_types.h"

extern long ObjectsNum;   
extern long NumGran;       
extern long NumVertex;  
extern int  NumLightSrc;   
extern OBJECTPARAM* objarray;   
extern GRANPARAM* grandef;       
extern VERTEXPARAM* vg;          
extern LIGHTSRCPARAM* lightsrc;  
extern VERTEX camerapos;        
extern float zProjectionPlane;                                  

extern float SpaceColorR,      
              SpaceColorG,
              SpaceColorB;
extern int  OpenArrays_SCE(long no,long ng,long nv,int nl);
extern void CloseArrays_SCE(void);
extern void SetColor_SCE(float r,float g,float b);
extern void SetMaterial_SCE(float ambient,
                            float diffuse,
                            float reflect,
                            float specular,
                            float p,
                            float transparency);

extern void SetCameraPosition_SCE(float x, float y, float z,
                                  float alpha, float beta,
                                  float zprojplane);
extern void CameraViewTransform_SCE(float* M);

extern int AddLightSource_SCE(float r,float g,float b,
                              float x,float y,float z);
extern int AddTriangle_SCE(float x1,float y1,float z1,
                           float x2,float y2,float z2,
                           float x3,float y3,float z3);
extern int AddQuad_SCE(float x1,float y1,float z1,
                       float x2,float y2,float z2,
                       float x3,float y3,float z3,
                       float x4,float y4,float z4);
extern int AddPyramid_SCE(float x0,float y0,float z0,
                          float x1,float y1,float z1,
                          float x2,float y2,float z2,
                          float x3,float y3,float z3,
                          float x4,float y4,float z4);
extern int AddPrisma4_SCE(float x1,float y1,
                          float x2,float y2,
                          float x3,float y3,
                          float x4,float y4,
                          float zbottom,float ztop);
extern int AddSphere_SCE(float x,float y,float z,
                         float r, int db, int dl);
extern int AddCylinder_SCE(float x,float y,float z,
                           float r, float h, int dl);
extern int CopyObjectAndShift_SCE(float sx,float sy,float sz);
#endif
