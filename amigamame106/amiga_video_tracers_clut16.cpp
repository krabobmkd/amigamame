#include "amiga_video_tracers_clut16.h"

// this files is just to implement template calls...

template<typename T> void doSwap(T&a,T&b) { T c=a; a=b; b=c; }

// to manage 24 bits mode pixel copy without any arse,
// assume there is a 3 byte length type than can copy its value from a 4 byte type.
// this is finely used by following templates for 24bits mode.
struct type24{
    type24(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    char r,g,b;
};


struct typeRGB15{
    typeRGB15(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+3))),b((char)argb>>3) {}
    USHORT  a:1, r:5, g:5, b:5;
};

struct typeBGR15{
    typeBGR15(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+3))),b((char)argb>>3) {}
    USHORT  a:1, b:5, g:5, r:5;
};
// validated:
struct typeRGB16{
    typeRGB16(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+2))),b((char)argb>>3) {}
    USHORT  r:5, g:6, b:5;
};

struct typeBGR16{
    typeBGR16(ULONG argb) : r((char)(argb>>(16+3))),g((char)(argb>>(8+2))),b((char)argb>>3) {}
    USHORT  b:5, g:6, r:5;
};
// - - - -

struct typeRGB15PC{
    typeRGB15PC(ULONG argb) : a(argb){
       b = (b<<8) |(b>>8);
    }
    union { typeRGB15 a; USHORT b;};
};

struct typeBGR15PC{
    typeBGR15PC(ULONG argb) : a(argb){
       b = (b<<8) |(b>>8);
    }
    union { typeBGR15 a; USHORT b;};
};


struct typeRGB16PC{
    typeRGB16PC(ULONG argb) : a(argb){
        b = (b<<8) |(b>>8);
    }
    union { typeRGB16 a; USHORT b;};
};

struct typeBGR16PC{
    typeBGR16PC(ULONG argb) : a(argb){
       b = (b<<8) |(b>>8);
    }
    union { typeBGR16 a; USHORT b;};
};

typedef ULONG typeARGB32;

