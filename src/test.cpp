#define BOOST_NETWORK_ENABLE_HTTPS
#include <iostream>
#include <boost/network/protocol/http/client.hpp>

int main()
{
	using namespace std;
	using namespace boost::network;
	http::client client;
	http::client::request request("https://doodle-manga-scraper.p.mashape.com/mangareader.net/manga/hidamari-ga-kikoeru/1");
	request << header("X-Mashape-Key", "av7yog4uPAmshcKLnifDJRqXBuFFp1pGpcSjsnt3V0QxkJTaHt");
	http::client::response response = client.get(request);
	cout << body(response) << endl;
}
