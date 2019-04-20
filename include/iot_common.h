#ifndef _IOT_COMMON_H_
#define _IOT_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <math.h>
#include <ctype.h>

#include "json.h"
#include "usock.h"

#define		IOT_DEBUG_ENABLE		1

#if IOT_DEBUG_ENABLE
#define IOT_DEBUG(fmt, args...) \
	printf("[DEBUG](%s) line:%d: "fmt"\r\n", __FUNCTION__, __LINE__, ##args)

#define IOT_ERROR(fmt, args...)  do \
	{																				\
		printf("[ERROR](%s) line:%d: "fmt"\r\n", __FUNCTION__, __LINE__, ##args);	\
		if (errno)																	\
			printf("%m\n");															\
	}while(0);
#else
#define IOT_DEBUG(fmt, args...)
#define IOT_ERROR(fmt, args...)
#endif

#define			ARRAY_SIZE(a)					((sizeof(a)) / (sizeof(a[0])))
#define			SET_BIT_1(value, offset)		((value) |= (1 << offset))
#define			CHECK_BIT_1(value, offset)		((0x01) & ((value) >> offset))

#define			MODULE_NUM_MAX					32
#define			RECORD_NUM_MAX					32
#define			STR_LEN							32

/* #if defined(__KERNEL__) || defined(_LVM_H_INCLUDE) */
struct list_head {
	struct list_head *next, *prev;
};
/* #endif */

/* module id */
typedef enum _M_ID_ {
	id_min,
	id_kasa,
	id_lifx,
	id_wemo,
	id_ecobee,
	id_vesync,
	id_nest,
	id_hue,
	id_ble,
	id_zb,
	id_yale,
	id_max
} M_ID;

typedef enum _OPR_ {
	opr_min,
	opr_get,
	opr_get_all,
	opr_get_profile,
	opr_add,
	opr_remove,
	opr_set,
	opr_start_scan,
	opr_stop_scan,
	opr_get_scan,
	opr_trig_reg,
	opr_trig_unreg,
	opr_trig_notice,
	opr_mod_reg,
	opr_mod_unreg,
	opr_max
} OPR;

typedef enum _CATG_BIT_ {
	SWITCHER,
	LIGHT,
	SENSOR,
	THERMESTAT,
	TAG,
} CATG_BIT;

typedef struct _MSG_ {
	M_ID module_id;
	OPR opr;
	unsigned int category;
	unsigned int seq;
	unsigned int sub_seq;
	unsigned char payload[0];
} MSG;

typedef struct _MOD_ENTRY_ {
	M_ID module_id;
	unsigned int category;
} MOD_ENTRY;

typedef struct _MSG_REC_ {
	M_ID module_id;
	unsigned int seq;
	unsigned int sub_seq;
} MSG_REC;

typedef struct _SVR_CTX_ {
} SVR_CTX;

#define		MOD_IN_INVALID(id)	((id <= id_min) || (id >= id_max))
#define		OPR_INVALID(opr)	((opr <= opr_min) || (opr >= opr_max))

/*
	{
		"opr":number,
		"id": string,
		"module": string,
		"category": num,
	
*/

#define			IOT_OPR			"opr"
#define			IOT_DEV_ID		"id"
#define			IOT_MODULE		"module"
#define			IOT_CATG		"category"

#endif

