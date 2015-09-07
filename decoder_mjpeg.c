#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "jerror.h"
#include "huffman.h"

#include "decoder_mjpeg.h"
#include "decoder.h"

#define __DEBUG__

static int is_huffman(unsigned char *buf)
{
	int i = 0;
	unsigned  char *pbuf = buf;
	while (((pbuf[0]<< 8)| pbuf[1]) != 0xffda)
	{
		if (i++ > 2048)
		{
			return 0;
		}

		if (((pbuf[0]<<8) | pbuf[1]) == 0xffc4)
		{
			return 1;
		}

		pbuf++;
	}

	return 0;
}
/*
typedef struct jpeg_error_mgr * my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	(*cinfo->err->output_message) (cinfo);

	longjmp(myerr->setjmp_buffer,1);
}
*/
static void decoder_jpeg_decompress(unsigned char *out_buf,
	unsigned char * jpeg_buf,
	int jpeg_size)
{
	int row_stride;
	unsigned char *output_index;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/*step1:*/
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/*step2*/
	jpeg_mem_src(&cinfo, jpeg_buf, jpeg_size);
#ifdef __DEBUG__
	printf("begin read head!\n");
#endif
	/*step3*/
	(void)jpeg_read_header(&cinfo, TRUE);
#ifdef __DEBUG__
	printf("after read head!\n");
#endif
	/*step4*/
	//cinfo.out_color_space = JCS_YCbCr;

	/*step5*/
	(void)jpeg_start_decompress(&cinfo);

	/*step6*/
	row_stride = cinfo.output_width * cinfo.output_components;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		output_index = out_buf + cinfo.output_scanline * row_stride;

		(void) jpeg_read_scanlines(&cinfo, &output_index, 1);
	}

	/*step7*/
	(void) jpeg_finish_decompress(&cinfo);

	/*step8*/
	jpeg_destroy_decompress(&cinfo);
#ifdef __DEBUG__
	printf("decompress finish!!\n");
#endif
}

void decoder_mjpeg_decode(Decoder *thiz,
		unsigned char *out_buf,
		unsigned char *in_buf,
		int buf_size)
{
	if (NULL == out_buf)
	{
		printf("out_buf is null!\n");
		exit(0);
	}
	if (NULL == in_buf)
	{
		printf("in_buf is null!\n");
		exit(0);
	}
#ifdef __DEBUG__
	printf("before is_huffman in decoder_mjpeg_decode!\n");
#endif
	int pos = 0;
	int size_start = 0;
	unsigned char *pdeb = in_buf;
	unsigned char *pcur = in_buf;
	unsigned char *plimit = in_buf + buf_size;
	unsigned char *jpeg_buf;

	if (is_huffman(in_buf))
	{
#ifdef __DEBUG__
		printf("huffman!\n");
#endif
		decoder_jpeg_decompress(out_buf, in_buf, buf_size);
	}
	else
	{
#ifdef __DEBUG__
		printf("no huffman!\n");
#endif
		/*find the SOFO*/
		while ( (((pcur[0]<<8) | pcur[1]) != 0xffc0) && (pcur < plimit))
		{
			pcur++;
		}

		/*SOFO exist*/
		if (pcur < plimit)
		{
#ifdef __DEBUG__
			printf("SOFO existde at the position!\n");
#endif
			jpeg_buf = malloc(buf_size + sizeof(dht_data) + 10);
			
			if (jpeg_buf != NULL)
			{
				size_start = pcur - pdeb;
				memcpy(jpeg_buf, in_buf, size_start);
				pos += size_start;

				memcpy(jpeg_buf + pos, dht_data, sizeof(dht_data));
				pos += sizeof(dht_data);

                memcpy(jpeg_buf + pos, pcur, buf_size - size_start);
                pos += buf_size - size_start;

                decoder_jpeg_decompress(out_buf, jpeg_buf, pos);

				free(jpeg_buf);
				jpeg_buf = NULL;
			}
		}
	}
}


static void decoder_mjpeg_destory(Decoder *thiz)
{
	if (NULL != thiz)
	{
		free(thiz);
	}
}

Decoder *decoder_mjpeg_create()
{
	Decoder *thiz = (Decoder*)malloc(sizeof(Decoder));
	if (NULL != thiz)
	{
		thiz->decode = decoder_mjpeg_decode;
		thiz->destroy = decoder_mjpeg_destory;
	}

	return thiz;
}
