include ../../common.mk

APP_ROOT=app
PROG=${APP_ROOT}/python-helloworld.py

DISK_IMAGE=sgxlkl-python.img
IMAGE_SIZE=1024M

ENCLAVE_CMD=/usr/bin/python3 ${PROG}

ifeq ($(SGXLKL_VERBOSE),)
SGXLKL_ENV=\
   SGXLKL_VERBOSE=1 SGXLKL_KERNEL_VERBOSE=1 SGXLKL_TRACE_SIGNAL=0\
   SGXLKL_TRACE_HOST_SYSCALL=0 SGXLKL_TRACE_LKL_SYSCALL=0 SGXLKL_TRACE_MMAP=0
else
SGXLKL_ENV=
endif

.DELETE_ON_ERROR:
.PHONY: run run-hw run-sw clean

$(DISK_IMAGE): $(PROG)
	${SGXLKL_DISK_TOOL} create --size=${IMAGE_SIZE} --docker=./Dockerfile --copy=./${APP_ROOT}/ ${DISK_IMAGE}

run: run-hw run-sw

run-hw: $(DISK_IMAGE)
	${SGXLKL_ENV} ${SGXLKL_STARTER} --hw-debug $(DISK_IMAGE) $(ENCLAVE_CMD)

run-hw-gdb: $(DISK_IMAGE)
	${SGXLKL_ENV} ${SGXLKL_GDB} --args ${SGXLKL_STARTER} --hw-debug $(DISK_IMAGE) $(ENCLAVE_CMD)

run-sw: $(DISK_IMAGE)
	${SGXLKL_ENV} ${SGXLKL_STARTER} --sw-debug $(DISK_IMAGE) $(ENCLAVE_CMD)

run-sw-gdb: $(DISK_IMAGE)
	${SGXLKL_ENV} ${SGXLKL_GDB} --args ${SGXLKL_STARTER} --sw-debug $(DISK_IMAGE) $(ENCLAVE_CMD)

clean:
	rm -f $(DISK_IMAGE)
