
#include "common.h"

void      data_fn_scale1 (void                  * target,
			  void                  * src,
			  size_t                  bytes,
			  void                  * cookie)
{
  //fprintf(stderr, "target %p src %p bytes %ld\n", target, src, bytes);
  double *s = (double *) src;
  double *d = (double *) target;
  int nd = bytes >> 3;
  int i = 0;
  for (i = 0; i < nd; ++i)
    d[i] += s[i];
}

void      data_fn (void                  * target,
		   void                  * src,
		   size_t                  bytes,
		   void                  * cookie)
{
  //fprintf(stderr, "target %p src %p bytes %ld\n", target, src, bytes);
  double *s = (double *) src;
  double *d = (double *) target;
  int nd = bytes >> 3;
  int i = 0;
  ARMCIX_Info_t *ainfo =  (ARMCIX_Info_t *) cookie;
  double scale = ainfo->scale;

  for (i = 0; i < nd; ++i)
    d[i] += scale * s[i];
}

void      data_fn_strided (void                  * target,
			     void                  * src,
			     size_t                  bytes,
			     void                  * cookie)
{
  double *s = (double *) src;
  double *d = (double *) target;
  int nd = 0; 
  size_t i = 0, j;

  ARMCIX_Info_t *ainfo =  (ARMCIX_Info_t *) cookie;
  double scale = ainfo->scale;
  size_t sx, sy, ex, ey;
  size_t stride = ainfo->dstride;
  size_t poffset = ((size_t)target - (size_t)ainfo->dstaddr);
  sx = poffset % ainfo->count[0];
  sy = poffset / ainfo->count[0];
  ex = (poffset+bytes)  % ainfo->count[0];
  ey = (poffset+bytes)  / ainfo->count[0];
  if (ex == 0) {
    ex = ainfo->count[0];
    --ey;
  }
  
  //fprintf (stderr, " sx %ld sy %ld ex %ld ey %ld poffset %ld bytes %ld count0 %d count1 %d\n", sx, sy, ex, ey, poffset, bytes, ainfo->count[0], ainfo->count[1]);

  size_t offset = (sy*stride) / sizeof(double); 
  for (i = sx/sizeof(double); i < (ainfo->count[0]/sizeof(double)); ++i) 
    d[offset + i] += scale * s[nd++];
  
  for (j = sy+1; j < ey; ++j) {
    offset = (j*stride) / sizeof(double);
    for (i = 0; i < (ainfo->count[0]/sizeof(double)); ++i) 
      d[offset + i] += scale * s[nd++];    
  }

  if (j <= ey) {
    offset = (ey*stride) / sizeof(double); 
    for (i = 0; i < (ex/sizeof(double)); ++i)
      d[offset + i] += scale * s[nd++];
  }

  //if (nd != (bytes/sizeof(double)))
  //fprintf(stderr, "nd = %d, bytes %ld\n", nd, bytes);  
  ARMCIX_Assert (nd == (bytes/sizeof(double)));
}


#define QPX_LOAD(si,sb,fp)                      \
  do {                                                                  \
  asm volatile("qvlfdux %0,%1,%2": "=f"(fp),"+b" (si): "r" (sb));       \
  } while(0);


#define QPX_STORE(si,sb,fp) \
  do {                                                                  \
  asm volatile("qvstfdux %2,%0,%1": "+b" (si) : "r" (sb), "f"(fp) :"memory");   \
  } while(0);

#define ASM asm volatile

#ifndef __GNUC__
#define FP_REG(i)   ("f" #i)
#define FP_REG1(i)  "f"#i
#else
#define FP_REG(i)  ("fr" #i)
#define FP_REG1(i)  "fr"#i
#endif

