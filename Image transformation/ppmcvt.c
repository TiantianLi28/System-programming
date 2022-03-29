/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING CODE
WRITTEN BY OTHER STUDENTS.
Tiantian Li */
#include "pbm.h"
#include<getopt.h>
#include<stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    int op;
    int g_arg, n_arg, t_arg,r_arg, i_arg;
    char* input;
    char* output;
} options;

options processing(int argc, char *argv[]);
//PPMImage * read_in( options p);
PBMImage * convert_b( PPMImage * ppm);
PGMImage * convert_g( PPMImage * ppm, int m);
PPMImage * convert_i( PPMImage * ppm, int c);
PPMImage * convert_r( PPMImage * ppm, int c);
PPMImage * convert_s( PPMImage * ppm);
PPMImage * convert_m(PPMImage * ppm);
PPMImage * convert_t(PPMImage * ppm, int scale);
PPMImage * convert_n(PPMImage * ppm, int scale);



options processing(int argc, char *argv[]){
int o;
options opt;
opt.op=0;

while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:"))!=-1){
    switch(o){
        
        //convert to pbm
    case 'b':
    
        if (opt.op!=0){
            fprintf(stderr,"Error: Multiple transformations specified\n");
            exit(1);
        }
        
        opt.op='b';
        printf("hi");
        break;
        
    //convert to pgm; gray scale 1-65535
    case 'g':
        if (opt.op!=0){
            fprintf(stderr,"Error: Multiple transformations specified\n");
            exit(1);
        }
        int scale=strtol(optarg, NULL, 10);
        if (scale<1||scale>65535){
            fprintf(stderr,"Error: Invalid max grayscale pixel value: %d; must be less than 65,536\n", scale);
            exit(1);
        }
        opt.op='g';
        opt.g_arg=scale;
        break;
    //isolate specified RGB channel
    case 'i':
        if (opt.op!=0){
                fprintf(stderr,"Error: Multiple transformations specified\n");
                exit(1);
            }
        if (strcmp(optarg, "red")==0){opt.i_arg=0;}
        else if (strcmp(optarg, "green")==0){opt.i_arg=1;}
        else if (strcmp(optarg, "blue")==0){opt.i_arg=2;}
        else{
            fprintf(stderr,"Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n",optarg);
            exit(1);
            }
        opt.op='i';
        break;
    //remove specified RGB channel
    case 'r':
        if (opt.op!=0){
                fprintf(stderr,"Error: Multiple transformations specified\n");
                exit(1);
            }
        
        if (strcmp(optarg, "red")==0){opt.r_arg=0;}
        else if (strcmp(optarg, "green")==0){opt.r_arg=1;}
        else if (strcmp(optarg, "blue")==0){opt.r_arg=2;}
        else{
            fprintf(stderr,"Error: Invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n",optarg);
            exit(1);
            }
        
        opt.op='r';
        break;
    //sepia transformation
    case 's':
        if (opt.op!=0){
                    fprintf(stderr,"Error: Multiple transformations specified\n");
                    exit(1);
                }
        opt.op='s';
        break;
    //vertically mirror the first half to second half
    case 'm':
        if (opt.op!=0){
                    fprintf(stderr,"Error: Multiple transformations specified\n");
                    exit(1);
                }
        opt.op='m';
        break;
    //reduce to thumbnail based on scaling factor
    case 't':
        if (opt.op!=0){
            fprintf(stderr,"Error: Multiple transformations specified\n");
            exit(1);
                    }
        scale=strtol(optarg, NULL, 10);
        if (scale<1||scale>8){
            fprintf(stderr,"Error: Invalid scale factor: %d; must be 1-8\n", scale);
            exit(1);
        }
        opt.op='t';
        opt.t_arg=scale;
        break;
    //tile thubnail given scaling factor
    case 'n':
        if (opt.op!=0){
            fprintf(stderr,"error: Multiple transformations specified\n");
            exit(1);
                    }
        scale=strtol(optarg, NULL, 10);
        if (scale<1||scale>8){
            fprintf(stderr,"Error: Invalid scale factor: %d; must be 1-8\n", scale);
            exit(1);
        }
        opt.op='n';
        opt.n_arg=scale;
        break;
    //write to specified file
    case 'o':
        if(optarg[0] == '-'){
            fprintf(stderr,"Error: No output file specified\n");
            exit(1);
        }
        opt.output=optarg;
        
        break;

    default:
        fprintf(stderr,"Usage: ppmcvt [-bgirsmtno] [FILE]\n");
        exit(1);
    }
    }
    if(opt.op==0) {
        opt.op='b';
    }
    if(argc == optind){
        fprintf(stderr,"Error: No input file specified\n");
        exit(1);
    }
    else{opt.input=argv[optind];}
    return opt;
}

