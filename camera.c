#include "camera.h"
#include "V4L2.h"
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define NUM_BUFFERNUM 2
#define BMP "/home/plg/image_bmp.bmp"
#define YUV "/home/plg/image_yuv.yuv"

#define __DEBUG__

void *CreateCameraContext()
{
	void* v4l2_context = CreamV4l2Context(); 
	return v4l2_context;
}
void DestoryCameraCotext(void* v4l2ctx)
{
	DestoryV4l2Context(v4l2ctx);
}

int OpenCamera(void* v4l2ctx, char *pname)
{
	return OpenCameraDevice(v4l2ctx, pname);
}

void CloseCamera(void* v4l2ctx)
{
	CloseCameraDevice(v4l2ctx);
}

int StartCamera(void* v4l2ctx, int *width, int *height, int Fps)
{
	int ret = -1;
	int buffernum = NUM_BUFFERNUM;
	struct v4l2_streamparm params;
	params.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	params.parm.capture.capturemode = V4L2_MODE_VIDEO;
	params.parm.capture.timeperframe.denominator = Fps;
	params.parm.capture.timeperframe.numerator = 1;

	//setV4l2FrameParam(v4l2ctx, params);

	setV4l2DeviceParam(v4l2ctx, width, height);

	V4l2ReqBuffer(v4l2ctx, &buffernum);

	V4l2MapBuffer(v4l2ctx);

	StarStreaming(v4l2ctx);

	return 0;
}
void StopCamera(void* v4l2ctx)
{
	StopStream(v4l2ctx);
#ifdef __DEBUG__
	printf("had stopstream ! \n");
#endif
	V4l2StopMapBuffer(v4l2ctx);
}

int CameraGetOneFrame(void*  v4l2ctx, struct v4l2_buffer *buffer)
{
	int ret;

	struct v4l2_buffer buff;
	memset(&buff, 0, sizeof(struct v4l2_buffer));

	ret = GetFrame(v4l2ctx, &buff);
	if (-1 == ret)
	{
		perror("it's can't get frame in camera.c CameraGetOneFramne!");
		return ret;
	}

	memcpy(buffer, &buff, sizeof(struct v4l2_buffer));
	return 0;
}


void CameraReturnOneFame(void* v4l2ctx, struct v4l2_buffer *buffer)
{
	ReturnFrame(v4l2ctx, buffer);
}

void Yuv_2_Rgb(void* v4l2ctx, unsigned char *Frame_buffer)
{
	int i, j;
	int y1, y2, u, v;
	int r1, b1, g1, r2, b2, g2;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;

	int height = V4l2_Context->height;
	int width = V4l2_Context->width;
	char *point = (char) V4l2_Context->mMapMem.mem[0];

	for(i = 0; i < height; i++)
	{
		for (j = 0; j < (width>>2); j++)
		{
			y1 = *(point + (i*width+j)*4);
			u  = *(point + (i*width+j)*4 + 1);
			y2 = *(point + (i*width+j)*4 + 2);
			v  = *(point + (i*width+j)*4 + 3);

			r1 = y1 + 1.042 * (v - 128);
			g1 = y1 - 0.34414 * (u - 128) - 0.71414 * (v - 128);
			b1 = y1 + 1.772 * (u - 128);

			r2 = y2 + 1.042 * (v - 128);
			g2 = y2 - 0.34414 * (u - 128) - 0.71414 * (v - 128);
			b2 = y2 + 1.772 * (u - 128);

			if (r1<0)
			{
				r1 = 0;
			}
			else if(r1>255)
			{
				r1 = 255;
			}
			if (r2<0)
			{
				r2 = 0;
			}
			else if (r2>255)
			{
				r2 = 255;
			}
			if (g1<0)
			{
				g1 = 0;
			}
			else if (g1>255)
			{
				g1 = 255;
			}
			if (g2<0)
			{
				g2 = 0;
			}
			else if (g2>255)
			{
				g2 = 255;
			}
			if (b1<0)
			{
				b1 = 0;
			}
			else if (b1>255)
			{
				b1 = 255;
			}
			if (b2<0)
			{
				b2 = 0;
			}
			else if (b2>255)
			{
				b2 = 255;
			}

			*(Frame_buffer + ((height-1-1)*width + j)*6 )    = (unsigned char) b1;
			*(Frame_buffer + ((height-1-1)*width + j)*6 + 1) = (unsigned char) g1;
			*(Frame_buffer + ((height-1-1)*width + j)*6 + 2) = (unsigned char) r1;
			*(Frame_buffer + ((height-1-1)*width + j)*6 + 3) = (unsigned char) b2;
			*(Frame_buffer + ((height-1-1)*width + j)*6 + 4) = (unsigned char) g2;
			*(Frame_buffer + ((height-1-1)*width + j)*6 + 5) = (unsigned char) r2;
		}
	}
}

