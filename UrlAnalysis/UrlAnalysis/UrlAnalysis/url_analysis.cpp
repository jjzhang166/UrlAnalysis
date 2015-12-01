// 包含库头文件
#include <iostream>
#include <string>
#include <vector>
#include <utility>

// 包含自己定义的头文件
#include "url_analysis.hpp"

using std::string;
using std::vector;
using std::pair;

string UrlSub::get_protocol(void) const
{
	return protocol;
}

void UrlSub::set_protocol(string const& protocol)
{
	this->protocol = protocol;
}

string UrlSub::get_host(void) const
{
	return host;
}

void UrlSub::set_host(string const& host)
{
	this->host = host;
}

string UrlSub::get_port(void) const
{
	return port;
}

void UrlSub::set_port(string const& port)
{
	this->port = port;
}

string UrlSub::get_path(void) const
{
	return path;
}

void UrlSub::set_path(string const& path)
{
	this->path = path;
}

vector< pair<string, string> >::size_type UrlSub::get_parameter_count(void) const
{
	return parameter.size();
}

std::pair<std::string, std::string> UrlSub::get_index_parameter(vector<string>::size_type index) const
{
	return parameter[index];
}

void UrlSub::push_parameter(string const& key, string const& value)
{
	parameter.push_back(make_pair(key, value));
}