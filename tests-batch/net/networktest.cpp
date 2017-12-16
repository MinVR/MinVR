#include "net/VRNetClient.h"
#include "net/VRNetServer.h"

#include "config/VRDataIndex.h"
#include "config/VRDataQueue.h"

int TestSwapBufferSignal();
int TestExchangeEventData();
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
    output = TestExchangeEventData();
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

#ifdef WIN32
  return true;
#else

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
#endif
}


int TestSwapBufferSignal() {

#ifdef WIN32
  return 0;
#else

  // This isn't much of a test, since all it does is fork a bunch of
  // clients and execute swapBuffer messages with them.  Since the
  // syncSwapBuffers method is a void return, there isn't much to
  // test, but if this runs to completion, you do know that the
  // functionality is basically all there.

  int out = 0;
  int numberOfClients = 10;
  int ret;

  std::vector<pid_t> clientPIDs(numberOfClients);

  // Fork N clients to connect to the server.
  std::string launchSwapClient = std::string(BINARYPATH) + "/bin/launchSwapClient";
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
    std::cout << "All swap requests " << i << " made by server." << std::endl;
  }

  std::cout << "done sending, now waiting for children to exit..." << std::endl;

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
      std::cout << "Process " << i+1 << " (pid " << clientPIDs[i] << ") failed" << std::endl;
      out += 1;
    }
  }
  std::cout << "CLIENT SUCCESS" << std::endl;

  return out;
#endif
}

int TestExchangeEventData() {

#ifdef WIN32
  return 0;
#else

  // This is the same as the test of the swapBuffers message, but it
  // passes event data back and forth, so we can check that as a test.
  // Plus, of course, if the test runs to completion, that's useful
  // information, too.

  int out = 0;
  int numberOfClients = 10;
  int numberOfSends = 3;
  int ret;

  std::vector<pid_t> clientPIDs(numberOfClients);

  // Fork N clients to connect to the server.
  std::string launchEventClient = std::string(BINARYPATH) + "/bin/launchEventClient";
  std::cout << "Using: " << launchEventClient << std::endl;

  char clientNumberStr[10];
  char numberOfSendsStr[10];
  sprintf(numberOfSendsStr, "%d", numberOfSends);
  for (int i = 0; i < numberOfClients; i++) {
    // Here's the fork...
    clientPIDs[i] = fork();

    if (clientPIDs[i] != 0) {
      // ... we are still on the main fork, and the PID returned belongs
      // to the process that was forked.
      std::cout << "client " << i+1
                << " forked, pid = " << clientPIDs[i] << std::endl;

    } else {
      // ... we are on a fork (client returns 0 from the fork() call),
      // so execute the launchEventClient helper program.

      sprintf(clientNumberStr, "%d", i+1);
      std::cout << "running: " << launchEventClient << " " << std::string(clientNumberStr) << " " << numberOfSends << std::endl;
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

    std::cout << queue;

    int gsum = 0;
    // Let's test the result.  The clients all send us events with 'client=n'.
    while (queue.notEmpty()) {
      MinVR::VRRawEvent g = queue.getFirst();
      if (g.exists("client")) {
        std::cout << g << std::endl;
        gsum += (int)g.getValue("client");
      }
      queue.pop();
    }

    // This is related to the number of clients.  If you change N,
    // change this sum.
    if (gsum != 55) out++;
  }

  std::cout << "done sending, now waiting for children to exit..." << std::endl;

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

      std::cerr << "Process " << i+1 << " (pid " << clientPIDs[i] << ") failed" << std::endl;
      out += 1;
    }
  }
  std::cout << "CLIENT SUCCESS" << std::endl;

  return out;
#endif
}

int TestThree() { return 0; }