//option -b: convert to bitmap
PBMImage * convert_b( PPMImage * ppm){
    int h=ppm->height;
    int w=ppm->width;
    PBMImage * pbm= new_pbmimage(w,h);

    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            float total_rgb=ppm->pixmap[0][i][j]+ppm->pixmap[1][i][j]+ppm->pixmap[2][i][j];
            float avg_rgb=(float)total_rgb/3.0;
            float half=ppm->max/2.0;
            pbm->pixmap[i][j]=(avg_rgb>half)?0:1;
            
        }
    }
    return pbm;
}

//option -g: convert to pgm
PGMImage * convert_g( PPMImage * ppm, int m){
    int h=ppm->height;
    int w=ppm->width;
    PGMImage * pgm= new_pgmimage(w,h,m);
    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            float avg_rgb=(ppm->pixmap[0][i][j]+ppm->pixmap[1][i][j]+ppm->pixmap[2][i][j])/3.0;
            pgm->pixmap[i][j]=(int)((float)(avg_rgb/(ppm->max))*m);
        }
    }
    return pgm;
}

//option -i: isolate specified channel; output ppm image
PPMImage * convert_i( PPMImage * ppm, int c){
    int h=ppm->height;
    int w=ppm->width;
    int m=ppm->max;
    PPMImage * ppm_out= new_ppmimage(w,h,m);
    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            for (int k=0;k<3;k++){
                if (k==c){
                    ppm_out->pixmap[k][i][j]=ppm->pixmap[k][i][j];
                }
                else{
                      ppm_out->pixmap[k][i][j]=0;
                }
            }
        }
    }
    return ppm_out;
}

//option -r: isolate specified channel; output ppm image
PPMImage * convert_r( PPMImage * ppm, int c){
    int h=ppm->height;
    int w=ppm->width;
    int m=ppm->max;
    PPMImage * ppm_out= new_ppmimage(w,h,m);
    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            for (int k=0;k<3;k++){
                if (k!=c){
                    ppm_out->pixmap[k][i][j]=ppm->pixmap[k][i][j];
                }
                else{
                      ppm_out->pixmap[k][i][j]=0;
                }
            }
        }
    }
    return ppm_out;
}

//option -s: apply sepia transformation; output ppm image
PPMImage * convert_s( PPMImage * ppm){
    int h=ppm->height;
    int w=ppm->width;
    int m=ppm->max;
    PPMImage * ppm_out= new_ppmimage(w,h,m);
    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            double red=0.393*ppm->pixmap[0][i][j]+0.769*ppm->pixmap[1][i][j]+0.189*ppm->pixmap[2][i][j];
            double green=0.349*ppm->pixmap[0][i][j]+0.686*ppm->pixmap[1][i][j]+0.168*ppm->pixmap[2][i][j];
            double blue=0.272*ppm->pixmap[0][i][j]+0.534*ppm->pixmap[1][i][j]+0.131*ppm->pixmap[2][i][j];
            ppm_out->pixmap[0][i][j]=(int)red;
            ppm_out->pixmap[1][i][j]=(int)green;
            ppm_out->pixmap[2][i][j]=(int)blue;
            if(ppm_out->pixmap[0][i][j]>m){
                ppm_out->pixmap[0][i][j]=m;
            }
            if(ppm_out->pixmap[1][i][j]>m){
                ppm_out->pixmap[1][i][j]=m;
            }
            if(ppm_out->pixmap[2][i][j]>m){
                ppm_out->pixmap[2][i][j]=m;
            }
    }}
    return ppm_out;
}