struct typeBGRA32{
    typeBGRA32(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    char b,g,r,a;
};

struct typeRGBA32{
    typeRGBA32(ULONG argb) : r((char)(argb>>16)),g((char)(argb>>8)),b((char)argb) {}
    char r,g,b,a;
};

template<typename SCREENPIXTYPE,typename CLUTTYPE,bool C_swapXY,bool useClut>
void directDrawScaleClutTT(directDrawScreen *screen,
                directDrawSource *source,
                LONG x1 ,
                LONG y1 ,
                WORD w ,
                WORD h ,
                CLUTTYPE *lut
            )
{
    UWORD wsobpr = source->_bpr>>1;
    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

    UWORD *psourcebm = (UWORD *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
    pscreenbm += (y1 * wscbpr) + x1;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if(sourceHeight<=0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if(sourceWidth<=0) return;

    LONG addw,addh;
    LONG vh ;
    LONG vxStart ;
    UWORD hmod;
    UWORD vmod;


    if(C_swapXY)
    {
        addw = (sourceHeight<<16)/w; // swapped source size
        addh = (sourceWidth<<16)/h; //
    } else
    {
        addw = (sourceWidth<<16)/w;
        addh = (sourceHeight<<16)/h;
    }
    vh = 0;
    vxStart = 0;
    hmod = (C_swapXY)?1:wsobpr;
    vmod = (C_swapXY)?wsobpr:1;
    if(source->_swapFlags & ORIENTATION_FLIP_Y)
    {
        vh = (C_swapXY)?(sourceWidth<<16)-1:(sourceHeight<<16)-1;
        addh = -addh;
    }
    if(source->_swapFlags & ORIENTATION_FLIP_X)
    {
        vxStart = (C_swapXY)?(sourceHeight<<16)-1:(sourceWidth<<16)-1;
        addw = -addw;
    }

    // clipping
    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }

    for(WORD hh=0;hh<h;)
    {
        UWORD *psoline = psourcebm + hmod*(vh>>16);
        UWORD *psoline2 = psourcebm + hmod*((vh+addh)>>16);
        UWORD *psoline3 = psourcebm + hmod*((vh+addh+addh)>>16);
        if(psoline == psoline3) // means 3 lines are the same.
        {
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                if(useClut)
                {
                    *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = lut[psoline[((UWORD)(vx>>16))*vmod]];
                } else
                {
                    *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = (SCREENPIXTYPE)psoline[((UWORD)(vx>>16))*vmod];
                }
                pscline++;
                vx += addw;
            }
            vh += addh*3;
            pscreenbm += wscbpr*3;
            hh +=3;
        } else
        if(psoline == psoline2) // means 2 lines are the same.
        {
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                if(useClut)
                {
                *pscline = pscline[wscbpr] = lut[psoline[((UWORD)(vx>>16))*vmod]];
                } else
                {
                *pscline = pscline[wscbpr] = (SCREENPIXTYPE)psoline[((UWORD)(vx>>16))*vmod];
                }
                pscline++;
                vx += addw;
            }
            vh += addh+addh;
            pscreenbm += wscbpr+wscbpr;
            hh +=2;
        } else
        {
            // other do normal one line ...
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                if(useClut)
                {
                    *pscline++ = lut[psoline[((UWORD)(vx>>16))*vmod]];
                } else
                {
                    *pscline++ = (SCREENPIXTYPE) psoline[((UWORD)(vx>>16))*vmod];
                }
                vx += addw;
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    } // end loop h

}
template<typename SCREENPIXTYPE,typename CLUTTYPE,bool C_swapXY,bool useClut>
void directDrawClutTT(directDrawScreen *screen,
                directDrawSource *source,
                LONG x1 ,
                LONG y1 ,
                WORD w ,
                WORD h ,
                CLUTTYPE *lut
            )
{
    UWORD wsobpr = source->_bpr>>1;
    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

    UWORD *psourcebm = (UWORD *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
    pscreenbm += (y1 * wscbpr) + x1;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if(sourceHeight<=0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if(sourceWidth<=0) return;

    // clipping
    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }


    LONG addw,addh;
    LONG vh =0;
    LONG vxStart=0 ;


    if(C_swapXY)
    {
        addw = wsobpr;
        addh = 1;
        if(source->_swapFlags & ORIENTATION_FLIP_Y)
        {
            vh = sourceWidth-1;
            addh = -1;
        }
        if(source->_swapFlags & ORIENTATION_FLIP_X)
        {
            vxStart = (sourceHeight-1)*wsobpr;
            addw = -wsobpr;
        }
        for(WORD hh=0;hh<h;)
        {
            UWORD *psoline = psourcebm + vh;

            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                if(useClut)
                *pscline++ = lut[psoline[vx]];
                else
                *pscline++ = (SCREENPIXTYPE) psoline[vx];
                vx += addw;
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    } else
    {
        addw = 1;
        addh = wsobpr;
        if(source->_swapFlags & ORIENTATION_FLIP_Y)
        {
            vh = (sourceHeight-1)*wsobpr;
            addh = -wsobpr;
        }
        if(source->_swapFlags & ORIENTATION_FLIP_X)
        {
            vxStart = sourceWidth/*-1*/; // ,no -1 because use --vx
            addw = -1;
        }

        for(WORD hh=0;hh<h;)
        {
            UWORD *psoline = psourcebm + vh;

            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            if(addw>0)
            {
                for(WORD ww=0;ww<w;ww++)
                {
                    if(useClut)
                    *pscline++ = lut[psoline[vx++]];
                    else
                    *pscline++ = (SCREENPIXTYPE)psoline[vx++];
                }
            } else
            {   // swap X
                for(WORD ww=0;ww<w;ww++)
                {
                    if(useClut)
                    *pscline++ = lut[psoline[--vx]];
                    else
                    *pscline++ = (SCREENPIXTYPE)psoline[--vx];
                }
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    }
}


template<typename SCREENPIXTYPE,typename CLUTTYPE,bool useClut>
void directDrawClutT(directDrawScreen *screen,
                directDrawSource *source,
                LONG x1 ,
                LONG y1 ,
                WORD w ,
                WORD h ,
                CLUTTYPE *lut
            )
{
    const bool doSwapXY = source->_swapFlags & ORIENTATION_SWAP_XY;

    WORD sourcewidth = source->_x2-source->_x1;
    WORD sourceheight = source->_y2-source->_y1;
    if(doSwapXY) doSwap(sourcewidth,sourceheight);

    const bool doScale = ((sourcewidth!=w) || (sourceheight!=h));
    if(doSwapXY)
        if(doScale) directDrawScaleClutTT<SCREENPIXTYPE,CLUTTYPE,true,useClut>( screen,source,x1,y1,w,h,lut);
        else  directDrawClutTT<SCREENPIXTYPE,CLUTTYPE,true,useClut>( screen,source,x1,y1,w,h,lut);
    else
        if(doScale) directDrawScaleClutTT<SCREENPIXTYPE,CLUTTYPE,false,useClut>( screen,source,x1,y1,w,h,lut);
        else  directDrawClutTT<SCREENPIXTYPE,CLUTTYPE,false,useClut>( screen,source,x1,y1,w,h,lut);

}



void directDrawClutT_UWORD_UWORD(directDrawScreen *screen , directDrawSource *source,LONG x1 ,LONG y1 ,LONG w, LONG h,UWORD *lut)
{
    directDrawClutT<UWORD,UWORD,true>(screen,source,x1,y1,w,h,lut);
}
void directDrawClutT_ULONG_ULONG(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,ULONG *lut)
{
    directDrawClutT<ULONG,ULONG,true>(screen,source,x1,y1,w,h,lut);
}
void directDrawClutT_type24_ULONG(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,ULONG *lut)
{
    directDrawClutT<type24,ULONG,true>(screen,source,x1,y1,w,h,lut);
}
void directDrawClutT_UBYTE_UBYTE(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h,UBYTE *lut)
{
    directDrawClutT<UBYTE,UBYTE,true>(screen,source,x1,y1,w,h,lut);
}

// - - --
void directDraw_UBYTE_UBYTE(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawClutT<UBYTE,UBYTE,false>(screen,source,x1,y1,w,h,0L);
}


























// - - - - - - -  - - -



















template<typename SCREENPIXTYPE,bool C_swapXY>
void directDrawRGB32TT(directDrawScreen *screen,
                directDrawSource *source,
                LONG x1 ,
                LONG y1 ,
                WORD w ,
                WORD h
            )
{
    UWORD wsobpr = source->_bpr>>2;
    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

    ULONG *psourcebm = (ULONG *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
    pscreenbm += (y1 * wscbpr) + x1;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if(sourceHeight<=0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if(sourceWidth<=0) return;

    // clipping
    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }


    LONG addw,addh;
    LONG vh =0;
    LONG vxStart=0 ;


    if(C_swapXY)
    {
        addw = wsobpr;
        addh = 1;
        if(source->_swapFlags & ORIENTATION_FLIP_Y)
        {
            vh = sourceWidth-1;
            addh = -1;
        }
        if(source->_swapFlags & ORIENTATION_FLIP_X)
        {
            vxStart = (sourceHeight-1)*wsobpr;
            addw = -wsobpr;
        }
        for(WORD hh=0;hh<h;)
        {
            ULONG *psoline = psourcebm + vh;

            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline++ = SCREENPIXTYPE(psoline[vx]);
                vx += addw;
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    } else
    {
        addw = 1;
        addh = wsobpr;
        if(source->_swapFlags & ORIENTATION_FLIP_Y)
        {
            vh = (sourceHeight-1)*wsobpr;
            addh = -wsobpr;
        }
        if(source->_swapFlags & ORIENTATION_FLIP_X)
        {
            vxStart = sourceWidth/*-1*/; // ,no -1 because use --vx
            addw = -1;
        }

        for(WORD hh=0;hh<h;)
        {
            ULONG *psoline = psourcebm + vh;

            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            if(addw>0)
            {
                for(WORD ww=0;ww<w;ww++)
                {
                    *pscline++ = SCREENPIXTYPE(psoline[vx++]);
                }
            } else
            {   // swap X
                for(WORD ww=0;ww<w;ww++)
                {
                    *pscline++ = SCREENPIXTYPE(psoline[--vx]);
                }
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    }
}

template<typename SCREENPIXTYPE,bool C_swapXY>
void directDrawRGB32ScaleTT(directDrawScreen *screen,
                directDrawSource *source,
                LONG x1 ,
                LONG y1 ,
                WORD w ,
                WORD h
            )
{
    UWORD wsobpr = source->_bpr>>2;
    UWORD wscbpr = screen->_bpr/sizeof(SCREENPIXTYPE);

    ULONG *psourcebm = (ULONG *)source->_base;
    psourcebm += (source->_y1 * wsobpr) + source->_x1;

    SCREENPIXTYPE *pscreenbm = (SCREENPIXTYPE *)screen->_base;
    pscreenbm += (y1 * wscbpr) + x1;

    LONG sourceHeight = (LONG)(source->_y2 - source->_y1);
    if(sourceHeight<=0) return;

    LONG sourceWidth = (LONG)(source->_x2 - source->_x1);
    if(sourceWidth<=0) return;

    LONG addw,addh;
    LONG vh ;
    LONG vxStart ;
    UWORD hmod;
    UWORD vmod;


    if(C_swapXY)
    {
        addw = (sourceHeight<<16)/w; // swapped source size
        addh = (sourceWidth<<16)/h; //
    } else
    {
        addw = (sourceWidth<<16)/w;
        addh = (sourceHeight<<16)/h;
    }
    vh = 0;
    vxStart = 0;
    hmod = (C_swapXY)?1:wsobpr;
    vmod = (C_swapXY)?wsobpr:1;
    if(source->_swapFlags & ORIENTATION_FLIP_Y)
    {
        vh = (C_swapXY)?(sourceWidth<<16)-1:(sourceHeight<<16)-1;
        addh = -addh;
    }
    if(source->_swapFlags & ORIENTATION_FLIP_X)
    {
        vxStart = (C_swapXY)?(sourceHeight<<16)-1:(sourceWidth<<16)-1;
        addw = -addw;
    }

    // clipping
    if(y1+h>screen->_clipY2) h=(screen->_clipY2)-y1;
    if(x1+w>screen->_clipX2) w=(screen->_clipX2)-x1;

    if(x1<screen->_clipX1) { w-=(screen->_clipX1-x1); x1=screen->_clipX1;  }
    if(y1<screen->_clipY1) { h-=(screen->_clipY1-y1); y1=screen->_clipY1;  }

    for(WORD hh=0;hh<h;)
    {
        ULONG *psoline = psourcebm + hmod*(vh>>16);
        ULONG *psoline2 = psourcebm + hmod*((vh+addh)>>16);
        ULONG *psoline3 = psourcebm + hmod*((vh+addh+addh)>>16);
        if(psoline == psoline3) // means 3 lines are the same.
        {
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline = pscline[wscbpr] = pscline[wscbpr<<1] = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);
                pscline++;
                vx += addw;
            }
            vh += addh*3;
            pscreenbm += wscbpr*3;
            hh +=3;
        } else
        if(psoline == psoline2) // means 2 lines are the same.
        {
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline = pscline[wscbpr] = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);

                pscline++;
                vx += addw;
            }
            vh += addh+addh;
            pscreenbm += wscbpr+wscbpr;
            hh +=2;
        } else
        {
            // other do normal one line ...
            SCREENPIXTYPE *pscline = pscreenbm;
            LONG vx = vxStart;
            for(WORD ww=0;ww<w;ww++)
            {
                *pscline++ = SCREENPIXTYPE(psoline[((UWORD)(vx>>16))*vmod]);
                vx += addw;
            }
            vh += addh;
            pscreenbm += wscbpr;
            hh++;
        }
    } // end loop h

}

template<typename SCREENPIXTYPE>
void directDrawRGB32T(directDrawScreen *screen,
                directDrawSource *source,
                LONG x1 ,
                LONG y1 ,
                WORD w ,
                WORD h
            )
{
    const bool doSwapXY = source->_swapFlags & ORIENTATION_SWAP_XY;

    WORD sourcewidth = source->_x2-source->_x1;
    WORD sourceheight = source->_y2-source->_y1;
    if(doSwapXY) doSwap(sourcewidth,sourceheight);

    const bool doScale = ((sourcewidth!=w) || (sourceheight!=h));
    if(doSwapXY)
        if(doScale) directDrawRGB32ScaleTT<SCREENPIXTYPE,true>( screen,source,x1,y1,w,h);
        else  directDrawRGB32TT<SCREENPIXTYPE,true>( screen,source,x1,y1,w,h);
    else
        if(doScale) directDrawRGB32ScaleTT<SCREENPIXTYPE,false>( screen,source,x1,y1,w,h);
        else  directDrawRGB32TT<SCREENPIXTYPE,false>( screen,source,x1,y1,w,h);

}


// - - - - - -

void directDraw_RGB15_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeRGB15>(screen,source,x1,y1,w,h);
}
void directDraw_BGR15_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeBGR15>(screen,source,x1,y1,w,h);
}
void directDraw_RGB15PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeRGB15PC>(screen,source,x1,y1,w,h);
}
void directDraw_BGR15PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeBGR15PC>(screen,source,x1,y1,w,h);
}

void directDraw_RGB16_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeRGB16>(screen,source,x1,y1,w,h);
}
void directDraw_BGR16_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeBGR16>(screen,source,x1,y1,w,h);
}
void directDraw_RGB16PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{

    directDrawRGB32T<typeRGB16PC>(screen,source,x1,y1,w,h);
}
void directDraw_BGR16PC_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeBGR16PC>(screen,source,x1,y1,w,h);
}

void directDraw_type24_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<type24>(screen,source,x1,y1,w,h);
}


void directDrawARGB32_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeARGB32>(screen,source,x1,y1,w,h);
}
void directDrawBGRA32_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeBGRA32>(screen,source,x1,y1,w,h);
}
void directDrawRGBA32_ARGB32(directDrawScreen *screen , directDrawSource *source,LONG x1, LONG y1 ,LONG w, LONG h)
{
    directDrawRGB32T<typeRGBA32>(screen,source,x1,y1,w,h);
}






