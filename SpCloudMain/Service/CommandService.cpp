#pragma once
#include <iostream>

class CommandService
{

public:
	static void execute_command(const std::string& command) {
		int result = std::system(command.c_str());  // NOLINT(concurrency-mt-unsafe)
		if (result != 0) {
			std::cerr << "Command failed with code: " << result << std::endl;
		}
	}
};

