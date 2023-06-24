#include "webserv.hpp"

int main(int args, char **a, char **env)
{

	std::string toSend("test PipeFORK");

	pid_t pid;
	int pF[2];
	int pB[2];

	pipe(pF);
	pipe(pB);
	write(pF[1], toSend.c_str(), toSend.length());
	pid = fork();
	if (pid == 0)
	{
		char **argv = new char*[2];
		argv[0] = new char[8];
		strcpy(argv[0], "test.sh");
		argv[1] = NULL;
		dup2(pF[0], 0);
		dup2(pB[1], 1);
		close(pF[1]);
		close(pF[0]);
		close(pB[1]);
		close(pB[0]);
		execve(argv[0], argv, env);
	}
	wait(NULL);
	char buf[100];
	int res = read(pB[0], buf, 100);
	buf[res] = 0;
	printf("%s\n", buf);
}