#include "iot_common.h"

#define			HELP_ARG		"--help"

static void iot_usage(void)
{
	printf("./iot_cli [h|r|p]\n");
	printf("\t-h:show this message\n");
	printf("\t-m:show module list\n");
	printf("\t-p:show protocol\n");
	printf("\t-r:option\n");
}

static void iot_parse_request(json_object *request)
{
	json_object *opr = NULL;
	json_object *mod = NULL;
	json_object *id = NULL;
	json_object *catg = NULL;
	if (NULL == request) {
		IOT_ERROR("request is not in json format");
		return;
	}

	opr = json_object_object_get(request, IOT_OPR);
	id = json_object_object_get(request, IOT_DEV_ID);
	mod = json_object_object_get(request, IOT_MODULE);
	catg = json_object_object_get(request, IOT_CATG);

	if (NULL == opr || NULL == id || NULL == mod || NULL == catg) {
		IOT_ERROR("parameter missed!");
		return;
	}

	IOT_DEBUG("module is %s", json_object_get_string(mod));
	IOT_DEBUG("id is %s", json_object_get_string(id));
}

static void iot_handle_opt(int opt, char *argv[])
{
	json_object *request = NULL;
	switch (opt) {
		case 'h':
			iot_usage();
			break;

		case 'm':
			break;

		case 'p':
			break;

		case 'r':
			if (NULL == argv || NULL == argv[2]) {
				IOT_ERROR("file required");
				return;
			}

			request = json_object_from_file(argv[2]);
			iot_parse_request(request);
			json_object_put(request);
			break;

		default:
			IOT_ERROR("Undefined option(%c)", opt);
			iot_usage();
	}
}

int main(int argc, char *argv[])
{
	int opt = -1;
	if (argc < 2 || 0 == strncmp(argv[1], HELP_ARG, strlen(HELP_ARG)))
	{
		iot_usage();
		return 0;
	}

	while ((opt = getopt(argc, argv, "hmrp")) != -1) {
		iot_handle_opt(opt, argv);
	}

    return 0;
}