static void RGB2BGR(void* v4l2ctx, unsigned char *Frame_buffer)
{
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;

	int height = V4l2_Context->height;
	int width = V4l2_Context->width;
	int r,g,b;
	int i,j;
	unsigned char * buf;
	buf = (unsigned char *)malloc(width*height*3*sizeof(unsigned char));
	if (NULL == buf)
	{
		perror("don't have enough space!!\n");
	}
	memset(buf, 0, (width*height*sizeof(unsigned char)));	
	//memcpy(buf, Frame_buffer,(width*height*3*sizeof(unsigned char)));
	for(i=0; i < height; i++)
	{
		for(j=0; j < width; j++)
		{
			r = *(Frame_buffer + (i * width +j) * 3);
			g = *(Frame_buffer + (i * width + j) * 3 + 1);
			b = *(Frame_buffer + (i * width + j) * 3 + 2);
			*(buf + ((height-i-1)*width+j)*3) = b;
			*(buf + ((height-i-1)*width+j)*3+1) = g;
			*(buf + ((height-i-1)*width+j)*3+2) = r;
		}
	}
	memcpy(Frame_buffer, buf, (width*height*3*sizeof(unsigned char)));
	free(buf);
}


void InitBMP(void* v4l2ctx, BITMAPINFOHEADER* bi, BITMAPFILEHEADER* bf)
{
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	bi->biSize = 40;
	bi->biWidth = V4l2_Context->width;
	bi->biHeight = V4l2_Context->height;
	bi->biPlanes = 1;
	bi->biBitCount = 24;
	bi->biCompression = 0;
	bi->biSizeImage = V4l2_Context->width * V4l2_Context->height * 3;
	bi->biXPelsPerMeter = 0;
	bi->biYPelsPerMeter = 0;
	bi->biClrImportant = 0;
	bi->biClrUsed = 0;


	bf->bfOffBits = 54;
	bf->bfSize = 54 + bi->biSizeImage;
	bf->bfType =0x4d42;

}


void ImageSave_2_Bmp(void* v4l2ctx, BITMAPINFOHEADER* bi, BITMAPFILEHEADER* bf, unsigned char *Frame_Buffer)
{
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	FILE *fp1;

	fp1 = fopen(BMP, "wb");
	if (NULL == fp1)
	{
		perror("open BMP error!!");
	}
	/*
	fp2 = fopen(YUV, "wb");
	if (NULL == fp2)
	{
		perror("open YUV error");
	}
	
	fwrite(V4l2_Context->mMapMem.mem[0], V4l2_Context->width*V4l2_Context->height*2, 1, fp2);
	printf("YUV write finish!\n");
	*/
	//Yuv_2_Rgb(v4l2ctx, Frame_Buffer);
	RGB2BGR(v4l2ctx, Frame_Buffer);
	fwrite(&bf, 14, 1, fp1);
	fwrite(&bi, 40, 1, fp1);
	fwrite(Frame_Buffer, bi->biSizeImage, 1, fp1);
	printf("BMP write finish!\n");

	fclose(fp1);
	//fclose(fp2);
}

/*
int main(_In_ int _Argc, _In_count_(_Argc) _Pre_z_ char ** _Argv, _In_z_ char ** _Env)
{
	int ret = 0;
	void * v4l2ctx = NULL;
	int height = 720;
	int width = 1280;
	int fps = 24;

	v4l2ctx = CreateCameraContext();

	struct v4l2_buffer buff;
	memset(buff, 0 , sizeof(struct v4l2_buffer));

	ret = OpenCamera(v4l2ctx, "dev/video0");
	if (0 != ret)
	{
		perror("Can't open Camera!\n");
		return ret;
	}

	StartCamera(v4l2ctx);

	CameraGetOneFrame(v4l2ctx, buff);
	CameraReturnOneFame(v4l2ctx, buff.index);

	StopCamera(v4l2ctx);

	CloseCameraDevice(v4l2ctx);
	DestoryCameraCotext(v4l2ctx);

	v4l2ctx = NULL;

	return 0;

}
*/
