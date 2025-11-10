/**
 * SquashFS delta tools
 * (c) 2025 Canonical
 * Released under the terms of the 2-clause BSD license
 */
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <iostream>

#include <cerrno>
#include <cstring>

extern "C"
{
#	include <sys/types.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <arpa/inet.h>
}

#include "util.hxx"
#include "delta.hxx"

int XDelta::delta(const char *path1, const char *path2, int out_fd)
{
	std::cerr << "Calling xdelta3 to generate the diff..." << std::endl;
	pid_t child = fork();
	if (child == -1)
		throw IOError("fork() failed", errno);
	if (child == 0)
	{
		try
		{
			// in child
			if (close(1) == -1)
				throw IOError("Unable to close stdout", errno);
			if (dup2(out_fd, 1) == -1)
				throw IOError("Unable to override stdout via dup2()", errno);

			if (execlp("xdelta3",
				   "xdelta3", "-v", "-9", "-S", "djw",
				   "-s", path1, path2, static_cast<const char*>(0)) == -1)
				throw IOError("execlp() failed", errno);
		}
		catch (IOError& e)
		{
			std::cerr << "Error occured in child process:\n\t"
				  << e.what() << "\n\terrno: " << strerror(e.errno_val) << "\n";
			return 1;
		}
	}
	else
	{
		int status;

		waitpid(child, &status, 0);

		if (WEXITSTATUS(status) != 0)
		{
			std::cerr << "Child process terminate with error status\n"
				"\treturn code: " << WEXITSTATUS(status) << "\n";
			return status;
		}
        }
	return 0;
}

int BsDiff::delta(const char *path1, const char *path2, int fd_out)
{
	std::cerr << "Calling bsdiff to generate the diff..." << std::endl;
	std::string deltaPath(get_temp_filename());

        pid_t child = fork();
	if (child == -1)
		throw IOError("fork() failed", errno);
	if (child == 0)
	{
		// in child
		if (execlp("bsdiff", "bsdiff", path1, path2, deltaPath.c_str(),
                           static_cast<const char*>(0)) == -1) {
			std::cerr << "Error occured in child process:\n\t"
				  << "execlp() failed\n\terrno: " << errno << "\n";
			return 1;
                }
		copy_file_to_fd(deltaPath.c_str(), fd_out);
	}
	else
	{
		int status;

		waitpid(child, &status, 0);

		if (WEXITSTATUS(status) != 0)
		{
			std::cerr << "Child process terminate with error status\n"
				"\treturn code: " << WEXITSTATUS(status) << "\n";
			return status;
		}
        }
	return 0;
}

