namespace mangaScraper{ namespace getEndpoints{
	std::string baseURI = "https://doodle-manga-scraper.p.mashape.com/";
	std::string getChapter(std::string siteid, std::string mangaid, std::string chapterid)
	{
		std::stringstream ss;
		ss << baseURI << siteid << "/manga/" << mangaid << "/" << chapterid;
		return ss.str();
	}

	std::string getGenreList(std::string siteid)
	{
		std::stringstream ss;
		ss << baseURI << siteid << "/search/genres";
		return ss.str();
	}

	std::string getManga(std::string siteid, std::string mangaid)
	{
		std::stringstream ss;
		ss << baseURI << siteid << "/manga/" << mangaid << "/";
		return ss.str();
	}

	std::string getMangaByGenre(std::string siteid, std::string genreid)
	{
		std::stringstream ss;
		ss << baseURI << siteid << "/search/genres/" << genreid;
		return ss.str();
	}

	std::string getMangaList(std::string siteid)
	{
		std::stringstream ss;
		ss << baseURI << siteid;
		return ss.str();
	}

	std::string getSiteList()
	{
		return baseURI;
	}

	std::string Search(std::string siteid, std::string query)
	{
		std::stringstream ss;
		ss << baseURI << siteid << "/search?cover=0&info=0&l=10&q=" << query;
		return ss.str();
	}
}}
