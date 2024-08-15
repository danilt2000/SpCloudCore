// ReSharper disable CppClangTidyBugproneSuspiciousInclude
//#include <curl/curl.h>/*
#include <iostream>

//#include "../src/mongocxx/"s

class MongoDbService
{

public:
	MongoDbService() 
	{
		//mongocxx::instance inst{};
		//const auto uri = mongocxx::uri{ "mongodb+srv://loker:<password>@spcloudcluster.xwpnw.mongodb.net/?retryWrites=true&w=majority&appName=SpCloudCluster" };
		//// Set the version of the Stable API on the client
		//mongocxx::options::client client_options;
		//const auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
		//client_options.server_api_opts(api);
		//// Setup the connection and get a handle on the "admin" database.
		//mongocxx::client conn{ uri, client_options };
		//mongocxx::database db = conn["admin"];
		//// Ping the database.
		//const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
		//db.run_command(ping_cmd.view());
		//std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;
	}
};
