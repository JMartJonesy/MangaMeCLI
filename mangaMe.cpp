#define BOOST_NETWORK_ENABLE_HTTPS
// STD INCLUDES
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

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
stringmap printList;

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

// Returns responsebody as property tree
propTree::ptree getJsonTree(string uri)
{
	stringstream ss;
	ss << getResponseBody(uri);
	propTree::ptree pt;
	propTree::read_json(ss, pt);
	return pt;
}

// TODO: Maybe short circuit downloadChapters if chapter isnt found
// Downloads a single chapter
void downloadChapter(string manga, string mangaId, int chapter)
{
	propTree::ptree pt = getJsonTree(endPoints::getChapter(siteId, mangaId, to_string(chapter)));
	cout << endPoints::getChapter(siteId, mangaId, to_string(chapter)) << endl;
	if(pt.count("name") != 0)
	{
		fs::path mangaPath = baseDownloadPath;
		mangaPath /= manga;
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
			cout << "Chapter already downloaded" << endl;
		}
	}
}

// Downloads chapters in range from sChaptert to eChapter
void downloadChapters(string manga, string mangaId, int sChapter, int eChapter)
{
	bool good = true;
	try
	{
		fs::path mangaPath = baseDownloadPath;
		mangaPath /= printList[manga];
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
			downloadChapter(manga, mangaId, i);
}

// Downloads all chapters of given manga
void downloadManga(string manga, string mangaId)
{
	int chapterCount = 0;
	propTree::ptree pt = getJsonTree(endPoints::getManga(siteId, mangaId));
	BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child("chapters"))
	{
		for(auto it = v.second.begin(); it != v.second.end(); ++it)
		{
			if(it->first.compare("chapterId") != 0)
				++chapterCount;
		}
	}

	cout << chapterCount << endl;
	downloadChapters(manga, mangaId, 1, chapterCount);
}

// Prints out search results for query
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

// Prints all manga found on MangaReader.net
void listManga()
{
	for(auto it = printList.begin(); it != printList.end(); ++it)
		cout << it->first << endl;
}

// Creates local list of manga from MangaReader.net
void setupMangaList()
{
	propTree::ptree pt = getJsonTree(endPoints::getMangaList(siteId));
	BOOST_FOREACH(propTree::ptree::value_type &v, pt.get_child(""))
	{
		string key, value, upperK;
		for(auto it = v.second.begin(); it != v.second.end(); ++it)
		{
			if(it->first.compare("mangaId") == 0)
			{
				value = it->second.get_value<string>();
			}
			else
			{
				key = it->second.get_value<string>();
				upperK = key;
				transform(key.begin(), key.end(), key.begin(), ::tolower);
				printList.emplace(key, upperK);
				mangaList.emplace(key, value);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	// Checks for download folder and creates it if doesn't exist
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
		// Setup and Main Menu
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
					cin.ignore(1000, '\n');
					getline(cin, choice);
					searchManga(choice);
					break;	
				case 3:
					int sInput = 0;
					cout << "Input manga: ";
					cin.ignore(1000, '\n');
					getline(cin, choice);
					transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
					if(mangaList.find(choice) == mangaList.end())
					{
						cout << choice << " not found" << endl;
						break;
					}
					string mangaId = mangaList[choice];
					int sChapter = 0;
					int eChapter = 0;
					cout << "Download" << endl;
					cout << "1. Entire Manga" << endl;
					cout << "2. Single Chapter" << endl;
					cout << "3. Multiple Chapters" << endl;
					cout << "0. Cancel" << endl;
					cout << "Input: ";
					cin >> sInput;
					switch(sInput)
					{
						case 1:
							downloadManga(choice, mangaId);
							break;
						case 2:
							cout << "Input chapter: ";
							cin >> sChapter;
							downloadChapters(choice, mangaId, sChapter, sChapter);
							break;
						case 3:
							cout << "Input start chapter: ";
							cin >> sChapter;
							cout << "Input end chapter: ";
							cin >> eChapter;
							downloadChapters(choice, mangaId, sChapter, eChapter);
					}
					
			}
		}
	}
	catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}

    return 0;
}
