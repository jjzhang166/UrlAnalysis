#include <iostream>
#include <string>
#include <vector>

#include "analysis_function.hpp"

using std::string;

int main()
{
	string url_str;

	url_str = get_url();

	print_url_analysis(url_str);

	return 0;
}