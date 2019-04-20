#include "iot_common.h"

#define		IOT_SVR_PID_FILE		"/tmp/iot_svr.pid"

static bool create_pid_file(void)
{
	int fd = -1;
	char pid_str[STR_LEN] = {0};

	if (0 == access(IOT_SVR_PID_FILE, F_OK))
	{
		IOT_ERROR("%s already exists", IOT_SVR_PID_FILE);
		return false;
	}

	fd = open(IOT_SVR_PID_FILE, O_WRONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		return false;
	}

	snprintf(pid_str, sizeof(pid_str), "%u", getpid());
	write(fd, pid_str, strlen(pid_str));
	close(fd);
	return true;
}

static void signal_h(int signo)
{
	IOT_DEBUG("%d receive signal(%d)", getpid(), signo);
	unlink(IOT_SVR_PID_FILE);
	exit(0);
}

int main(int argc, char const *argv[])
{
	int ret = 0;
	fd_set fds;
	struct timeval timeout;
	SVR_CTX ctx;
	memset(&ctx, 0, sizeof(SVR_CTX));

	signal(SIGTERM, signal_h);
	signal(SIGINT, signal_h);

	if (false == create_pid_file())
	{
		IOT_ERROR("create_pid_file fail");
		return 0;
	}

	while (true)
	{
		FD_ZERO(&fds);
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		IOT_DEBUG("sizeof msg is %ld", sizeof(SVR_CTX));
		ret = select(2, &fds, NULL, NULL, &timeout);
		if (ret < 0)
		{
			continue;
		}

		if (0 == ret)
		{
			;
		}

		IOT_DEBUG("ret:%d", ret);
		ret = 1;
	}

	return 0;
}

