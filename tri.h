
#ifndef     _tri_h_
#define     _tri_h_

#undef EXT
#ifdef  __cplusplus
#define EXT extern "C" 
#else
#define EXT extern
#endif


struct TRI_VERT {
  float x,y,z,u,v;
};

struct TRI_CLIP {
  int l,r,t,b;
};

struct TRI {
  void *buffer;
  int bpl;
  int bpp;
  void *texture;
  struct TRI_CLIP clip;
  struct TRI_VERT vert[3];
};

EXT struct TRI tri;
EXT void tri_draw();


#endif
