#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.141592654
#define DIMENTION 8 // ����ȭ ���̺��� 8 by 8�� ����
#define twoN (DIMENTION*2)

unsigned char **UCalloc(int width, int height)
{
 int i, j;
 unsigned char **ptr;
 
 if( ( ptr = (unsigned char**)malloc(height * sizeof(unsigned char*)) ) == NULL )
 {
  printf("\nMemory allocation failure\n");
  exit(1);
 }
 
 for(i=0; i<height; i++)
 {
  if( ( ptr[i] = (unsigned char*)malloc(width * sizeof(unsigned char)) ) == NULL )
  {
   printf("\nMemory allocation failure\n");
   exit(1);
  }
 }
 
 for(i=0; i<height; i++)
  for(j=0; j<width; j++)
   ptr[i][j] = 0;
  
  printf("\nMEMORY ALLOCATION(UNSIGNED CHAR) OK!\n");
  return ptr;
}

void UCfree(unsigned char **ptr, int height)
{
 int i;
 for(i=0; i<height; i++)
  free(ptr[i]);
 
 free(ptr);
 
 printf("\nMEMORY FREE(UNSIGNED CHAR) OK!\n");
}


void Readfile(char *filename, unsigned char **source, int width, int height)
{
 int i, j;
 FILE *readf;
 
 if ( (readf = fopen(filename, "rb")) == NULL )
 {
  fprintf(stderr, "ERROR : File cannot open : %s \n", filename);
  exit(-1);
 }
 
 for(i=0; i<height; i++) 
  for(j=0; j<width; j++)
   source[i][j] = (unsigned char)getc(readf);
  
  printf("\n%sFILE READING OK!\n",filename);
  fclose(readf);
}

void Writefile(char *filename, unsigned char **result, int width, int height)
{
 int i, j;
 FILE *writef;
 
 if ( (writef = fopen(filename, "wb")) == NULL )
 {
  fprintf(stderr, "ERROR : File cannot open : %s \n", filename);
  exit(-1);
 }
 
 for(i=0; i<height; i++)
  for(j=0; j<width; j++)
   putc((unsigned char)result[i][j],writef);
  
  printf("\n%sFILE WRITING OK!\n",filename);
  fclose(writef);
}

double trans[256][256];
double transcp[256][256];
int qtrans[256][256];
double trans1[256][256];
/*
int quan8[8][8] ={
	{8 ,16, 19, 22, 26, 27, 29, 34},
	{16, 16, 22, 24, 27, 29, 34, 37},
	{19, 22, 26, 27, 29, 34, 34, 38},
	{22, 22, 26, 27, 29, 34, 37, 40},
	{22, 26, 27, 29, 32, 35, 40, 48},
	{26, 27, 29, 32, 35, 40, 48, 58},
	{26, 27, 29, 34, 38, 46, 56, 69},
	{27, 29, 35, 38, 46, 56, 69, 83}
};
*/
int quan8[8][8] ={
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16},
	{16, 16, 16, 16, 16, 16, 16, 16}
};

