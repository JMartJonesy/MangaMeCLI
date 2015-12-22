#define BOOST_NETWORK_ENABLE_HTTPS
// STD INCLUDES
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

// MY INCLUDES
#include "MS-Wrapper.hpp"

// CPP-NETLIB INCLUDES
#include <boost/network/uri.hpp>
#include <boost/network/protocol/http/client.hpp>

// BOOST FILESYSTEM INCLUDES
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

// BOOST JSON INCLUDES
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// NAMESPACES
using namespace std;
namespace endPoints = mangaScraper::getEndpoints;
namespace propTree = boost::property_tree;
namespace http = boost::network::http;
namespace uri = boost::network::uri;
namespace fs = boost::filesystem;

// TYPEDEFS
typedef map<string, string> stringmap;

// GLOBALS
auto mashapeHeader = boost::network::header("X-Mashape-Key", "av7yog4uPAmshcKLnifDJRqXBuFFp1pGpcSjsnt3V0QxkJTaHt");
fs::path baseDownloadPath(fs::current_path());
string siteId = "mangareader.net"; // change later
stringmap mangaList;

// FUNCTIONS
void printDownloadStatus(int curr, int outOf)
{
	double percent = (curr / (double)outOf) * 100;
	int pounds = (curr / (double)outOf) * 50; // 50 pounds
	int spaces = 50 - pounds;

	cout << "\r" << flush;
	for(int i = 0; i < pounds; ++i)
		cout << "#" << flush;
	for(int i = 0; i < spaces; ++i)
		cout << " " << flush;
	cout << " " << setprecision(2) << fixed << percent << "%" << flush;

	if(curr == outOf)
		cout << endl;	
}

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

void downloadChapter(string manga, int chapter)
{
	fs::path mangaPath = baseDownloadPath;
	mangaPath /= manga;
	propTree::ptree pt = getJsonTree(endPoints::getChapter(siteId, manga, to_string(chapter)));
	mangaPath /= to_string(chapter) + "-" + pt.get<string>("name");
	cout << "Downloading chapter " << chapter << "-" << pt.get<string>("name") << endl;
	if(!exists(mangaPath) || !is_directory(mangaPath))
	{
		int pageCount = 0;
		fs::create_directory(mangaPath);
		BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child("pages"))
			for(auto it = v.second.begin(); it != v.second.end(); ++it)		
				if(it->first.compare("url") == 0)
					++pageCount;
		BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child("pages"))
		{
			string page = "";
			for(auto it = v.second.begin(); it != v.second.end(); ++it)		
			{
				if(it->first.compare("url") == 0)
				{
					string filename = mangaPath.string() + "/" + page + ".jpg";
					ofstream ofs(filename.c_str());
					ofs << static_cast<string>(getResponseBody(it->second.get_value<string>())) << endl;
					ofs.close();
					printDownloadStatus(stoi(page), pageCount);
				}
				else
				{
					page = it->second.get_value<string>();
				}
			}
		}
	}
	else
	{
		cout << "Chapter " << chapter << "already downloaded" << endl;
	}
}

void downloadChapters(string manga, int sChapter, int eChapter)
{
	bool good = true;
	try
	{
		fs::path mangaPath = baseDownloadPath;
		mangaPath /= manga;
		if(!exists(mangaPath) || !is_directory(mangaPath))
		{
			cout << "Creating folder: " << mangaPath << endl;
			if(fs::create_directory(mangaPath))
			{
				cout << "Directory " << mangaPath << " created successfully" << endl;
			}
			else
			{
				cout << "Error creating " << mangaPath << endl;
				good = false;
			}
		}
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
		good = false;
	}
	if( good )
		for(int i = sChapter; i <= eChapter; ++i)
			downloadChapter(manga, i);
}

void downloadManga(string manga)
{
	int chapterCount = 0;
	propTree::ptree pt = getJsonTree(endPoints::getManga(siteId, manga));
	BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child("chapters"))
	{
		for(auto it = v.second.begin(); it != v.second.end(); ++it)
		{
			if(it->first.compare("chapterId") != 0)
				++chapterCount;
		}
	}

	downloadChapters(manga, 1, chapterCount);
}

void searchManga(string query)
{
	bool results = false;
	propTree::ptree pt = getJsonTree(endPoints::Search(siteId, query));
	BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child(""))
	{
		for(auto it = v.second.begin(); it != v.second.end(); ++it)
		{
			results = true;
			if(it->first.compare("name") == 0)
			{
				cout << it->second.get_value<string>() << endl;
			}
		}
	}

	if( !results )
	{
		cout << "No results found" << endl;
	}
}

void listManga()
{
	for(auto it = mangaList.begin(); it != mangaList.end(); ++it)
		cout << it->first << endl;
}

void setupMangaList()
{
	propTree::ptree pt = getJsonTree(endPoints::getMangaList(siteId));
	BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child(""))
	{
		string key, value;
		for(auto it = v.second.begin(); it != v.second.end(); ++it)
		{
			if(it->first.compare("mangaId") == 0)
			{
				value = it->second.get_value<string>();
			}
			else
			{
				key = it->second.get_value<string>();
				mangaList.emplace(key, value);
			}
		}
	}
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
			cout << "Downloads folder: " << baseDownloadPath << endl;
		}
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}

	try 
	{
		setupMangaList();
		int input = -1;
		string choice = "";
		while( input != 0 )
		{
			cout << "1. List Manga" << endl;
			cout << "2. Search Manga" << endl;
			cout << "3. Download Manga" << endl;
			cout << "0. Exit MangaMe" << endl;
			cout << "Input: ";
			cin >> input;
			switch(input)
			{
				case 1:
					listManga();
					break;
				case 2:
					cout << "Input search query: ";
					cin >> choice;
					searchManga(choice);
					break;	
			}
			/*
			string choice = searchManga("naruto");
			if(choice.compare("None") != 0)
				cout << choice << endl;
			*/
		}
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}

    return 0;
}