static inline void     qpx_data_fn_sx (void                  * target,
				       void                  * src,
				       size_t                  bytes,
				       double                * scale)
{
  double *src0     = (double *) src;
  double *src1     = (double *) target;
  double *dst      = (double *) target;

  unsigned num_dbls = 64;
  register double f0  __asm__ FP_REG(0);
  register double f1  __asm__ FP_REG(1);
  register double f2  __asm__ FP_REG(2);
  register double f3  __asm__ FP_REG(3);
  register double f4  __asm__ FP_REG(4);
  register double f5  __asm__ FP_REG(5);
  register double f6  __asm__ FP_REG(6);
  register double f7  __asm__ FP_REG(7);
  register double f8  __asm__ FP_REG(8);
  register double f9  __asm__ FP_REG(9);
  register double f10 __asm__ FP_REG(10);
  register double f11 __asm__ FP_REG(11);
  register double f12 __asm__ FP_REG(12);
  f12 = *scale;

  unsigned niter = num_dbls >> 5; 
  niter --;

  double *src0_1, *src1_1;
  double *src0_2, *src1_2;
  double *dst_1, *dst_2;
  register int inc ;

  src0_1 = src0 -8;
  src1_1 = src1 -8;
  src0_2 = src0 -4;
  src1_2 = src1 -4;

  dst_1 = dst -8;
  dst_2 = dst -4;

  inc=64;

  QPX_LOAD(src0_1,inc,f0);
  QPX_LOAD(src1_1,inc,f1);
  QPX_LOAD(src0_1,inc,f2);
  QPX_LOAD(src1_1,inc,f3);
  QPX_LOAD(src0_1,inc,f4);
  QPX_LOAD(src1_1,inc,f5);
  QPX_LOAD(src0_1,inc,f6);
  QPX_LOAD(src1_1,inc,f7);
  ASM("qvfmadd 8, 0, 1, 12" ::: FP_REG1(8) );
  QPX_LOAD(src0_2,inc,f0);
  QPX_LOAD(src1_2,inc,f1);
  
  ASM("qvfmadd 9, 2, 3, 12" ::: FP_REG1(9) );
  QPX_LOAD(src0_2,inc,f2);
  QPX_LOAD(src1_2,inc,f3);
  
  ASM("qvfmadd 10, 4, 5, 12" ::: FP_REG1(10) );
  QPX_LOAD(src0_2,inc,f4);
  QPX_LOAD(src1_2,inc,f5);
  
  ASM("qvfmadd 11, 6, 7, 12" ::: FP_REG1(11) );
  QPX_LOAD(src0_2,inc,f6);
  QPX_LOAD(src1_2,inc,f7);  

  while (niter --) {
    QPX_STORE(dst_1,inc,f8);
    ASM("qvfmadd 8, 0, 1, 12" ::: FP_REG1(8)  );
    QPX_LOAD(src0_1,inc,f0);
    QPX_LOAD(src1_1,inc,f1);
    
    QPX_STORE(dst_1,inc,f9);
    ASM("qvfmadd 9, 2, 3, 12" ::: FP_REG1(9)  );
    QPX_LOAD(src0_1,inc,f2);
    QPX_LOAD(src1_1,inc,f3);
       
    QPX_STORE(dst_1,inc,f10);
    ASM("qvfmadd 10, 4, 5, 12" ::: FP_REG1(10)  );
    QPX_LOAD(src0_1,inc,f4);
    QPX_LOAD(src1_1,inc,f5);

    QPX_STORE(dst_1,inc,f11);
    ASM("qvfmadd 11, 6, 7, 12" ::: FP_REG1(11)  );
    QPX_LOAD(src0_1,inc,f6);
    QPX_LOAD(src1_1,inc,f7);
    
    QPX_STORE(dst_2,inc,f8);      
    ASM("qvfmadd 8, 0, 1, 12" ::: FP_REG1(8)  );
    QPX_LOAD(src0_2,inc,f0);
    QPX_LOAD(src1_2,inc,f1);

    QPX_STORE(dst_2,inc,f9);
    ASM("qvfmadd 9, 2, 3, 12" ::: FP_REG1(9)  );
    QPX_LOAD(src0_2,inc,f2);
    QPX_LOAD(src1_2,inc,f3);

    QPX_STORE(dst_2,inc,f10);
    ASM("qvfmadd 10, 4, 5, 12" ::: FP_REG1(10)  );
    QPX_LOAD(src0_2,inc,f4);
    QPX_LOAD(src1_2,inc,f5);

    QPX_STORE(dst_2,inc,f11);   
    ASM("qvfmadd 11, 6, 7, 12" ::: FP_REG1(11)  );
    QPX_LOAD(src0_2,inc,f6);
    QPX_LOAD(src1_2,inc,f7);      
  }

  QPX_STORE(dst_1,inc,f8);
  ASM("qvfmadd 8, 0, 1, 12" ::: FP_REG1(8) );
  QPX_STORE(dst_1,inc,f9);
  ASM("qvfmadd 9, 2, 3, 12" ::: FP_REG1(9) );
  QPX_STORE(dst_1,inc,f10);
  ASM("qvfmadd 10, 4, 5, 12" ::: FP_REG1(10) );
  QPX_STORE(dst_1,inc,f11);
  ASM("qvfmadd 11, 6, 7, 12" ::: FP_REG1(11) );
  
  QPX_STORE(dst_2,inc,f8);      
  QPX_STORE(dst_2,inc,f9);
  QPX_STORE(dst_2,inc,f10);  
  QPX_STORE(dst_2,inc,f11);       
}

void     qpx_data_fn_scale1 (void                  * target,
			     void                  * src,
			     size_t                  bytes,
			     void                  * cookie)
{
  if (bytes < 512) {
    data_fn_scale1(target, src, bytes, cookie);
    return;
  }
  
  double scale = 1.0;
  qpx_data_fn_sx (target, src, bytes, &scale);
}

