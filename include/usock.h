#ifndef _IOT_SOCK_H_
#define _IOT_SOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

typedef enum _MSG_ID_ {
	MSG_ID_MIN = 1,
	MSG_ID_IOT_CENTER,
	MSG_ID_HUE,
	MSG_ID_BLE,
	MSG_ID_TPRA,
	MSG_ID_ZB,
	MSG_ID_AUTO,
	MSG_ID_NEST,
	MSG_ID_NETDEV,
	MSG_ID_LIFX,
	MSG_ID_WEMO,
	MSG_ID_ECOBEE,
	MSG_ID_VESYNC,
	MSG_ID_YALE,
	MSG_ID_SVR,
	MSG_ID_CLI,
	MSG_ID_ALL_IOT,
	MSG_ID_MAX
} MSG_ID;

typedef struct _MSG_FD_ 
{
	int fd;
	struct sockaddr_un local_addr;
	struct sockaddr_un remote_addr;
} MSG_FD;

typedef struct _MSG_BUF_ {
	MSG_ID id;
	int payload_len;
	unsigned char *payload;
} MSG_BUF;

typedef enum _MALLOC_FLAG_ {
	MA_NONE,
	MA_SEND,
	MA_RECV,
	MA_INVALID
} MALLOC_FLAG;

bool msg_open(MSG_FD *msg_fd);

bool msg_init(MSG_ID msg_id, MSG_FD *msg_fd);
void msg_done(MSG_FD *msg_fd);

bool msg_malloc(MSG_FD *msg_fd, MALLOC_FLAG flag, MSG_BUF *msg_buf);
void msg_free(MSG_BUF *msg_buf);

bool msg_conn(MSG_ID msg_id, MSG_FD *msg_fd);
bool msg_send(MSG_FD *msg_fd, MSG_BUF *msg_buf);
bool msg_recv(MSG_FD *msg_fd, MSG_BUF *msg_buf);

#define		MSG_ID_VALID(id)				((id > MSG_ID_MIN) && (id < MSG_ID_MAX))
#define		SOCK_MAX_WAIT					60

#if 0
#define _DEBUG(fmt, args...) \
	printf("[DEBUG](%s) line:%d: "fmt"\r\n", __FUNCTION__, __LINE__, ##args)
#else
#define _DEBUG(fmt, args...)
#endif

#define _ERROR(fmt, args...)  \
	printf("[IOTSOCK_ERROR](%s) line:%d: "fmt"\r\n", __FUNCTION__, __LINE__, ##args)

#endif
