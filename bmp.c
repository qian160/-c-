#include<stdio.h>

#include<malloc.h>

#define BM 19778        // 位图的标志

# ifndef BMP_H

# define BMP_H

typedef struct node

{

    unsigned char r;

    unsigned char g;

    unsigned char b;

}Point;



typedef struct tagBITMAPFILEHEADER

{

    unsigned int   bfSize;        // 文件大小 以字节为单位(2-5字节)

    unsigned short bfReserved1;   // 保留，必须设置为0 (6-7字节)

    unsigned short bfReserved2;   // 保留，必须设置为0 (8-9字节)

    unsigned int   bfOffBits;     // 从文件头到像素数据的偏移  (10-13字节)

} BITMAPFILEHEADER;



//图像信息头结构体

typedef struct tagBITMAPINFOHEADER

{

    unsigned int    biSize;          // 此结构体的大小 (14-17字节)

    long            biWidth;         // 图像的宽  (18-21字节)

    long            biHeight;        // 图像的高  (22-25字节)

    unsigned short  biPlanes;        // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)

    unsigned short  biBitCount;      // 一像素所占的位数，一般为24   (28-29字节)

    unsigned int    biCompression;   // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)

    unsigned int    biSizeImage;     // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)

    long            biXPelsPerMeter; // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)

    long            biYPelsPerMeter; // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)

    unsigned int    biClrUsed;       // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)

    unsigned int    biClrImportant;  // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)

} BITMAPINFOHEADER;

#endif

BITMAPFILEHEADER fileHeader;

BITMAPINFOHEADER infoHeader;

void getData(FILE* fp,Point **Pix,long height,long width,int offset)

{

    FILE* fpr;

    FILE* fpg;

    FILE* fpb;

   

    int i, j=0;

    int stride;

    unsigned char* pix=NULL;

    fseek(fp, offset, SEEK_SET);    //找到位图的数据区

   

    stride=(24*width+31)/8; //对齐,计算一行有多少个8位

    stride=stride/4*4;      //取四的倍数 r,g,b,alph

   

    //写入数组

    pix=(unsigned char *)malloc(stride);

    for(j=0;j<height;j++)

    {

       fread(pix, 1, stride, fp);

       for(i=0;i<width;i++)

        {

            Pix[height-j-1][i].r=pix[i*3+2];

            Pix[height-j-1][i].g=pix[i*3+1];

            Pix[height-j-1][i].b=pix[i*3];

        }

    }

   

    //写入文件

    fpr=fopen("bmpr1.txt","w+");

    fpg=fopen("bmpg1.txt","w+");

    fpb=fopen("bmpb1.txt","w+");

   

    for(i =0; i < height; i++)

     {

        for(j = 0; j < width; j++)

        {  

        fprintf(fpr,"%4d",Pix[i][j].r);

        fprintf(fpg,"%4d",Pix[i][j].g);

        fprintf(fpb,"%4d",Pix[i][j].b);

        }

        fprintf(fpr,"\n");

        fprintf(fpg,"\n");

        fprintf(fpb,"\n");

    }

     

    fclose(fpr);

    fclose(fpg);

    fclose(fpb);

         

}

int main()

{

    FILE* fp;

    long width1,height1;

    unsigned int offset;

    int i,j;

    fp = fopen("input2.bmp", "rb");//读取同目录下的image.bmp文件。

    if(fp == NULL)

    {

        printf("打开'input2.bmp'失败!\n");

        return -1;

    }

    unsigned short  fileType;

    fread(&fileType,1,sizeof (unsigned short), fp);  

    if (fileType = 0x4d42)

    {

        printf("文件类型标识正确!" );  

        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fp);

        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);

        offset=fileHeader.bfOffBits;

        height1=infoHeader.biWidth;

        width1=infoHeader.biHeight;

        Point **pix;

    //每一个像素申请内存

        pix = (Point **)malloc(sizeof(Point*)*height1);

        for(i=0;i<height1;i++)

        *(pix+i)=(Point*)malloc(sizeof(Point)*width1);

        getData(fp,pix,height1,width1,offset);

        fclose(fp);

        printf("r的位图矩阵\n");

        for(i=0;i<height1;i++){

            for ( j = 0; j < width1; j++)

                printf("%4d",pix[i][j].r);

            printf("\n");}

        printf("g的位图矩阵\n");

        for(i=0;i<height1;i++){

            for ( j = 0; j < width1; j++)

                printf("%4d",pix[i][j].g);

            printf("\n");}

        printf("b的位图矩阵\n");

        for(i=0;i<height1;i++){

            for ( j = 0; j < width1; j++)

                printf("%4d",pix[i][j].b);

            printf("\n");}

    }

} 
//作者：七月的小小剑 https://www.bilibili.com/read/cv15459784/ 出处：bilibili
