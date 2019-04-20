#include "usock.h"

#define		DOMAIN_SOCK_PATH_PREX			"/var/tmp/iot_"

/* 
 * fn			bool msg_open(MSG_FD *msg_fd)
 * 
 * brief		use socket to create an file descriptor for msg
 * 
 * param[in]	msg_fd - msg descriptor 
 *
 * return		return false if an error occurs, otherwise true
 *
 * note			call msg_done() when you no longer use this msg 
 */
bool msg_open(MSG_FD *msg_fd)
{
	if (NULL == msg_fd)
	{
		return false;
	}

	msg_fd->fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (msg_fd->fd < 0)
	{
		_ERROR("socket:%m");
		return false;
	}

	if (fcntl(msg_fd->fd, F_SETFD, FD_CLOEXEC))
	{
		_ERROR("fcntl:%m");
		close(msg_fd->fd);
		return false;
	}

	return true;
}

/* 
 * fn			bool msg_bind(MSG_ID msg_id, MSG_FD *msg_fd)
 * 
 * brief		init an endpoint as a server and bind a name to this endpoint msg	
 * 
 * param[in]	msg_id - server id	
 * param[in]	msg_fd - server endpoint msg fd
 *
 * return		return false if an error occurs, otherwise true
 */
static bool msg_bind(MSG_ID msg_id, MSG_FD *msg_fd)
{
	int size = 0;
	if (!MSG_ID_VALID(msg_id) || NULL == msg_fd)
	{
		return false;
	}

	size = sizeof(msg_fd->local_addr.sun_path);
	memset(msg_fd->local_addr.sun_path, 0, size);
	msg_fd->local_addr.sun_family = AF_LOCAL;
	snprintf(msg_fd->local_addr.sun_path, size, "%s%d", DOMAIN_SOCK_PATH_PREX, msg_id);
	unlink(msg_fd->local_addr.sun_path);

	if (bind(msg_fd->fd, (struct sockaddr *)(&msg_fd->local_addr), 
				SUN_LEN(&msg_fd->local_addr)))
	{
		_ERROR("bind:%m");
		return false;
	}

	return true;
}

/* 
 * fn			bool msg_init(MSG_ID msg_id, MSG_FD *msg_fd)
 * brief		create an endpoint for msg and bind it
 *	
 * param[in]	msg_id - msg descriptor that's going to create 
 * param[in]	msg_fd - msg file descriptor
 *
 * return		return false if an error occurs, otherwise true
 *
 * note			call msg_done() when you no longer use this msg 
 */
bool msg_init(MSG_ID msg_id, MSG_FD *msg_fd)
{
	if (false == msg_open(msg_fd) || false == msg_bind(msg_id, msg_fd))
	{
		_ERROR("msg_init fail");
		return false;
	}

	return true;
}

/* 
 * fn			bool msg_done(, MSG_FD *msg_fd)
 * brief		close an file descriptor
 *	
 * param[in]	msg_fd - close msg_fd->fd
 *
 * return		return false if an error occurs, otherwise true
 *
 * note			call msg_done() if you have called msg_init() before;
 *				msg_fd->fd will be set as -1
 */
void msg_done(MSG_FD *msg_fd)
{
	if (NULL == msg_fd || msg_fd->fd < 0)
	{
		return;
	}

	if (close(msg_fd->fd))
	{
		_ERROR("close:%m");
		return;
	}

	msg_fd->fd = -1;
}

/* 
 * fn			bool msg_malloc(MSG_FD *msg_fd, MALLOC_FLAG flag, MSG_BUF *msg_buf)
 * brief		close an file descriptor
 *	
 * param[in]	msg_fd		- if MALLOC_FLAG is MA_RECV, msg_fd should be valid
 * param[in]	MALLOC_FLAG - only MA_SEND/MA_RECV valid
 * param[in]	msg_buf		- buffer to be malloc
 *
 * return		return false if an error occurs, otherwise true
 *
 * note			if success, msg_buf->payload should be NULL, and msg_buf->payload_len 
 *				is the malloc size, otherwise, payload will be NULL;
 */
