#ifdef __cplusplus
extern "C"
{
#endif

#include "videodev.h"

#define MAX_BUFFER_NUM 4
#define MAX_DEVICE_NAME_LENGTH 24

typedef struct v4l2_mem_map_t
{
	void * mem[MAX_BUFFER_NUM];
	int length;
}v4l2_mem_map_t;

typedef struct V4L2_CONTEXT
{
	int mCamFd;
	int mDeviceID;
	int mBufferCnt;
	int mCaptureFormat;
	char mDeviceName[MAX_DEVICE_NAME_LENGTH];
	int width;
	int height;
	v4l2_mem_map_t mMapMem;
}V4L2_CONTEXT;

void *CreamV4l2Context();
void DestoryV4l2Context(void* v4l2ctx);

int OpenCameraDevice(void* v4l2ctx, char *pDeviceName);
void CloseCameraDevice(void *v4l2ctx);

int setV4l2DeviceParam(void* v4l2ctx, int *width, int *height);
int setV4l2FrameParam(void* v4l2ctx, struct v4l2_streamparm params);

int V4l2ReqBuffer(void* v4l2ctx, int *buffernum);
int V4l2MapBuffer(void* v4l2ctx);
int V4l2StopMapBuffer(void* v4l2ctx);

int GetFrame(void* v4l2ctx, struct v4l2_buffer *buff);
void ReturnFrame(void* v4l2ctx, struct v4l2_buffer *buff);

int StarStreaming(void* v4l2ctx);
void StopStream(void* v4l2ctx);

#ifdef __cplusplus 
}
#endif



