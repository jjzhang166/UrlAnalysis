#include <iostream>
#include <string>
#include <vector>

#include "url_analysis.hpp"
#include "analysis_function.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

string get_url(void)
{
		string url_str;
		cout << "请输入URL地址：" << endl;
		cin >> url_str;

		return url_str;
}

static int check_url(string const& url_str)
{
	if (url_str.empty()) {
		cout << "协议: 未指定" << endl;
		cout << "主机: 未指定" << endl;
		cout << "端口: 未指定" << endl;
		cout << "路径: 未指定" << endl;
		cout << "参数共0项: "  << endl;
		return 0;
	} else
		return 1;
}

static UrlSub* analysis_url(string const& url_str)
{
	UrlSub *url_sub = new UrlSub();
	string::size_type index1, index2, index3;

	// 获取协议
	index1 = url_str.find(':');
	url_sub->set_protocol(url_str.substr(0, index1));

	// 获取主机
	index1 += 3;
	index2 = url_str.find(':', index1);
	index3 = url_str.find('/', index1);
	if (index2 != string::npos) {
		url_sub->set_host(url_str.substr(index1, index2 - index1));
		index2++;
		url_sub->set_port(url_str.substr(index2, index3 - index2));
	} else {
		url_sub->set_host(url_str.substr(index1, index3 - index1));
	}

	// 获取路径
	index2 = url_str.find('?', index3);
	if (string::npos == index2) {
		url_sub->set_path(url_str.substr(index3, url_str.length() - index1));
	} else {
		url_sub->set_path(url_str.substr(index3, index2 - index3));
		// 获取参数
		index2++;
		do
		{
			index1 = url_str.find('&', index2);
			if(string::npos == index1)
				index1 = url_str.length();
			index3 = url_str.find('=', index2);
			string key = url_str.substr(index2, index3 - index2);
			index3++;
			string value = url_str.substr(index3, index1 - index3);
			url_sub->push_parameter(key, value);
			index2=index1 + 1;
		}
		while(url_str.length()!= index1);
	}

	return url_sub;
}

static void print_url_sub(UrlSub* url_sub)
{
	string tmp;
	vector<string>::size_type para_count;

	cout << "协议: " << url_sub->get_protocol() << endl;
	cout << "主机: " << url_sub->get_host() << endl;

	tmp = url_sub->get_port();
	if (!tmp.empty()) {
		cout << "端口: " << tmp << endl;
	} else {
		cout << "端口: 未指定" << endl;
	}

	cout << "路径: " << url_sub->get_path() << endl;

	para_count = url_sub->get_parameter_count();
	cout << "参数共" << para_count << endl;
	if (0 != para_count) {
		for (int index = 0; index != para_count; ++index) {
			cout << "第" << index << "项: ";
			cout << "参数名: " << url_sub->get_index_parameter(index).first << " ";
			cout << "参数值: " << url_sub->get_index_parameter(index).second << endl;
		}
	}
}

static void print_exist_url_analysis(string const& url_str)
{
	UrlSub * url_sub;

	url_sub = analysis_url(url_str);
	print_url_sub(url_sub);
	delete url_sub;
}

void print_url_analysis(string const& url_str)
{
	if (!check_url(url_str)) {
		exit(0);
	}

	print_exist_url_analysis(url_str);
}