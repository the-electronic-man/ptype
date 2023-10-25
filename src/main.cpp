#include "pt.h"

int main(int32_t argc, char** argv)
{
	switch (argc)
	{
		case 2:
		{
			if (!strcmp(argv[1], "--help"))
			{
				pt_cmd_print_help();
			}
			else
			{
				// run file
			}
			break;
		}
		default:
		{
			pt_cmd_print_help();
			break;
		}
	}

	return 0;
}