#include "pt.h"

int main(int32_t argc, char** argv)
{
	if (argc >= 2)
	{
		if (!strcmp(argv[1], "--help"))
		{
			pt_cmd_print_help();
		}
		else
		{
			pt_cmd_run_file(argv[1]);
		}
	}
	else
	{
		pt_cmd_print_help();
	}

	printf("Done!\n");
	return 0;
}