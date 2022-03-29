#include "pbm.h"
#include <stdlib.h>
PPMImage * new_ppmimage( unsigned int w, unsigned int h, unsigned int m );
PBMImage * new_pbmimage( unsigned int w, unsigned int h );
PGMImage * new_pgmimage( unsigned int w, unsigned int h, unsigned int m );
void del_ppmimage( PPMImage * p );
void del_pgmimage( PGMImage * p );
void del_pbmimage( PBMImage * p );

PPMImage * new_ppmimage( unsigned int w, unsigned int h, unsigned int m )
{
    PPMImage * ppm=malloc(sizeof(PPMImage));
    unsigned int ** pixm[3];
    for (int i = 0; i<3; i++){
        ppm->pixmap[i]=malloc(h*sizeof(unsigned int *));
        for (int r=0;r<h;r++){
        ppm->pixmap[i][r]=malloc(w*sizeof(unsigned int));
    }
    }
    ppm->height=h;
    ppm->max=m;
    ppm->width=w;
    return ppm;
    
}

PBMImage * new_pbmimage( unsigned int w, unsigned int h )
{
    PBMImage * pbm=malloc(sizeof(PBMImage));
    pbm->pixmap=malloc(h*sizeof(unsigned int *));
    for (int r=0;r<h;r++){
        pbm->pixmap[r]=malloc(w*sizeof(unsigned int));
    }
    pbm->height=h;
    pbm->width=w;
    return pbm;}

PGMImage * new_pgmimage( unsigned int w, unsigned int h, unsigned int m )
{
    PGMImage * pgm=malloc(sizeof(PGMImage));
    pgm->pixmap=malloc(h*sizeof(unsigned int *));
    for (int r=0;r<h;r++){
        pgm->pixmap[r]=malloc(w*sizeof(unsigned int));
    }
    pgm->height=h;
    pgm->max=m;
    pgm->width=w;
    return pgm;

}

void del_ppmimage( PPMImage * p )
{
    for (int i = 0; i<3;i++){
        for (int r=0;r<p->height;r++){
        free(p->pixmap[i][r]);
        }
        free(p->pixmap[i]);
    }
    free(p);
}

void del_pgmimage( PGMImage * p )
{
    for (int r=0;r<p->height;r++){
        free(p->pixmap[r]);
    }
    free(p);
}

void del_pbmimage( PBMImage * p )
{
    for (int r=0;r<p->height;r++){
        free(p->pixmap[r]);
    }
    free(p);
}

