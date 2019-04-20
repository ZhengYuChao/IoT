CUR_DIR	 := $(shell /bin/pwd)
SRC_C_DIR	 := $(CUR_DIR)/src
SRC_H_DIR	 := $(CUR_DIR)/include
SRC_L_DIR	 := $(CUR_DIR)/lib

CJSON_DIR := /usr/local
CFLAGS += -I$(CJSON_DIR)/include/json-c

CFLAGS += -fPIC -std=gnu99 -I. 
CFLAGS += -I$(SRC_H_DIR) -I$(SRC_L_DIR) -I/include/linux
# CFLAGS += -Wall -Werror

LDFLAGS = -Wl,--start-group -ldl -lpthread -Wl,--end-group
LDFLAGS += -L$(INSTALL_ROOT)/lib
LDFLAGS += -ljson-c -lm

SVR_C_SRC := $(SRC_C_DIR)/iot_svr.c $(SRC_C_DIR)/iot_common.c \
			 $(SRC_C_DIR)/iot_util.c
SVR_OBJ := $(SVR_C_SRC:%.c=%.o)

CLI_C_SRC := $(SRC_C_DIR)/iot_cli.c $(SRC_C_DIR)/iot_common.c
CLI_OBJ := $(CLI_C_SRC:%.c=%.o)

SVR_TARGET := iot-svr
CLI_TRAGET := iot-cli

.PHONY: all clean
all: $(SVR_TARGET) $(CLI_TRAGET)

%.o: %.c Makefile *.h
	@echo  "CC	$@"
	$(CC) -c $(CFLAGS)  -o $@ $<
	
$(SVR_TARGET): $(SVR_OBJ)
	@echo  "LD	$@ "
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

$(CLI_TRAGET): $(CLI_OBJ)
	@echo  "LD	$@ "
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

clean:
	-rm -rf $(SVR_TARGET) $(SVR_OBJ) $(CLI_TRAGET) $(CLI_OBJ)
	@cd $(SRC_C_DIR)/.. && find . -name "*.gc*" -type f | xargs rm -f
