// SpCloudMain.cpp : Defines the entry point for the application.
//

// ReSharper disable CppClangTidyBugproneSuspiciousInclude

#include "SpCloudMain.h"

#include "Controllers/PublishController.cpp"

using namespace std;

int main()
{
	std::cout << "SpCloud start\n";

	PublishController publish_controller;

}
