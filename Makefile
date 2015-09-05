CC = /home/work/workshop/licheelinux/out/linux/common/buildroot/external-toolchain/bin/arm-linux-gnueabi-gcc
OBJS = main.o \
	   camera.o \
	   decoder_mjpeg.o \
	   V4L2.o

COURCE = main.c \
	   camera.c \
	   decoder_mjpeg.c \
	   V4L2.c 

target = CAMERA
all:	$(target)

$(target):$(OBJS)
	$(CC) -lm -lpthread -L$(PWD) -ljpeg -o $(target) $(OBJS) 
install:
	sudo chmod 777 * -R
.PHONY:clean
clean:
	rm $(OBJS) $(target)