//option -m: vertically reflect the left half to the right half; output ppm image
PPMImage * convert_m(PPMImage * ppm){
    int h=ppm->height;
    int w=ppm->width;
    int m=ppm->max;
    PPMImage * ppm_out= new_ppmimage(w,h,m);
    for (int i =0; i< h; i++){
        for(int j = 0; j <= (w-1)/2; j++){
            ppm_out->pixmap[0][i][w-1-j]=ppm->pixmap[0][i][j];
            ppm_out->pixmap[1][i][w-1-j]=ppm->pixmap[1][i][j];
            ppm_out->pixmap[2][i][w-1-j]=ppm->pixmap[2][i][j];
            ppm_out->pixmap[0][i][j]=ppm->pixmap[0][i][j];
            ppm_out->pixmap[1][i][j]=ppm->pixmap[1][i][j];
            ppm_out->pixmap[2][i][j]=ppm->pixmap[2][i][j];
        }
    }
    return ppm_out;
}

//option -t: shrinking the image by the scale given; output ppm image
PPMImage * convert_t(PPMImage * ppm, int scale){
    int h=ppm->height;
    int w=ppm->width;
    int m=ppm->max;
    if (w%scale!=0){
        w=w/scale+1;
    }
    else{
        w=w/scale;
    }
    if (h%scale!=0){
        h=h/scale+1;
    }
    else{
        h=h/scale;
    }
    PPMImage * ppm_out= new_ppmimage(w,h,m);
    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            ppm_out->pixmap[0][i][j]=ppm->pixmap[0][i*scale][j*scale];
            ppm_out->pixmap[1][i][j]=ppm->pixmap[1][i*scale][j*scale];
            ppm_out->pixmap[2][i][j]=ppm->pixmap[2][i*scale][j*scale];
            
        }
    }
    return ppm_out;
}

//option -n: tile 1/n scale thumbnails
PPMImage * convert_n(PPMImage * ppm, int scale){
    int h=ppm->height;
    int w=ppm->width;
    int m=ppm->max;
    PPMImage * ppm_out= new_ppmimage(w,h,m);
    int t_w, t_h;
    if (w%scale!=0){
        t_w=w/scale+1;
    }
    else{
        t_w=w/scale;
    }
    if (h%scale!=0){
        t_h=h/scale+1;
    }
    else{
        t_h=h/scale;
    }
    for (int i =0; i< h; i++){
        for(int j = 0; j < w; j++){
            ppm_out->pixmap[0][i][j]=ppm->pixmap[0][i%(t_h)*scale][j%(t_w)*scale];
            ppm_out->pixmap[1][i][j]=ppm->pixmap[1][i%(t_h)*scale][j%(t_w)*scale];
            ppm_out->pixmap[2][i][j]=ppm->pixmap[2][i%(t_h)*scale][j%(t_w)*scale];
        }
    }
    return ppm_out;
}

int main( int argc, char *argv[] )
{   
    options opt=processing(argc, argv);
    
    PPMImage *ppm=read_ppmfile(opt.input);
    
    if (opt.op=='b'){
       
        PBMImage * converted=convert_b(ppm);
        
        write_pbmfile(converted, opt.output);
        del_pbmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='g'){
        PGMImage * converted=convert_g(ppm, opt.g_arg);
        write_pgmfile(converted, opt.output);
        del_pgmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='i'){
        PPMImage * converted=convert_i(ppm, opt.i_arg);
        write_ppmfile(converted, opt.output);
        del_ppmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='r'){
        PPMImage * converted=convert_r(ppm, opt.r_arg);
        write_ppmfile(converted, opt.output);
        del_ppmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='s'){
        PPMImage * converted=convert_s(ppm);
        write_ppmfile(converted, opt.output);
        del_ppmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='m'){
        PPMImage * converted=convert_m(ppm);
        write_ppmfile(converted, opt.output);
        del_ppmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='t'){
        PPMImage * converted=convert_t(ppm, opt.t_arg);
        write_ppmfile(converted, opt.output);
        del_ppmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    else if (opt.op=='n'){
        PPMImage * converted=convert_n(ppm, opt.n_arg);
        write_ppmfile(converted, opt.output);
        del_ppmimage(converted);
        del_ppmimage(ppm);
        exit(0);
    }
    
    return 0;
    

}
