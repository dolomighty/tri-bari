

// renderer per triangoli
// usa l'equazione del semipiano
// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation

// versione float
// texture prospettica
// tutto il precalcolabile è precalcolato
// il che ha portato ad un codice snello e generale
// amazing


#include "tri.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>


typedef unsigned char u8;

struct TRI tri;



void bayer_rgb( char *p , int u , int v ){
  const int r[]={ 100, 100, 255 };
  const int g[]={ 100, 255, 100 };
  const int b[]={ 255, 100, 100 };
  int i = ((u>>3)&1) + ((v>>3)&1);
  p[0] = r[i];
  p[1] = g[i];
  p[2] = b[i];
}


//int checker_rgb( char *p , int u , int v ){
//  static int r[]={ 255, 100 };
//  static int g[]={ 255, 100 };
//  static int b[]={ 255, 100 };
//  int i = ((u>>3)&1) ^ ((v>>3)&1);
//  p[0] = r[i];
//  p[1] = g[i];
//  p[2] = b[i];
//}



void tri_draw (){
#define V tri.vert  

  int xa,ya;
  int xb,yb;

  float dx10,dx21,dx02;
  float dy10,dy21,dy02;

  float du01,du02;
  float dv01,dv02;
  float dz01,dz02;

  // bounding rect + reject + viewport clip
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
    float iarea;
    float area = (V[2].x - V[0].x) * dy10 - (V[2].y - V[0].y) * dx10;
    if(area<0)return;
    iarea = 1 / area;
    du01 = (V[1].u-V[0].u)*iarea;
    du02 = (V[2].u-V[0].u)*iarea;
    dv01 = (V[1].v-V[0].v)*iarea;
    dv02 = (V[2].v-V[0].v)*iarea;
    dz01 = (V[1].z-V[0].z)*iarea;
    dz02 = (V[2].z-V[0].z)*iarea;
  }

  // i restanti delta
  dy21 = V[2].y - V[1].y;
  dy02 = V[0].y - V[2].y;
  dx21 = V[2].x - V[1].x;
  dx02 = V[0].x - V[2].x;


  // passiamo tutti i pixel
  {
    int y;

    float y0 = (ya - V[0].y) * dx10;
    float y1 = (ya - V[1].y) * dx21;
    float y2 = (ya - V[2].y) * dx02;

    float y0_dz02_y2_dz01_z0 = y0*dz02 + y2*dz01 - V[0].z;
    float y0_dz02_y2_dz01_z0_dy = dz02*dx10 + dz01*dx02;

    float y0_du02_y2_du01_u0 = y0*du02 + y2*du01 - V[0].u;
    float y0_du02_y2_du01_u0_dy = du02*dx10 + du01*dx02;

    float y0_dv02_y2_dv01_v0 = y0*dv02 + y2*dv01 - V[0].v;
    float y0_dv02_y2_dv01_v0_dy = dv02*dx10 + dv01*dx02;


    float x0 = (xa - V[0].x) * dy10;
    float x1 = (xa - V[1].x) * dy21;
    float x2 = (xa - V[2].x) * dy02;

    float x2_dz01_x0_dz02 = x2*dz01 + x0*dz02;
    float izdx = dz01*dy02 + dz02*dy10;

    float x2_du01_x0_du02 = x2*du01 + x0*du02;
    float uzdx = du01*dy02 + du02*dy10;

    float x2_dv01_x0_dv02 = x2*dv01 + x0*dv02;
    float vzdx = dv01*dy02 + dv02*dy10;

    u8 *rp = (u8*)tri.buffer + xa*tri.bpp + ya*tri.bpl;

//    DBG("uzdx %f vzdx %f izdx %f",uzdx,vzdx,izdx);

    for( y=ya ; y<=yb ; 
                        rp += tri.bpl,

                        y0 += dx10,
                        y1 += dx21,
                        y2 += dx02,

                        y0_dz02_y2_dz01_z0 += y0_dz02_y2_dz01_z0_dy,
                        y0_du02_y2_du01_u0 += y0_du02_y2_du01_u0_dy,
                        y0_dv02_y2_dv01_v0 += y0_dv02_y2_dv01_v0_dy,

                        y++ 
    ){
      int x;

      float _x0 = x0;
      float _x1 = x1;
      float _x2 = x2;

      float iz = x2_dz01_x0_dz02 - y0_dz02_y2_dz01_z0;
      float uz = x2_du01_x0_du02 - y0_du02_y2_du01_u0;
      float vz = x2_dv01_x0_dv02 - y0_dv02_y2_dv01_v0;

      u8 *p = rp;

      for( x=xa ; x<=xb ; 
                          p += tri.bpp,

                          _x0 += dy10,
                          _x1 += dy21,
                          _x2 += dy02,

                          iz += izdx,
                          uz += uzdx, 
                          vz += vzdx,

                          x ++
      ){
        if( _x0 < y0 )continue;
        if( _x1 < y1 )continue;
        if( _x2 < y2 )continue;
        // xy è dentro il triangolo
        {
          int u = uz / iz;
          int v = vz / iz;
          bayer_rgb(p,u,v);
//          checker_rgb(p,u,v);
//          DBG("uz %f vz %f iz %f",uz,vz,iz);
        }
      }

      
    }
  }
//  exit(1);
#undef V
}

/*



le grandezze in gioco

tri.c:187:tri.draw: uzdx 0.001594 vzdx -0.000135 izdx 0.000000
tri.c:230:tri.draw: uz 0.054236 vz 0.026970 iz 0.004344

vediamo che dot possiamo usare in un'implementazione fix

-0.000135 * (1<<16) == -8.84736 = -0x8 = -0b1000
... mmm un po pochino. dot24 credo vada bene
-0.000135 * (1<<24) == -2264.92416 = -0x8d8 = -0b100011011000

si, direi che va bene ... sicuramente è un buon inizio


*/


