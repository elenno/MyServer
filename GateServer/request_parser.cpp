//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_parser.hpp"
#include <boost/regex.hpp>

namespace my {
	namespace http {

		request_parser::request_parser()
			: state_(method_start)
		{
		}

		void request_parser::reset()
		{
			state_ = method_start;
		}

		boost::tribool request_parser::consume(request& req, char input)
		{
			switch (state_)
			{
			case method_start:
				if (!is_char(input) || is_ctl(input) || is_tspecial(input))
				{
					return false;
				}
				else
				{
					state_ = method;
					req.method.push_back(input);
					return boost::indeterminate;
				}
			case method:
				if (input == ' ')
				{
					state_ = uri;
					return boost::indeterminate;
				}
				else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
				{
					return false;
				}
				else
				{
					req.method.push_back(input);
					return boost::indeterminate;
				}
			case uri_start:
				if (is_ctl(input))
				{
					return false;
				}
				else
				{
					state_ = uri;
					req.uri.push_back(input);
					return boost::indeterminate;
				}
			case uri:
				if (input == ' ')
				{
					state_ = http_version_h;
					return boost::indeterminate;
				}
				else if (is_ctl(input))
				{
					return false;
				}
				else
				{
					req.uri.push_back(input);
					return boost::indeterminate;
				}
			case http_version_h:
				if (input == 'H')
				{
					state_ = http_version_t_1;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_t_1:
				if (input == 'T')
				{
					state_ = http_version_t_2;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_t_2:
				if (input == 'T')
				{
					state_ = http_version_p;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_p:
				if (input == 'P')
				{
					state_ = http_version_slash;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_slash:
				if (input == '/')
				{
					req.http_version_major = 0;
					req.http_version_minor = 0;
					state_ = http_version_major_start;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_major_start:
				if (is_digit(input))
				{
					req.http_version_major = req.http_version_major * 10 + input - '0';
					state_ = http_version_major;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_major:
				if (input == '.')
				{
					state_ = http_version_minor_start;
					return boost::indeterminate;
				}
				else if (is_digit(input))
				{
					req.http_version_major = req.http_version_major * 10 + input - '0';
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_minor_start:
				if (is_digit(input))
				{
					req.http_version_minor = req.http_version_minor * 10 + input - '0';
					state_ = http_version_minor;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case http_version_minor:
				if (input == '\r')
				{
					state_ = expecting_newline_1;
					return boost::indeterminate;
				}
				else if (is_digit(input))
				{
					req.http_version_minor = req.http_version_minor * 10 + input - '0';
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case expecting_newline_1:
				if (input == '\n')
				{
					state_ = header_line_start;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case header_line_start:
				if (input == '\r')
				{
					state_ = expecting_newline_3;
					return boost::indeterminate;
				}
				else if (!req.headers.empty() && (input == ' ' || input == '\t'))
				{
					state_ = header_lws;
					return boost::indeterminate;
				}
				else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
				{
					return false;
				}
				else
				{
					req.headers.push_back(header());
					req.headers.back().name.push_back(input);
					state_ = header_name;
					return boost::indeterminate;
				}
			case header_lws:
				if (input == '\r')
				{
					state_ = expecting_newline_2;
					return boost::indeterminate;
				}
				else if (input == ' ' || input == '\t')
				{
					return boost::indeterminate;
				}
				else if (is_ctl(input))
				{
					return false;
				}
				else
				{
					state_ = header_value;
					req.headers.back().value.push_back(input);
					return boost::indeterminate;
				}
			case header_name:
				if (input == ':')
				{
					state_ = space_before_header_value;
					return boost::indeterminate;
				}
				else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
				{
					return false;
				}
				else
				{
					req.headers.back().name.push_back(input);
					return boost::indeterminate;
				}
			case space_before_header_value:
				if (input == ' ')
				{
					state_ = header_value;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case header_value:
				if (input == '\r')
				{
					state_ = expecting_newline_2;
					return boost::indeterminate;
				}
				else if (is_ctl(input))
				{
					return false;
				}
				else
				{
					req.headers.back().value.push_back(input);
					return boost::indeterminate;
				}
			case expecting_newline_2:
				if (input == '\n')
				{
					state_ = header_line_start;
					return boost::indeterminate;
				}
				else
				{
					return false;
				}
			case expecting_newline_3:
				if (req.method == "GET")
				{
					return (input == '\n');
				}
				else
				{
					state_ = http_post_content;
					return boost::indeterminate;
				}
			case http_post_content:
				size_t cont_len;
				req.get_content_length(cont_len);
				if (req.content.length() < cont_len)
				{
					req.content.append(&input);
					if (req.content.length() == cont_len)
						return true;
					return boost::indeterminate;
				}
// 				if (req.content.length() == cont_len)
// 				{
// 					return true;
// 					if (input == '\r')
// 					{
// 						state_ = expecting_content_newline_1;
// 						return boost::indeterminate;
// 					}
// 					else
// 					{
// 						return false;
// 					}
//				}
			case expecting_content_newline_1:
				return (input == '\n');
			default:
				return false;
			}
		}

		bool request_parser::is_char(int c)
		{
			return c >= 0 && c <= 127;
		}

		bool request_parser::is_ctl(int c)
		{
			return (c >= 0 && c <= 31) || (c == 127);
		}

		bool request_parser::is_tspecial(int c)
		{
			switch (c)
			{
			case '(': case ')': case '<': case '>': case '@':
			case ',': case ';': case ':': case '\\': case '"':
			case '/': case '[': case ']': case '?': case '=':
			case '{': case '}': case ' ': case '\t':
				return true;
			default:
				return false;
			}
		}

		bool request_parser::is_digit(int c)
		{
			return c >= '0' && c <= '9';
		}

		inline bool boost_match(const char* pattern,const char* text,unsigned int flags=boost::regex::normal,boost::match_results<const char*>* result=NULL)
		{
			boost::basic_regex<char,boost::regex_traits<char>> expression(pattern,flags); 
			if(NULL==result)
				return boost::regex_match(text,expression);
			return boost::regex_match(text,*result,expression);
		}

		inline bool boost_search(const char* pattern,const char* text,unsigned int flags=boost::regex::normal,boost::match_results<const char*>* result=NULL)
		{
			boost::basic_regex<char,boost::regex_traits<char>> expression(pattern,flags); 
			if(NULL==result)
				return boost::regex_search(text,expression);
			return boost::regex_search(text,*result,expression);
		}

		void split_string(std::vector<std::string> &str_list, string o_str, string split_str)
		{
			int comma_n = 0;
			
			do
			{
				std::string tmp_s = "";
				comma_n = o_str.find(split_str);
				if( -1 == comma_n )
				{
					tmp_s = o_str.substr( 0, o_str.length() );
					str_list.push_back( tmp_s );
					break;
				}
				tmp_s = o_str.substr( 0, comma_n );
				o_str.erase( 0, comma_n+1 );
				str_list.push_back( tmp_s );
			}
			while(true);
		}

		void get_kv(map<string, string>& kv, string kv_str, string kv_sep)
		{
			int pos = kv_str.find(kv_sep);
			if (pos != string::npos)
			{
				kv[kv_str.substr(0, pos)] = kv_str.substr(pos+1, string::npos);
			}			
		}

		int request_parser::parse_content( int type, std::string content, Json::Value& jreq )
		{
			string str_to_parse;
			if (type == GET_METHOD)
			{
				size_t qm_idx = content.find("?");
				if (qm_idx == std::string::npos)
					return -1;
				str_to_parse = content.substr(qm_idx+1);
			}
			else if (type == POST_METHOD)
			{
				str_to_parse = content;
			}

  			map <string, string> kv;
// 			string expr1 = "((\\w+)=(.+(?!&)))";
// 			string expr2 = "(\\w+)=(.+(?!&))";
// 			boost::regex expression(expr1,
// 				boost::regbase::normal | boost::regbase::icase);
// 			boost::sregex_token_iterator begin(str_to_parse.begin(), str_to_parse.end(), expression, 1);
// 			boost::sregex_token_iterator end;
// 			boost::cmatch what;
// 			while(begin != end)
// 			{
// 				string sValue = *begin++;
// 				boost_search(expr2.c_str(), sValue.c_str(), boost::regex::icase|boost::regex::perl, &what);
// 				kv[what[1].str()] = what[2].str();
// 				std::cout << "w1:" << what[1].str()<<endl;
// 				std::cout << "w2:" << what[2].str()<<endl;
// 			}
			vector<std::string> str_list;
			split_string(str_list, str_to_parse, "&");
			for (size_t list_idx = 0; list_idx < str_list.size(); list_idx++)
			{
				get_kv(kv, str_list[list_idx], "=");
			}			

			map<string, string>::iterator it = kv.begin();
			while (it != kv.end())
			{
				jreq[it->first] = it->second;
				it++;
			}

// 			if (jreq["req_type"] == Json::Value::null)
// 			{
// 				return false;
// 			}
// 			else
// 			{
// 				req_type = boost::lexical_cast<int, string>(jreq["req_type"].asString());
// 			}
// 			if (jreq["player_id"] == Json::Value::null)
// 			{
// 				player_id = 0;
// 			}
// 			else
// 			{
// 				player_id = jreq["player_id"].asInt();
// 			}
// 			string s = "";
// 			if (jreq["content"] != Json::Value::null)
// 			{
// 				s = jreq["content"].asString();
// 			}
// 			
// 			scontent = s;
			return 0;
		}

	} // namespace http
} // namespace my
