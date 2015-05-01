#include "file_system.h"
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include "Glog.h"

using namespace std;
using namespace boost::filesystem;
namespace na
{
	namespace file_system
	{
		void find_files(const path& dir, const string& filename, vector<path>& v)
		{
			static boost::xpressive::sregex_compiler rc;
			if(!rc[filename].regex_id())
			{
				string str = boost::replace_all_copy(
					boost::replace_all_copy(filename, ".", "\\."),"*",".*");
				rc[filename] = rc.compile(str);
			}

			typedef vector<path> result_type;
			if(!exists(dir) || !is_directory(dir)) { return; }

			recursive_directory_iterator  end;
			for (recursive_directory_iterator pos(dir);pos != end; ++pos)
			{
				if(!is_directory(*pos) && boost::xpressive::regex_match( pos->path().filename().string(),rc[filename]))
				{
					v.push_back(pos->path());
				}
			}
		}
		std::string load_jsonfile(const string& file_name)
		{
			string json,line;
			ifstream ifs(file_name.c_str());
			while(getline(ifs,line))
			{
				json.append(line);
			}
			return json;
		}

		Json::Value load_jsonfile_val( const std::string& file_name )
		{
			string json,line;
			ifstream ifs(file_name.c_str());
			while(getline(ifs,line))
			{
				json.append(line);
			}
			Json::Value val;
			Json::Reader reader;
			if(false ==reader.parse(json, val))
			{
				LogE << "failed to parse file " << file_name << " " << reader.getFormatedErrorMessages() << LogEnd;
				LogI << json << LogEnd;
				return Json::Value::null;
			}
			return val;
		}


		int load_jsonfiles_from_dir( const std::string& files_path,json_file_vec& vec )
		{
			LogS << "load json files form:" << files_path << LogEnd;

			vector<path> v;
			path dir;
			dir /= files_path;
			find_files(dir,"*.json",v);
			if(v.size()==0) return 0;
			for (vector<path>::iterator i = v.begin();i!=v.end();++i)
			{
				path& p = *i;
				LogS << "filename:" << p.generic_string() << LogEnd;
				vec.push_back(load_jsonfile(p.generic_string()));
			}
			return 1;
		}

		int load_jsonfiles_from_dir( const std::string& files_path,json_value_vec& vec )
		{
			LogS << "load json files form:" << files_path << LogEnd;

			vector<path> v;
			path dir;
			dir /= files_path;
			find_files(dir,"*.json",v);
			if(v.size()==0) return 0;
			for (vector<path>::iterator i = v.begin();i!=v.end();++i)
			{
				path& p = *i;
				string s = load_jsonfile(p.generic_string());
				Json::Value val;
				Json::Reader reader;
				reader.parse(s, val);
				vec.push_back(val);
			}
			return 1;
		}

		int load_jsonfiles_from_dir( const std::string& files_path,json_value_map& m, string item_id)
		{
			LogS << "load json files form:" << files_path << LogEnd;

			vector<path> v;
			path dir;
			dir /= files_path;
			find_files(dir,"*.json",v);
			if(v.size()==0) return 0;
			for (vector<path>::iterator i = v.begin();i!=v.end();++i)
			{
				path& p = *i;
				string s = load_jsonfile(p.generic_string());
				Json::Value val;
				Json::Reader reader;
				reader.parse(s, val);
				m.insert(make_pair(val[item_id].asInt(),val));
			}
			return 1;
		}

		bool is_flord_exists(const std::string& flordor)
		{
			boost::filesystem::path f(flordor);
			return boost::filesystem::exists(f);
		}

		bool is_flord_or_file_exists(const std::string& flordor_file_dir)
		{
			return boost::filesystem::exists(flordor_file_dir);
		}

		void create_flord( const std::string& flordor_dir)
		{
			boost::filesystem::path f(flordor_dir);
			boost::filesystem::create_directory(f);
		}

		int write_file( const std::string& file_path, const std::string& content_str )
		{
			std::ofstream f(file_path.c_str());
			f << content_str;
			f.flush();
			f.close();
			return 0;
		}

		int rename_file(const std::string& flord_path, const std::string& old_file_name, const std::string& new_file_name)
		{
			string old_file_dir = flord_path + old_file_name;
			string new_file_dir = flord_path + new_file_name;
			if (!is_flord_or_file_exists(old_file_dir))
				return -1;

			boost::filesystem::rename(old_file_dir,new_file_dir);
			return 0;
		}

		int copy_file( const std::string& sorce_file_dir, const std::string& target_file_dir, bool is_over_write /*= false*/ )
		{
			if (is_flord_or_file_exists(target_file_dir) && !is_over_write)
				return -1;
			boost::filesystem::copy_file(sorce_file_dir,target_file_dir,copy_option::overwrite_if_exists);
			return 0;
		}

		int delete_flord(const std::string& flord_path)
		{
			boost::filesystem::remove_all(flord_path);
			return 0;
		}

	}
}
