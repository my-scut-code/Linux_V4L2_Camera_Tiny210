#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

struct _Decoder;
typedef struct _Decoder Decoder;

struct _Decoder 
{
	void (*decode)(Decoder *thiz,
		unsigned char *out_buf,
		unsigned char *in_buf,
		int buf_size);
	void (*destroy)(Decoder *thiz);

	char priv[];
};

static inline void decoder_decode(Decoder *thiz,
	unsigned char *out_buf,
	unsigned char *in_buf,
	int buf_size)
{
	assert(thiz != NULL && thiz->decode != NULL)
	{
		thiz->decode(thiz, out_buf, in_buf, buf_size);
	}
}

static inline void decoder_destory(Decoder *thiz,
	unsigned char *out_buf,
	unsigned char *in_buf,
	int buf_size)
{
	assert(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}
}



#ifdef 
}
#endif