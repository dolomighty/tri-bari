
// renderer per triangoli
// usa l'equazione del semipiano
// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation

// versione float
// texture prospettica
// proviamo a vedere come appare il montecarlo


#include "tri.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>


struct TRI tri;



void bayer_rgb( uint8_t *p , int u , int v ){
    static int r[]={ 100, 100, 255 };
    static int g[]={ 100, 255, 100 };
    static int b[]={ 255, 100, 100 };
    int i = ((u>>3)&1) + ((v>>3)&1);
    p[0] = r[i];
    p[1] = g[i];
    p[2] = b[i];
}


//int checker_rgb( uint8_t *p , int u , int v ){
//  static int r[]={ 255, 100 };
//  static int g[]={ 255, 100 };
//  static int b[]={ 255, 100 };
//  int i = ((u>>3)&1) ^ ((v>>3)&1);
//  p[0] = r[i];
//  p[1] = g[i];
//  p[2] = b[i];
//}



void tri_draw (){

// ref. impl.

#define V tri.vert  

    int xa,ya;
    int xb,yb;

    float dx10,dx21,dx02;
    float dy10,dy21,dy02;

    float du01,du02;
    float dv01,dv02;
    float dz01,dz02;

    // bounding rect + reject + viewport clip
    //  x→ y↑       xb,yb
    //       ......,
    //       :     :
    //       :     :
    //       ;.....:
    // xa,ya 

    xa = V[0].x;
    if(xa>V[1].x)xa=V[1].x;
    if(xa>V[2].x)xa=V[2].x;
    if(xa>tri.clip.r)return;
    if(xa<tri.clip.l)xa=tri.clip.l;

    xb = V[0].x;
    if(xb<V[1].x)xb=V[1].x;
    if(xb<V[2].x)xb=V[2].x;
    if(xb<tri.clip.l)return;
    if(xb>tri.clip.r)xb=tri.clip.r;

    ya = V[0].y;
    if(ya>V[1].y)ya=V[1].y;
    if(ya>V[2].y)ya=V[2].y;
    if(ya>tri.clip.b)return;
    if(ya<tri.clip.t)ya=tri.clip.t;

    yb = V[0].y;
    if(yb<V[1].y)yb=V[1].y;
    if(yb<V[2].y)yb=V[2].y;
    if(yb<tri.clip.t)return;
    if(yb>tri.clip.b)yb=tri.clip.b;

    // alcuni delta
    dy10 = V[1].y - V[0].y;
    dx10 = V[1].x - V[0].x;

    {
        float area = (V[2].x - V[0].x) * dy10 - (V[2].y - V[0].y) * dx10;
        if(area<0)return;
        {
            float iarea = 1 / area;
            du01 = (V[1].u-V[0].u)*iarea;
            du02 = (V[2].u-V[0].u)*iarea;
            dv01 = (V[1].v-V[0].v)*iarea;
            dv02 = (V[2].v-V[0].v)*iarea;
            dz01 = (V[1].z-V[0].z)*iarea;
            dz02 = (V[2].z-V[0].z)*iarea;
        }
    }

    // i restanti delta
    dy21 = V[2].y - V[1].y;
    dy02 = V[0].y - V[2].y;
    dx21 = V[2].x - V[1].x;
    dx02 = V[0].x - V[2].x;

    {
        int y;
#pragma omp parallel for
        for(y=ya;y<=yb;y++)
        {
            float sa0y = V[0].x*dy10 + (y - V[0].y)*dx10;
            float sa1y = V[1].x*dy21 + (y - V[1].y)*dx21;
            float sa2y = V[2].x*dy02 + (y - V[2].y)*dx02;
            uint8_t* rp = (uint8_t*)tri.buffer + y*tri.bpl;
            int x;
            for(x=xa;x<=xb; x++)
            {
                float sa0;
                float sa1;
                float sa2;
                float u,v,z;

//        if(rand()&3)continue;

                sa0 = x*dy10 - sa0y;
                if(sa0<0)continue;
                sa1 = x*dy21 - sa1y;
                if(sa1<0)continue;
                sa2 = x*dy02 - sa2y;
                if(sa2<0)continue;

                // il punto è dentro il triangolo

                z = (V[0].z + sa2*dz01 + sa0*dz02);
                u = (V[0].u + sa2*du01 + sa0*du02) / z;
                v = (V[0].v + sa2*dv01 + sa0*dv02) / z;


    //      sa0 /= area;
    //      sa1 /= area;
    //      sa2 /= area;
    //
    //      z = 1 / (sa1*V[0].z + sa2*V[1].z + sa0*V[2].z);
    //      v = z * (sa1*V[0].v + sa2*V[1].v + sa0*V[2].v);
    //      u = z * (sa1*V[0].u + sa2*V[1].u + sa0*V[2].u);

                bayer_rgb( rp+x*tri.bpp , u , v );
    //      DBG("x %d y %d u %f v %f z %f",x,y,u,v,z);
            }
        }
    }




//  exit(1);
#undef V
}






