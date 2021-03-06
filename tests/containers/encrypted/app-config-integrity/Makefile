include ../../../common.mk

CC_APP=/usr/bin/python3
CC_ENCLAVE_CONFIG_TEMPLATE=sgxlkl-encrypted-python-config-template.json
CC_ENCLAVE_CONFIG=sgxlkl-encrypted-python-config.json

CC_IMAGE_SIZE=128M

CC_IMAGE_DATA=data-crypt-integrity.img

SGXLKL_ENV_APP_CONFIG=\
SGXLKL_HDS=${CC_IMAGE_DATA}:/data:0

VERBOSE_OPTS=SGXLKL_VERBOSE=1 SGXLKL_KERNEL_VERBOSE=1

CC_IMAGE_INTEGRITY=sgxlkl-alpine-crypt-integrity.img
CC_IMAGE_INTEGRITY_KEY_FILE=$(CC_IMAGE_INTEGRITY).key

ifeq ($(SGXLKL_VERBOSE),)
	SGXLKL_ENV_APP_CONFIG+=${VERBOSE_OPTS}
endif

.DELETE_ON_ERROR:
.PHONY: clean run-hw run-sw

clean:
	rm -f $(CC_IMAGE_INTEGRITY) $(CC_IMAGE_INTEGRITY_KEY_FILE) ${CC_ENCLAVE_CONFIG} ${CC_IMAGE_DATA}

$(CC_IMAGE_INTEGRITY):
	${SGXLKL_DISK_TOOL} create --size=${CC_IMAGE_SIZE} --encrypt --key-file --integrity --alpine="python3" ${CC_IMAGE_INTEGRITY}

${CC_ENCLAVE_CONFIG}: $(CC_IMAGE_INTEGRITY)
	cp ${CC_IMAGE_INTEGRITY} ${CC_IMAGE_DATA}
	cp ${CC_ENCLAVE_CONFIG_TEMPLATE} ${CC_ENCLAVE_CONFIG}
	$(eval CC_IMAGE_INTEGRITY_KEY=$(shell xxd -ps -c100 ${CC_IMAGE_INTEGRITY_KEY_FILE}))

	sed -i "/\"key\":/c\\"\"key\"": \"${CC_IMAGE_INTEGRITY_KEY}\"" ${CC_ENCLAVE_CONFIG}

run-hw: ${CC_ENCLAVE_CONFIG}
	${SGXLKL_ENV_APP_CONFIG} ${SGXLKL_STARTER} --enclave-config=${CC_ENCLAVE_CONFIG} --hw-debug $(CC_IMAGE_INTEGRITY)

run-sw: ${CC_ENCLAVE_CONFIG}
	${SGXLKL_ENV_APP_CONFIG} ${SGXLKL_STARTER} --enclave-config=${CC_ENCLAVE_CONFIG} --sw-debug $(CC_IMAGE_INTEGRITY)
