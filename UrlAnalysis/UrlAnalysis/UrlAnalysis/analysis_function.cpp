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
		cout << "������URL��ַ��" << endl;
		cin >> url_str;

		return url_str;
}

static int check_url(string const& url_str)
{
	if (url_str.empty()) {
		cout << "Э��: δָ��" << endl;
		cout << "����: δָ��" << endl;
		cout << "�˿�: δָ��" << endl;
		cout << "·��: δָ��" << endl;
		cout << "������0��: "  << endl;
		return 0;
	} else
		return 1;
}

static UrlSub* analysis_url(string const& url_str)
{
	UrlSub *url_sub = new UrlSub();
	string::size_type index1, index2, index3;

	// ��ȡЭ��
	index1 = url_str.find(':');
	url_sub->set_protocol(url_str.substr(0, index1));

	// ��ȡ����
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

	// ��ȡ·��
	index2 = url_str.find('?', index3);
	if (string::npos == index2) {
		url_sub->set_path(url_str.substr(index3, url_str.length() - index1));
	} else {
		url_sub->set_path(url_str.substr(index3, index2 - index3));
		// ��ȡ����
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

	cout << "Э��: " << url_sub->get_protocol() << endl;
	cout << "����: " << url_sub->get_host() << endl;

	tmp = url_sub->get_port();
	if (!tmp.empty()) {
		cout << "�˿�: " << tmp << endl;
	} else {
		cout << "�˿�: δָ��" << endl;
	}

	cout << "·��: " << url_sub->get_path() << endl;

	para_count = url_sub->get_parameter_count();
	cout << "������" << para_count << endl;
	if (0 != para_count) {
		for (int index = 0; index != para_count; ++index) {
			cout << "��" << index << "��: ";
			cout << "������: " << url_sub->get_index_parameter(index).first << " ";
			cout << "����ֵ: " << url_sub->get_index_parameter(index).second << endl;
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