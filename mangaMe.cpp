#define BOOST_NETWORK_ENABLE_HTTPS
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "MS-Wrapper.hpp"

//CPP-NETLIB INCLUDES
#include <boost/network/uri.hpp>
#include <boost/network/protocol/http/client.hpp>

//BOOST FILESYSTEM INCLUDES
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

//BOOST JSON INCLUDES
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

namespace endPoints = mangaScraper::getEndpoints;
namespace propTree = boost::property_tree;
namespace http = boost::network::http;
namespace uri = boost::network::uri;
namespace fs = boost::filesystem;
namespace {
	string get_filename(const uri::uri &url) 
	{
		string path = uri::path(url);
		size_t index = path.find_last_of('/');
		string filename = path.substr(index + 1);
		return filename.empty()? "index.html" : filename;
	}
}

auto mashapeHeader = boost::network::header("X-Mashape-Key", "av7yog4uPAmshcKLnifDJRqXBuFFp1pGpcSjsnt3V0QxkJTaHt");
fs::path baseDownloadPath(fs::current_path());
string siteId = "mangareader.net"; // change later

// Makes get request using uri param and returns
// the body of the reponse
string getResponseBody(string uri)
{
	http::client client;
	http::client::request request(uri);
	request << mashapeHeader;
	return body(client.get(request));
}

propTree::ptree getJsonTree(string uri)
{
	stringstream ss;
	ss << getResponseBody(uri);
	propTree::ptree pt;
	propTree::read_json(ss, pt);
	return pt;
}

void downloadManga(string site, string manga)
{
	http::client client;
	http::client::request request(endPoints::getChapter("mangareader.net", "naruto", "1"));
	request << mashapeHeader;
	http::client::response response = client.get(request);
}

void downloadChapters(string site, string manga, int sChapter, int eChapter)
{
	
}

int main(int argc, char *argv[]) {
	// Checks for download folder and creates it if
	// doesn't exist
	try
	{
		baseDownloadPath /= "manga";
		if(!exists(baseDownloadPath) || !is_directory(baseDownloadPath))
		{
			cout << "Creating download folder" << endl;
			if(fs::create_directory(baseDownloadPath))
			{
				cout << "Download directory " << baseDownloadPath << " created successfully" << endl;
			}
			else
			{
				cout << "Error creating " << baseDownloadPath << endl;
				return 1;
			}
		}
		else
		{
			cout << "Download folder: " << baseDownloadPath << endl;
		}
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}

	try 
	{
		//Setting up and requesting json
		http::client client;
		http::client::request request(endPoints::getChapter("mangareader.net", "naruto", "1"));
		request << mashapeHeader;
		http::client::response response = client.get(request);

		//Parsing JSON
		string resBody = body(response);
		cout << resBody << endl;
		stringstream ss;
		ss << resBody;
		propTree::ptree pt;
		propTree::read_json(ss, pt);
		BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child("pages"))
		{
			for(auto it = v.second.begin(); it != v.second.end(); ++it)
				cout << it->first << " : " << it->second.get_value<string>() << endl;
		}

		//Downloading File
	       // string filename = get_filename(request.uri());
	       // cout << "Saving to: " << filename << endl;
	       // ofstream ofs(filename.c_str());
	       // ofs << static_cast<string>(body(response)) << endl;
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}

    return 0;
}