bool msg_malloc(MSG_FD *msg_fd, MALLOC_FLAG flag, MSG_BUF *msg_buf)
{
	if (NULL == msg_buf || flag <= MA_NONE || flag >= MA_INVALID)
	{
		return false;
	}

	if (MA_RECV == flag && msg_fd && ioctl(msg_fd->fd, FIONREAD, &msg_buf->payload_len)) 
	{
		_ERROR("errmsg:%m");
		return false;
	}

	if (msg_buf->payload_len <= 0)
	{
		_ERROR("len(%d)", msg_buf->payload_len);
		return false;
	}
	
	msg_buf->payload = (unsigned char *)malloc(msg_buf->payload_len + 1);
	if (NULL == msg_buf->payload)
	{
		_ERROR("malloc:%m");
		return false;
	}

	memset(msg_buf->payload, 0, msg_buf->payload_len);
	return true;
}

/* 
 * fn			bool msg_free(MSG_BUF *msg_buf)
 * brief		free msg_buf
 *	
 * param[in]	msg_buf - buffer to be free
 *
 * return		return false if an error occurs, otherwise true
 *				msg_buf->payload_len will be set to 0, and payload will be NULL
 *
 * note			call msg_done() if you have called msg_init() before;
 */
void msg_free(MSG_BUF *msg_buf)
{
	if (NULL == msg_buf || NULL == msg_buf->payload || msg_buf->payload_len <= 0)
	{
		return;
	}

	free(msg_buf->payload);
	msg_buf->payload = NULL;
	msg_buf->payload_len = 0;
}

/* 
 * fn			bool msg_recv(MSG_FD *msg_fd, MSG_BUF *msg_buf)
 * brief		recv from an endpoint
 *	
 * param[in]	msg_fd - msg file descriptor
 * param[in]	msg_buf - msg to be send
 *
 * return		return false if an error occurs, otherwise true
 *
 * note			call msg_malloc() before send, and call free after send
 */
bool msg_recv(MSG_FD *msg_fd, MSG_BUF *msg_buf)
{
	int ret = 0;
	int len = sizeof(msg_fd->remote_addr);
	if (NULL == msg_fd || NULL == msg_buf)
	{
		return false;
	}

	if (NULL == msg_buf->payload || msg_buf->payload_len <= 0)
	{
		_ERROR("payload invalid");
		return false;
	}

	memset(msg_buf->payload, 0, msg_buf->payload_len);
	ret = recvfrom(msg_fd->fd, msg_buf->payload, msg_buf->payload_len, 0, 
				(struct sockaddr *)(&msg_fd->remote_addr), (socklen_t *)&len);
	if (ret != msg_buf->payload_len)
	{
		_ERROR("recvfrom ret:%d, %m", ret);
		return false;
	}

	return true;
}

/* 
 * fn			bool msg_send(MSG_FD *msg_fd, MSG_BUF *msg_buf)
 * brief		send msg to an endpoint
 *	
 * param[in]	msg_fd - msg file descriptor
 * param[in]	msg_buf - msg to be send
 *
 * return		return false if an error occurs, otherwise true
 *
 * note			call msg_malloc() before send, and call free after send
 */
bool msg_send(MSG_FD *msg_fd, MSG_BUF *msg_buf)
{
	int ret = 0;
	if (NULL == msg_fd || NULL == msg_buf)
	{
		return false;
	}

	if (msg_buf->payload_len <= 0 || NULL == msg_buf->payload)
	{
		_ERROR("no payload found");
		return false;
	}

	ret = sendto(msg_fd->fd, msg_buf->payload, msg_buf->payload_len, 0, 
			(struct sockaddr *)(&msg_fd->remote_addr), SUN_LEN(&msg_fd->remote_addr));
	if (ret != msg_buf->payload_len)
	{
		_ERROR("sendto:%m");
		return false;
	}

	return true;
}

/* 
 * fn			bool msg_conn(MSG_ID msg_id, MSG_FD *msg_fd)
 * brief		connect to an endpoint before send msg to it
 *	
 * param[in]	msg_id - remote endpoint id
 * param[in]	msg_fd - msg file descriptor
 *
 * return		return false if an error occurs, otherwise true
 */
bool msg_conn(MSG_ID msg_id, MSG_FD *msg_fd)
{
	int size = 0;
	if (!MSG_ID_VALID(msg_id) || NULL == msg_fd)
	{
		return false;
	}

	size = sizeof(msg_fd->remote_addr.sun_path);
	memset(&msg_fd->remote_addr, 0, sizeof(msg_fd->remote_addr));
	msg_fd->remote_addr.sun_family = AF_LOCAL;
	snprintf(msg_fd->remote_addr.sun_path, size, "%s%d", DOMAIN_SOCK_PATH_PREX, msg_id);
	return true;
}