void     qpx_data_fn (void                  * target,
		      void                  * src,
		      size_t                  bytes,
		      void                  * cookie)
{
  if (bytes < 512) {
    data_fn(target, src, bytes, cookie);
    return;
  }
  
  ARMCIX_Info_t *ainfo = (ARMCIX_Info_t *)cookie;
  qpx_data_fn_sx (target, src, bytes, &ainfo->scale);
}

static inline void  qpx_data_fn_strided_c0 (void                  * target,
					      void                  * src,
					      size_t                  bytes,
					      void                  * cookie,
					      int                     c0)
{
  if (bytes < 512) {
    data_fn_strided(target, src, bytes, cookie);
    return;
  }

  double *s0 = (double *) src;
  double *s1; 
  double *d0;
  size_t inc = 0;
  size_t offset = 0;
  size_t i = 0, j;
  
  ARMCIX_Info_t *ainfo =  (ARMCIX_Info_t *) cookie;
  size_t sy, ey;
  size_t stride = ainfo->dstride;
  size_t n = c0 / 32;

  size_t poffset = ((size_t)target - (size_t)ainfo->dstaddr);
  sy = poffset / c0;
  ey = (poffset+512)  / c0;

  register double f0  __asm__ FP_REG(0);
  register double f1  __asm__ FP_REG(1);
  register double f2  __asm__ FP_REG(2);
  register double f3  __asm__ FP_REG(3);
  register double f4  __asm__ FP_REG(4);
  f2 = ainfo->scale;

  inc = 32;
  s0 -= 4;
  for (j = sy; j < ey; ++j) {
    offset = (j*stride) / sizeof(double);
    d0     = (double*)((char*)target + offset);
    d0    -= 4;
    s1     = d0;
    for (i = 0; i < n; ++i) {
      //d[offset + i] += scale * s[nd++];    
      QPX_LOAD(s0,inc,f0);
      QPX_LOAD(s1,inc,f1);
      ASM("qvfmadd 4, 0, 1, 2" ::: FP_REG1(4) );      
      QPX_STORE(d0,inc,f4);
    }
  }

  ARMCIX_Assert ((size_t)s0 == (size_t)((char*)src+512-32));
}


void      qpx_data_fn_strided_1 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie)
{
  qpx_data_fn_strided_c0 (target,
			  src,
			  bytes,
			  cookie,
			  32);
}

void      qpx_data_fn_strided_2 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie)
{
  qpx_data_fn_strided_c0 (target,
			  src,
			  bytes,
			  cookie,
			  64);
}

void      qpx_data_fn_strided_4 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie)
{
  qpx_data_fn_strided_c0 (target,
			  src,
			  bytes,
			  cookie,
			  128);
}

void      qpx_data_fn_strided_8 (void                  * target,
				 void                  * src,
				 size_t                  bytes,
				 void                  * cookie)
{
  qpx_data_fn_strided_c0 (target,
			  src,
			  bytes,
			  cookie,
			  256);
}

void packStrided   ( ARMCIX_Info_t     * ainfo,
		     void              * dstbuf,
		     void              * srcbuf ) 
{
  int i,j,nd=0;
  int offset = 0;
  double *d = (double *) dstbuf;
  double *s = (double *) srcbuf;
  register double f0  __asm__ FP_REG(0);
  int inc = 32;

  if (ainfo->count[0] == 32){
    d -= 4;    
    for (j = 0; j < ainfo->count[1]; ++j) {
      offset = j*ainfo->sstride/sizeof(double);
      s  =  (double *)srcbuf + offset;
      s -= 4;
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
    }
  }    
  else if (ainfo->count[0] == 64) {
    d -= 4;    
    for (j = 0; j < ainfo->count[1]; ++j) {
      offset = j*ainfo->sstride/sizeof(double);
      s  =  (double *)srcbuf + offset;
      s -= 4;
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
    }
  }
  else if (ainfo->count[0] == 128){
    d -= 4;    
    for (j = 0; j < ainfo->count[1]; ++j) {
      offset = j*ainfo->sstride/sizeof(double);
      s  =  (double *)srcbuf + offset;
      s -= 4;
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
    }
  }
  else if (ainfo->count[0] == 256){
    d -= 4;    
    for (j = 0; j < ainfo->count[1]; ++j) {
      offset = j*ainfo->sstride/sizeof(double);
      s  =  (double *)srcbuf + offset;
      s -= 4;
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
      QPX_LOAD  (s, inc, f0);
      QPX_STORE (d, inc, f0);
    }
  }
  else {
    for (j = 0; j < ainfo->count[1]; ++j) {
      offset = j*ainfo->sstride/sizeof(double);
      for (i = 0; i < ainfo->count[0]/sizeof(double); ++i)
	d[nd ++] = s[offset + i];
    }
  }


#if 0
  else 
#endif
}
