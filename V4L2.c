#include "V4L2.h"
#include <malloc.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h> 
#include<unistd.h>  
#define __DEBUG__

#ifdef __cplusplus 
extern "C"
{
#endif

void *CreamV4l2Context()
{
	struct V4L2_CONTEXT *v4l2ctx;
	
	v4l2ctx = (V4L2_CONTEXT*) malloc(sizeof(struct V4L2_CONTEXT));
	if(NULL == v4l2ctx)
	{
		perror("error in struct V4l2_CONTEXT Init!");
	}
	memset(v4l2ctx, 0, sizeof(struct V4L2_CONTEXT));

	strcpy(v4l2ctx->mDeviceName,  "/dev/video0");
	v4l2ctx->mDeviceID = 0;
	v4l2ctx->mCaptureFormat = V4L2_PIX_FMT_MJPEG;
#ifdef __DEBUG__
	printf("success creat the v4l2_context\n");
#endif
	return (void*) v4l2ctx;
}
void DestoryV4l2Context(void* v4l2ctx)
{
	if (NULL != v4l2ctx)
	{
		free(v4l2ctx);
		v4l2ctx = NULL;	
	}
}

int OpenCameraDevice(void* v4l2ctx, char *pDeviceName)
{
	int ret = -1;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;

	strncpy(V4l2_Context->mDeviceName, pDeviceName, strlen(pDeviceName));

	V4l2_Context->mCamFd = open(V4l2_Context->mDeviceName, O_RDWR, 0);
	if (-1 == V4l2_Context->mDeviceID)
	{
		perror("Can't open the camera in OpenCameraDevice!!\n");
		return -1;
	}
#ifdef __DEBUG__
	printf("Open Camera in V4L2_OpenCameraDevice!\n");
#endif
	/*
	if (-1 == ioctl(V4l2_Context->mCamFd, VIDIOC_S_INPUT, V4l2_Context->mDeviceID))
	{
		perror("VIDIOC_S_INPUT Error in OpenCameraDevice!\n");
		goto ErrorEnd;
	}
#ifdef __DEBUG__
	printf("Set VIDIOC_S_INPUT OK in V4l2_OCameraDevice!\n");
#endif
	*/
	struct v4l2_capability cap;
	ret = ioctl (V4l2_Context->mCamFd, VIDIOC_QUERYCAP, &cap); 
	if (ret < 0)
	{
		perror("VIDIOC_QUERYCAP Error in OpenCameraDevice!\n");
		goto ErrorEnd;
	}
#ifdef __DEBUG__
	printf("Set VIDIOC_QUERYCAP in V4L2_OpenCameraDevice!\n");
	printf("driver:\t\t%s\n", cap.driver);
	printf("capabilities:\t%x\n", cap.capabilities);
	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
	{
		printf("Device %s:supports capture.\n", pDeviceName);
	}
	if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
	{
		printf("Device : support streaming.\n");
	}
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmtdesc.flags = V4L2_FMT_FLAG_EMULATED;
	printf("Support format:\n");
	while (ioctl(V4l2_Context->mCamFd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("\t%d.%s\n",++(fmtdesc.index), fmtdesc.description);
	}
#endif
	printf("Camera is Open!\n");
	return 0;
ErrorEnd:
	if (0 != V4l2_Context->mCamFd)
	{
		close(V4l2_Context->mCamFd);
		V4l2_Context->mCamFd = 0;
	}
	return -1;
}
void CloseCameraDevice(void *v4l2ctx)
{
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	if (v4l2ctx)
	{
		close(V4l2_Context->mCamFd);
		V4l2_Context = NULL;
		printf("succes close the camera!\n");
	}
}

int setV4l2DeviceParam(void* v4l2ctx, int *width, int *height)
{
	int ret = 0;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	struct v4l2_format format;
	memset(&format, 0, sizeof(struct v4l2_format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.height = *height;
	format.fmt.pix.width = *width;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	format.fmt.pix.field = V4L2_FIELD_NONE;

	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_S_FMT, &format);
	if (ret < -1)
	{
		perror("Error in Params Setting!!!\n");
		return ret;
	}
	/************************************************************************/
#ifdef __DEBUG__
	if(ret = ioctl(V4l2_Context->mCamFd, VIDIOC_G_FMT, &format) == -1)
	{
		printf("Unable to get format\n");
		return ret;
	} 
	{
		printf("fmt.type:\t\t%d\n",format.type);
		printf("pix.pixelformat:\t%c%c%c%c\n",format.fmt.pix.pixelformat & 0xFF, (format.fmt.pix.pixelformat >> 8) & 0xFF,(format.fmt.pix.pixelformat >> 16) & 0xFF, (format.fmt.pix.pixelformat >> 24) & 0xFF);
		printf("pix.height:\t\t%d\n",format.fmt.pix.height);
		printf("pix.width:\t\t%d\n",format.fmt.pix.width);
		printf("pix.field:\t\t%d\n",format.fmt.pix.field);
	}
#endif
	/************************************************************************/
#ifdef __DEBUG__
	printf("Set VIDIOC_S_FMT in V4L2_setV4l2DeviceParam!\n");
#endif

	return 0 ;
}
int setV4l2FrameParam(void* v4l2ctx, struct v4l2_streamparm params)
{
	int ret = -1;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_S_PARM, &params);
	if (ret < 0)
	{
		perror("Error in Setting FrameParam!\n");
		return ret ;
	}
#ifdef __DEBUG__
	printf("Set VIDIOC_S_PARM in V4L2setV4l2FrameParam!\n");
#endif
	return 0;
}

int V4l2ReqBuffer(void* v4l2ctx, int *buffernum)
{
	if(*buffernum > MAX_BUFFER_NUM)
	{
		perror("The Buffer_Num is Too Big!\n");
		return -1;
	}
	int ret = -1;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	struct v4l2_requestbuffers RequestBuff;
	memset(&RequestBuff, 0, sizeof(struct v4l2_requestbuffers));

	V4l2_Context->mBufferCnt = *buffernum;
	RequestBuff.count = *buffernum;
	RequestBuff.memory = V4L2_MEMORY_MMAP;
	RequestBuff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef __DEBUG__
	printf("the buff memory is %d!\n", RequestBuff.memory);	
	printf("the buff type is %d!\n", RequestBuff.type);	
	printf("the buff count is %d!\n", RequestBuff.count);
#endif
	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_REQBUFS, &RequestBuff);
	if (ret < 0)
	{
		perror("Error in VIDIOC_REQBUFS\n");
		return ret;
	}
#ifdef __DEBUG__
	printf("Set VIDIOC_REQUBUF in V4L2_V4l2ReqBuffer!\n");
#endif
	if (V4l2_Context->mBufferCnt != (int)RequestBuff.count)
	{
		V4l2_Context->mBufferCnt = (int)RequestBuff.count;
		*buffernum = (int) RequestBuff.count;
		printf("The V4L2_CONTEXT.MBUFFERCNT is %d", V4l2_Context->mBufferCnt);
		perror("\n");
	}
	return 0;
}
int V4l2MapBuffer(void* v4l2ctx)
{
	int ret;
	int count;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	struct v4l2_buffer Buff;
	for (count = 0; count < V4l2_Context->mBufferCnt; count++)
	{
		memset(&Buff, 0, sizeof(struct v4l2_buffer));
		Buff.index = count;
		Buff.field = V4L2_FIELD_NONE;
		Buff.memory = V4L2_MEMORY_MMAP;
		Buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ret = ioctl(V4l2_Context->mCamFd, VIDIOC_QUERYBUF, &Buff);
		if(ret < 0)
		{
			perror("Error in VIDIOC_QUERYBUF----V4L2MAPBUFFER!\n");
			return ret;
		}
#ifdef __DEBUG__
		printf("Set VIDIOC_QUERYBUF in V4L2_V4l2MapBuffer!\n");
#endif
		V4l2_Context->mMapMem.mem[count] = mmap(0, Buff.length, PROT_READ|PROT_WRITE, MAP_SHARED, V4l2_Context->mCamFd, Buff.m.offset);
		if (-1 == V4l2_Context->mMapMem.mem[count])
		{
			perror("Error in memmapping in V4L2_V4l2MapBuffer!\n");
			return -1;
		}
		V4l2_Context->mMapMem.length = Buff.length;
#ifdef __DEBUG__
		printf("Map buffer is ok in V4L2_V4l2MapBuffer!\n");
#endif
		ret = ioctl(V4l2_Context->mCamFd, VIDIOC_QBUF, &Buff);
		if(ret < -1)
		{
			perror("Error is releaseBUF in V4l2MapBuffer!\n");
			return ret;
		}
	}

	return 0;
	
}
int V4l2StopMapBuffer(void* v4l2ctx)
{
	int ret = -1;
	int count;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	for(count = 0; count < V4l2_Context->mBufferCnt; count++)
	{
		ret = munmap(V4l2_Context->mMapMem.mem[count], V4l2_Context->mMapMem.length);
		if (0 != ret)
		{
			perror("Error in Unmapmemory!\n");
			return -1;
		}
	}
	return 0;
}

int GetFrame(void* v4l2ctx, struct v4l2_buffer *buff)
{
	int ret = -1;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	buff->field = V4L2_FIELD_NONE;
	buff->memory = V4L2_MEMORY_MMAP;
	buff->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_DQBUF, buff);
	if (ret < 0)
	{
		perror("Error in GetFrame in GetFrame!\n");
		return -1;
	}
	V4l2_Context->mBufferCnt = buff->index;
	return 0;
	

}
void ReturnFrame(void* v4l2ctx, struct v4l2_buffer *buff)
{
	printf("in RetuenFrame in V4L2.\n");
	int ret = -1;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	//struct v4l2_buffer *buff;
	//buff->field = V4L2_FIELD_INTERLACED;
	//buff->memory = V4L2_MEMORY_MMAP;
	//buff->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	//buff->index = id;
	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_QBUF, buff);
	if (ret < 0)
	{
		perror("Error in ReturnFrame!\n");
		//return -1;
	}

	//return 0;
	
}

int StarStreaming(void* v4l2ctx)
{
	int ret = -1;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_STREAMON, &type);
	if (ret < 0)
	{
		perror("Error in StarSteaming!\n");
		return ret;
	}
	return 0;
}
void StopStream(void* v4l2ctx)
{
	int ret = -1;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	V4L2_CONTEXT* V4l2_Context = (V4L2_CONTEXT*) v4l2ctx;
	ret = ioctl(V4l2_Context->mCamFd, VIDIOC_STREAMOFF, &type);
	if (ret < 0)
	{
		perror("Error in StarSteaming!\n");
	}
}

#ifdef __cplusplus 
}
#endif
