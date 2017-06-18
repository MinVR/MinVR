#include "net/VRNetClient.h"
#include "net/VRNetServer.h"

#include "config/VRDataIndex.h"
#include "config/VRDataQueue.h"

int TestSwapBufferSignal();
int TestTwo();
int TestThree();

int networktest(int argc, char* argv[]) {
//int main(int argc, char* argv[]) {

  int defaultchoice = 1;

  int choice = defaultchoice;

  if (argc > 1) {
    if(sscanf(argv[1], "%d", &choice) != 1) {
      printf("Couldn't parse that input as a number\n");
      return -1;
    }
  }

  int output;

  switch(choice) {
  case 1:
    output = TestSwapBufferSignal();
    break;

  case 2:
    output = TestTwo();
    break;

  case 3:
    output = TestThree();
    break;

    // Add case statements to handle other values.
  default:
    std::cout << "Test #" << choice << " does not exist!\n";
    output = -1;
  }

  return output;
}

int TestSwapBufferSignal() {

  int numberOfClients = 10;
  int ret;

  std::vector<pid_t> clientPIDs(numberOfClients);

  // Fork N clients to connect to the server.
  std::string launchSwapClient = std::string(BINARYPATH) + "/launchSwapClient";
  std::cout << "Using: " << launchSwapClient << std::endl;

  std::stringstream clientNumber;
  for (int i = 0; i < numberOfClients; i++) {
    clientPIDs[i] = fork();

    if (clientPIDs[i] != 0) {
      // This is still on the main fork, and the PID returned belongs
      // to the process that was forked.
      std::cout << "client " << i+1
                << " forked, pid = " << clientPIDs[i] << std::endl;

    } else {
      // The forked client returns 0 from the fork() call.

      clientNumber << i+1;
      ret = execl(launchSwapClient.c_str(),
                  launchSwapClient.c_str(),
                  clientNumber.str().c_str(), (char*)NULL);

      // Shouldn't get here, unless the execl() fails.
      if (ret < 0) {
        std::cerr << "execl number " << i << " failed: " << errno << std::endl;
        return 1;
      }
    }
  }

  std::cout << "All clients forked, open for business now." << std::endl;

	MinVR::VRNetServer server = MinVR::VRNetServer("3490", numberOfClients);

  for (int i = 0; i < 10; i++) {
    server.syncSwapBuffersAcrossAllNodes();
    std::cout << "All swaps for request " << i << " made by server." << std::endl;
  }

  // Waits for all the child processes to finish running
  for (int i = 0; i < numberOfClients; ++i) {
    int status;

    while (-1 == waitpid(clientPIDs[i], &status, WNOHANG));
    std::cout << "waited for " << clientPIDs[i] << std::endl;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      std::cout << "WIFSIGNALED=" << WIFSIGNALED(status) << '\n';
      std::cout << "WIFSTOPPED=" << WIFSTOPPED(status) << '\n';
      std::cout << "WTERMSIG=" << WTERMSIG(status) << '\n';
      std::cout << "WSTOPSIG=" << WSTOPSIG(status) << '\n';
      if (WIFSIGNALED(status) != 0) {
        cerr << "Process " << i+1 << " (pid " << clientPIDs[i] << ") failed" << endl;
        return 1;
      } else {
        return 0;
      }
    }
  }
  std::cout << "CLIENT SUCCESS" << std::endl;

  return 0; }

int TestTwo() { return 0; }

int TestThree() { return 0; }
