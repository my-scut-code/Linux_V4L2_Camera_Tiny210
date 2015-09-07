#include "camera.h"
#include "V4L2.h"
#include "decoder.h"
#include "decoder_mjpeg.h"


#include <errno.h>
#include <string.h>
#define __DEBUG__
void main()
{
	int ret = 0;
	void * v4l2ctx = NULL;
	int height = 240;
	int width = 320;
	int fps = 10;
	unsigned char *Frame_Buffer, *Jpeg_Buffer;
	
	Decoder *decoder = decoder_mjpeg_create();
	
	Frame_Buffer = (unsigned char *) malloc(sizeof(unsigned char)*240*320*3);
	Jpeg_Buffer = (unsigned char *)malloc(sizeof(unsigned char) * 240*320*2);

	v4l2ctx = CreateCameraContext();
#ifdef __DEBUG__
	printf("CrateCameractx finish!!\n");
#endif
	struct v4l2_buffer buff;
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	memset(&buff, 0, sizeof(struct v4l2_buffer));
	memset(&bf, 0, sizeof(BITMAPFILEHEADER));
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
#ifdef __DEBUG__
	printf("Before OPEN Camera in main!\n");
#endif
	
	ret = OpenCamera(v4l2ctx, "/dev/video0");
	if (0 != ret)
	{
		perror("Can't open Camera!\n");
	}
#ifdef __DEBUG__
	printf("After OPEN Camera in main!\n");
#endif
	StartCamera(v4l2ctx, &width, &height, fps);
#ifdef __DEBUG__
	printf("After startcamera in main!\n");
#endif
	CameraGetOneFrame(v4l2ctx, &buff);
#ifdef __DEBUG__
	printf("After CameraGetOneFram in main!\n");
#endif
	int cnt = ((V4L2_CONTEXT*)v4l2ctx)->mBufferCnt;
	int length = ((V4L2_CONTEXT*)v4l2ctx)->mMapMem.length;
	memcpy(Jpeg_Buffer, ((V4L2_CONTEXT*)v4l2ctx)->mMapMem.mem[cnt], length);
#ifdef __DEBUG__
	printf("the buff legth is %d !!\nthe buffcnt is %d!!\n",length, cnt);
#endif
	decoder_decode(decoder, Frame_Buffer, Jpeg_Buffer, length);
#ifdef __DEBUG__
	printf("IMG decompress finish!\n");
#endif
	InitBMP(v4l2ctx, &bi, &bf);
#ifdef __DEBUG__
	printf("Before ImageSave_2_Bmp in main!\n");
#endif
	ImageSave_2_Bmp(v4l2ctx, &bi, &bf, Frame_Buffer);
#ifdef __DEBUG__
	printf("After ImageSave_2_Bmp in main!\n");
#endif
	CameraReturnOneFame(v4l2ctx, &buff);
#ifdef __DEBUG__
	printf("Before StopCamera in main!\n");
#endif
	StopCamera(v4l2ctx);
#ifdef __DEBUG__
	printf("Before CloseCameraDevice in main!\n");
#endif
	CloseCamera(v4l2ctx);
	DestoryCameraCotext(v4l2ctx);

	v4l2ctx = NULL;
	free(Frame_Buffer);
	free(Jpeg_Buffer);

//	return 0;

}
