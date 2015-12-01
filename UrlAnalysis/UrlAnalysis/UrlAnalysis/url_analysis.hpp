#ifndef URL_ANALYSIS_HPP
#define URL_ANALYSIS_HPP

#include <string>
#include <vector>
#include <utility>

class UrlSub
{
public:
	// 目前指定义了一个构造函数，就是默认构造函数
	UrlSub(void) {};

	std::string get_protocol(void) const;
	void set_protocol(std::string const&);

	std::string get_host(void) const;
	void set_host(std::string const&);

	std::string get_port(void) const;
	void set_port(std::string const&);

	std::string get_path(void) const;
	void set_path(std::string const&);

	std::vector<std::string>::size_type get_parameter_count(void) const;
	std::pair<std::string, std::string> get_index_parameter(std::vector<std::string>::size_type index) const;
	void push_parameter(std::string const&, std::string const&);
private:
	std::string protocol;
	std::string host;
	std::string port;
	std::string path;

	// 参数可能有多个因此定义vector变量
	std::vector< std::pair<std::string, std::string> > parameter;
};

#endif