int main(void)
{
 char imagename[100];
 char imagenames[100];
 char imagefullname[100];
 unsigned char **image,**dct;
 int i,j,k,l,m,n;
 double cu,cv,result,MSE,SUM,PSNR = 0;
 
 sprintf(imagename,"lena"); // ���ϸ��Է�
 sprintf(imagefullname,"%s.pgm",imagename); // ������ Ȯ���� �Է�
 
 image = UCalloc(256, 256);
 dct = UCalloc(256, 256);
 
 Readfile(imagefullname,image,256,256);
 
 //Forward DCT
 for(i=0;i<(256/DIMENTION);i++)
 {
  for(j=0;j<(256/DIMENTION);j++)
  {
   for(k=0;k<DIMENTION;k++)
   {
    for(l=0;l<DIMENTION;l++)
    {
     result=0.0;
     for(m=0;m<DIMENTION;m++)
     {
      for(n=0;n<DIMENTION;n++)
      {
       result += (double)cos((((2*(double)m+1)*(double)k*PI)/(DIMENTION*2)))*cos((((2*(double)n+1)*(double)l*PI)/(DIMENTION*2)))*(double)image[(i*DIMENTION)+m][(j*DIMENTION)+n];
       //printf("%f\n",result);
      }
     }
     if(k==0)
      cu=1/sqrt((long double)2);
     else
      cu=1;
     
     if(l==0)
      cv=1/sqrt((long double)2);
     else
      cv=1;
     //printf("%f %f\n",cu,cv);
     trans[(i*DIMENTION)+k][(j*DIMENTION)+l] = result*((cu*cv)/4);
     //printf("%f\n",trans[(i*DIMENTION)+k][(j*DIMENTION)+l]);
    }
   }
  }
 }
 
 // DCT�� ���� ���
 for(i=0;i<256;i++)
 {
  for(j=0;j<256;j++)
  {
   //printf("%f\n",trans[i][j]);
   
   if(trans[i][j]<0)
    transcp[i][j] = trans[i][j]*(-1);
   else
    transcp[i][j] = trans[i][j];
   
   transcp[i][j] = transcp[i][j]*10;
   
   if(transcp[i][j]>255)
    transcp[i][j]=255;
   
   //printf("%f\n",transcp[i][j]);
   
   //if((i==0&&j==0)||(PSNR<transcp[i][j]))
   // PSNR = trans[i][j];
   
   dct[i][j] = (unsigned char)floor(transcp[i][j]+0.5);
  }
 }
 
 //printf("%f\n",PSNR);
 
 sprintf(imagenames,"%s_ForDCT_%d.raw",imagename,DIMENTION);
 Writefile(imagenames,dct,256,256);
 
 //Quantization
 for(i=0;i<(256/DIMENTION);i++)
 {
  for(j=0;j<(256/DIMENTION);j++)
  {
   for(k=0;k<DIMENTION;k++)
   {
    for(l=0;l<DIMENTION;l++)
    {
     qtrans[(i*DIMENTION)+k][(j*DIMENTION)+l] = (int)(trans[(i*DIMENTION)+k][(j*DIMENTION)+l]/quan8[k][l]);
    // printf("%d ",qtrans[(i*DIMENTION)+k][(j*DIMENTION)+l]);
    // printf("%d\n",(j*DIMENTION)+l);
    // printf("%d\n",quan8[k][l]);
    }
  //  printf("\n");
   }
  }
 }
 
 // Quantization�� ���� ���
 for(i=0;i<256;i++)
 {
  for(j=0;j<256;j++)
  {
   //printf("%f\n",trans[i][j]);
   
   if(qtrans[i][j]<0)
    transcp[i][j] = qtrans[i][j]*(-1);
   else
    transcp[i][j] = qtrans[i][j];
   
   transcp[i][j] = transcp[i][j]*16;
   
   if(transcp[i][j]>255)
    transcp[i][j]=255;
   
   //printf("%f\n",transcp[i][j]);
   
   //if((i==0&&j==0)||(PSNR<transcp[i][j]))
   // PSNR = trans[i][j];
   
   dct[i][j] = (unsigned char)floor(transcp[i][j]+0.5);
  }
 }
 
 //printf("%f\n",PSNR);
 
 sprintf(imagenames,"%s_Quan_%d.raw",imagename,DIMENTION);
 Writefile(imagenames,dct,256,256);
 
 //Dequantization
 for(i=0;i<(256/DIMENTION);i++)
 {
  for(j=0;j<(256/DIMENTION);j++)
  {
   for(k=0;k<DIMENTION;k++)
   {
    for(l=0;l<DIMENTION;l++)
    {
     trans1[(i*DIMENTION)+k][(j*DIMENTION)+l] = (double)(qtrans[(i*DIMENTION)+k][(j*DIMENTION)+l]*quan8[k][l]);
    }
   }
  }
 }
 
 // Dequantization�� ���� ���
 for(i=0;i<256;i++)
 {
  for(j=0;j<256;j++)
  {
   //printf("%f\n",trans[i][j]);
   
   if(trans1[i][j]<0)
    transcp[i][j] = trans1[i][j]*(-1);
   else
    transcp[i][j] = trans1[i][j];
   
   transcp[i][j] = transcp[i][j]*10;
   
   if(transcp[i][j]>255)
    transcp[i][j]=255;
   
   //printf("%f\n",transcp[i][j]);
   
   //if((i==0&&j==0)||(PSNR<transcp[i][j]))
   // PSNR = trans[i][j];
   
   dct[i][j] = (unsigned char)floor(transcp[i][j]+0.5);
  }
 }
 
//printf("%f\n",PSNR);
 
 sprintf(imagenames,"%s_Dequan_%d.raw",imagename,DIMENTION);
 Writefile(imagenames,dct,256,256);
 UCfree(dct,256);
 
 
 //Inverse DCT
 for(i=0;i<(256/DIMENTION);i++)
 {
  for(j=0;j<(256/DIMENTION);j++)
  {
   for(m=0;m<DIMENTION;m++)
   {
    for(n=0;n<DIMENTION;n++)
    {
     result=0.0;
     for(k=0;k<DIMENTION;k++)
     {
      for(l=0;l<DIMENTION;l++)
      {
       if(k==0)
        cu=1/sqrt((long double)2);
       else
        cu=1;
       
       if(l==0)
        cv=1/sqrt((long double)2);
       else
        cv=1;
       result += ((cu*cv)/4)*(double)cos((((2*(double)m+1)*(double)k*PI)/(DIMENTION*2)))*cos((((2*(double)n+1)*(double)l*PI)/(DIMENTION*2)))*trans1[(i*DIMENTION)+k][(j*DIMENTION)+l];
  //     printf("%f\n",result);
      }
     }
     
 //    printf("%f %f\n",cu,cv);
     image[(i*DIMENTION)+m][(j*DIMENTION)+n] = (unsigned char)floor(result+0.5);
     //printf("%d\n",image[(i*DIMENTION)+m][(j*DIMENTION)+n]);
    }
   }
  }
 }
 sprintf(imagenames,"%s_InvDCT_%d.raw",imagename,DIMENTION);
 Writefile(imagenames,image,256,256);
 
 UCfree(image,256);
 
 // PSNR ���
 image = UCalloc(256, 256);
 dct = UCalloc(256, 256);
 
 Readfile(imagefullname,image,256,256);
 
 sprintf(imagenames,"%s_InvDCT_%d.raw",imagename,DIMENTION);
 Readfile(imagenames,dct,256,256);
 
 MSE=0.0;
 SUM=0.0;
 
 for(i=0;i<256;i++)
 {
  for(j=0;j<256;j++)
  {
   SUM += ((int)image[i][j]-(int)dct[i][j])*((int)image[i][j]-(int)dct[i][j]);
  }
 }
 MSE = SUM/262144.0;
 PSNR = 10.0*log10((255.0*255.0)/MSE);
 printf("PSNR : %f\n\n",PSNR); 
 
 
 int count = 0;
 // ������ ������ �׸� ��
 k=1;
 for(i=0;i<256;i++)
 {
  for(j=0;j<256;j++)
  {
   if (image[i][j]!=dct[i][j])
   {
   // printf("�� ������ %d�� %d�� �ȼ��� Ʋ���ϴ�.\n",i+1,j+1);
	   count++;
    k=0;
   }
  }
 }
 
 // �� �� ���Ἲ ���� ��ǥ
 if (k==0)
 {
  printf("������ ������ �̹����� ���� �ʽ��ϴ�.\n\n");
 printf("�ٸ� �ȼ��� : %d\n\n",count);
 }
 else
  printf("������ ������ �̹����� �����մϴ�.\n\n");
 
 return 0;
}



