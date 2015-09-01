#ifdef __cplusplus
extern "C"
{
#endif

#include "videodev.h"
#include <string.h>
typedef unsigned short      WORD;      /* WORD (无符号短整型) */
typedef unsigned long       DWORD;     /* DOUBLE WORD (无符号长整形)*/

typedef struct tagBITMAPFILEHEADER{
     WORD    bfType;                // the flag of bmp, value is "BM"
     DWORD    bfSize;                // size BMP file ,unit is bytes
     DWORD    bfReserved;            // 0
     DWORD    bfOffBits;             // must be 54

}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER{
     DWORD    biSize;                // must be 0x28£¨40£©
     DWORD    biWidth;           //
     DWORD    biHeight;          //
     WORD        biPlanes;          // must be 1
     WORD        biBitCount;            //1,4,8£¨»Ò¶È£©,24£¨²ÊÉ«£©
     DWORD    biCompression;         //0
     DWORD    biSizeImage;       //Í¼Ïñ´óÐ¡
     DWORD    biXPelsPerMeter;   //0
     DWORD    biYPelsPerMeter;   //0
     DWORD    biClrUsed;             //0
     DWORD    biClrImportant;        //0
}BITMAPINFOHEADER;

void *CreateCameraContext();
void DestoryCameraCotext(void* v4l2ctx);

int OpenCamera(void* v4l2ctx, char *pname);
void CloseCamera(void* v4l2ctx);

int StartCamera(void* v4l2ctx, int *width, int *height, int Fps);
void StopCamera(void* v4l2ctx);

int CameraGetOneFrame(void*  v4l2ctx, struct v4l2_buffer *buffer);
void CameraReturnOneFame(void* v4l2ctx, struct v4l2_buffer *buffer);

void Yuv_2_Rgb(void* v4l2,unsigned char *Frame_buffer);
void InitBMP(void* v4l2ctx, BITMAPINFOHEADER* bi, BITMAPFILEHEADER* bf);
void ImageSave2Bmp(void* v4l2ctx, BITMAPINFOHEADER* bi, BITMAPFILEHEADER* bf, unsigned char *Frame_Buffer);

#ifdef __cplusplus
}
#endif
