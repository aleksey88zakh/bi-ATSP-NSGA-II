#include "common.h"

#include <string>

using namespace std;
using namespace System;
using namespace System::IO;


void time_format(unsigned long long result_time, String^ title, StreamWriter^ sw)
{
	int res_sec = (result_time / 1000) % 60;
	int res_min = (result_time / (60 * 1000)) % 60;
	int res_hour = result_time / (3600 * 1000);
	sw->WriteLine(title + ": ; {0}; h; {1}; min; {2}; sec", res_hour, res_min, res_sec);
	printf("Time:\n %d:%d:%d\n", res_hour, res_min, res_sec);
}
