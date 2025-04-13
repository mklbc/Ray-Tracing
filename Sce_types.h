#ifndef _SCE_TYPES_H_
#define _SCE_TYPES_H_
#define FILL_GRAN_FLAT 0
#define FILL_GRAN_PHONG 1

struct VERTEX
{
    float x, y, z;      
};

struct VERTEXPARAM  
{
    VERTEX v;             
    VERTEX normal;       
};

struct OBJECTPARAM  
{
    long start;         
    long ng;            
    long fill;                                 
    float ambient;    
    float diffuse;
    float reflect;
    float specular;
    float p;          
    float transparency;     
    float r, g, b;       
    float radius;       
    float xo, yo, zo;    
};

struct GRANPARAM     
{
    long start;       
    long nv;             
    float radius;      
    float xo, yo, zo;     
};

struct LIGHTSRCPARAM
{
    VERTEX pos;          
    float r, g, b;        
};
#endif
