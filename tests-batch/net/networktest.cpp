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

bool checkClientExitStatus(const int &status) {

  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {

    std::cout << "WIFEXITED=" << WIFEXITED(status) << std::endl;
    std::cout << "WEXITSTATUS=" << WEXITSTATUS(status) << std::endl;
    std::cout << "WIFSIGNALED=" << WIFSIGNALED(status) << std::endl;
    std::cout << "WIFSTOPPED=" << WIFSTOPPED(status) << std::endl;
    std::cout << "WTERMSIG=" << WTERMSIG(status) << std::endl;
    std::cout << "WSTOPSIG=" << WSTOPSIG(status) << std::endl;

    if (WIFSIGNALED(status) != 0) {
      return true;
    } else {
      return false;
    }
  } else {
    return true;
  }
}


int TestSwapBufferSignal() {

  int out = 0;
  int numberOfClients = 10;
  int ret;

  std::vector<pid_t> clientPIDs(numberOfClients);

  // Fork N clients to connect to the server.
  std::string launchSwapClient = std::string(BINARYPATH) + "/launchSwapClient";
  std::cout << "Using: " << launchSwapClient << std::endl;

  char clientNumberStr[10];
  for (int i = 0; i < numberOfClients; i++) {
    clientPIDs[i] = fork();

    if (clientPIDs[i] != 0) {
      // This is still on the main fork, and the PID returned belongs
      // to the process that was forked.
      std::cout << "client " << i+1
                << " forked, pid = " << clientPIDs[i] << std::endl;

    } else {
      // The forked client returns 0 from the fork() call.

      sprintf(clientNumberStr, "%d", i+1);
      ret = execl(launchSwapClient.c_str(),
                  launchSwapClient.c_str(),
                  clientNumberStr, (char*)NULL);

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

    if (!checkClientExitStatus(status)) {
      cerr << "Process " << i+1 << " (pid " << clientPIDs[i] << ") failed" << endl;
      out += 1;
    }
  }
  std::cout << "CLIENT SUCCESS" << std::endl;

  return out;
}

int TestTwo() {

  int out = 0;
  int numberOfClients = 10;
  int numberOfSends = 3;
  int ret;

  std::vector<pid_t> clientPIDs(numberOfClients);

  // Fork N clients to connect to the server.
  std::string launchEventClient = std::string(BINARYPATH) + "/launchEventClient";
  std::cout << "Using: " << launchEventClient << std::endl;

  char clientNumberStr[10];
  char numberOfSendsStr[10];
  sprintf(numberOfSendsStr, "%d", numberOfSends);
  for (int i = 0; i < numberOfClients; i++) {
    clientPIDs[i] = fork();

    if (clientPIDs[i] != 0) {
      // This is still on the main fork, and the PID returned belongs
      // to the process that was forked.
      std::cout << "client " << i+1
                << " forked, pid = " << clientPIDs[i] << std::endl;

    } else {
      // The forked client returns 0 from the fork() call.


      sprintf(clientNumberStr, "%d", i+1);
      std::cout << "running:" << launchEventClient << " " << std::string(clientNumberStr) << ":" << i << " " << numberOfSends << std::endl;
      ret = execl(launchEventClient.c_str(),
                  launchEventClient.c_str(),
                  clientNumberStr,
                  numberOfSendsStr, (char*)NULL);

      // Shouldn't get here, unless the execl() fails.
      if (ret < 0) {
        std::cerr << "execl number " << i << " failed: " << errno << std::endl;
        return 1;
      }
    }
  }

  std::cout << "All clients forked, open for business now." << std::endl;

	MinVR::VRNetServer server = MinVR::VRNetServer("3490", numberOfClients);

  for (int i = 0; i < numberOfSends; i++) {

    std::cout << "Server cycle " << i << std::endl;

    MinVR::VRDataQueue queue;
    MinVR::VRRawEvent e = MinVR::VRRawEvent("testEvent");
    e.addData("testInt", i);
    e.addData("testFloat", ((float)i)/7.0f);
    queue.push(e);

    queue = server.syncEventDataAcrossAllNodes(queue);
    std::cout << "All event requests " << i << " made by server." << std::endl;
  }

  std::cout << "done sending, now waiting..." << std::endl;

  // Waits for all the child processes to finish running
  for (int i = 0; i < numberOfClients; ++i) {
    int status;

    while (-1 == waitpid(clientPIDs[i], &status, WNOHANG | WUNTRACED)) {
      std::cout << i << ":" << clientPIDs[i] << ":errno:" << errno << std::endl;
      // If errno == 10, the process has already ended.
      if (errno == 10) break;
    };
    std::cout << "waited for " << clientPIDs[i] << std::endl;

    if (!checkClientExitStatus(status)) {

      cerr << "Process " << i+1 << " (pid " << clientPIDs[i] << ") failed" << endl;
      out += 1;
    }
  }
  std::cout << "CLIENT SUCCESS" << std::endl;

  return out;
}

int TestThree() { return 0; }
