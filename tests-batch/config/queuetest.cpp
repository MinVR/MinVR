#include "config/VRDataIndex.h"
#include "config/VRDataQueue.h"
#include <main/VRConfig.h>

int TestQueueArray();
int TestQueueUnpack();
int TestQueueMultipleTimeStamps();
int TestQueueIterator();
int TestAddQueue();
int TestAddQueueSerialized();

int queuetest(int argc, char* argv[]) {

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
    output = TestQueueArray();
    break;

  case 2:
    output = TestQueueUnpack();
    break;

  case 3:
    output = TestQueueMultipleTimeStamps();
    break;

  case 4:
    output = TestQueueIterator();
    break;

  case 5:
    output = TestAddQueue();
    break;

  case 6:
    output = TestAddQueueSerialized();
    break;

    // Add case statements to handle other values.
  default:
    std::cout << "Test #" << choice << " does not exist!\n";
    output = -1;
  }

  return output;
}

// This is pretty much the same function as setupIndex() in
// indextest.cpp, but has its name changed to avoid link conflicts in
// the construction of test_runner.
MinVR::VRDataIndex* setupQIndex() {

  MinVR::VRDataIndex *n = new MinVR::VRDataIndex;

  MinVR::VRInt a = 4;
  MinVR::VRFloat b = 3.1415926f;

  n->addData("/george/a0", a);
  n->addData("/george/a1", a + 1);
  n->addData("/george/a2", a + 2);
  n->addData("/george/a3", a + 3);
  n->addData("/george/a4", a + 4);
  n->addData("/george/a5", a + 5);
  n->addData("/george/a6", a + 6);
  n->addData("/george/a7", a + 7);
  n->addData("/george/a8", a + 8);
  n->addData("/george/a9", a + 9);

  n->addData("/martha/b0", b);
  n->addData("/martha/b1", b * 1);
  n->addData("/martha/b2", b * 2);
  n->addData("/martha/b3", b * 3);
  n->addData("/martha/b4", b * 4);
  n->addData("/martha/b5", b * 5);
  n->addData("/martha/b6", b * 6);
  n->addData("/martha/b7", b * 7);
  n->addData("/martha/b8", b * 8);
  n->addData("/martha/b9", b * 9);

  MinVR::VRString c = "abigail";
  n->addData("/john/c0", c + "0");
  n->addData("/john/c1", c + "1");
  n->addData("/john/c2", c + "2");
  n->addData("/john/c3", c + "3");
  n->addData("/john/c4", c + "4");
  n->addData("/john/c5", c + "5");
  n->addData("/john/c6", c + "6");
  n->addData("/john/c7", c + "7");
  n->addData("/john/c8", c + "8");
  n->addData("/john/c9", c + "9");

  std::vector<float>d;
  d.push_back(1.2f);
  d.push_back(2.3f);
  d.push_back(3.4f);
  d.push_back(4.5f);
  d.push_back(5.6f);

  n->addData("/donna/d0", d);

  // This file is specified using the WORKING_DIRECTORY option in the
  // ctest framework.  See the CMakeLists.txt file in this directory,
  // and look for the add_test command.
  n->processXMLFile(MINVR_BUILD_PREFIX "/tests-batch/config/test.xml", "/");

  return n;
}

// Removes the timestamps from a queue string so they can be compared
// with each other.
std::string removeTimeStamps(const std::string inString) {

  std::string outString = inString;

  size_t start = 0, timeStampPos, endQuotePos;

  // Find the 'timeStamp' string.
  while ((timeStampPos = outString.find("timeStamp", start)) != std::string::npos) {

    // Find the closing quote of its value.
    if ((endQuotePos = outString.find("\"", timeStampPos + 12))
        != std::string::npos) {

      // Delete the timeStamp value between the quotes.
      outString.replace(timeStampPos + 11, endQuotePos - timeStampPos - 11, "");

      start = endQuotePos;

    } else break;
  }

  return outString;
}

int TestAddQueue() {

  int out = 0;

  MinVR::VRDataQueue q1, q2;

  char ename[10], fname[10];
  std::vector<MinVR::VRRawEvent> e(10), f(10);

  for (int i = 0; i < 10; i++) {
    sprintf(ename, "ENAME%d", i);
    e[i] = MinVR::VRRawEvent(ename);
    e[i].addData("testInt", i);
    q1.push(e[i]);

    sprintf(fname, "FNAME%d", i);
    f[i] = MinVR::VRRawEvent(fname);
    f[i].addData("testInt", 10-i);
    q2.push(f[i]);
  }

  // Now we have two queues, each of which has ten entries that were
  // added in alternating fashion.  If the time stamps and everthing
  // work out right, then when the two queues are added, the entries
  // from each should appear alternating, too.

  // Add the queues.
  q1.addQueue(q2);


  std::cout << q1.printQueue();

  // This is how we expect the entries to appear.
  int expectedOrder[20] = {0,10,1,9,2,8,3,7,4,6,5,5,6,4,7,3,8,2,9,1};

  int j = 0;
  while (q1.notEmpty()) {

    std::cout << "Entry:" << q1.getFirst() << std::endl;
    if (expectedOrder[j++] != (int)q1.getFirst().getValue("testInt")) out++;
    q1.pop();
  }

  return out;
}

// This is precisely the same test as above, but with one of the
// queues composed of serialized data.  The queue is indifferent, and it
// only serializes as necessary.
int TestAddQueueSerialized() {

  int out = 0;

  MinVR::VRDataQueue q1, q2;

  char ename[10], fname[10];
  std::vector<MinVR::VRRawEvent> e(10), f(10);

  for (int i = 0; i < 10; i++) {
    sprintf(ename, "ENAME%d", i);
    e[i] = MinVR::VRRawEvent(ename);
    e[i].addData("testInt", i);
    q1.push(e[i].serialize());

    sprintf(fname, "FNAME%d", i);
    f[i] = MinVR::VRRawEvent(fname);
    f[i].addData("testInt", 10-i);
    q2.push(f[i]);
  }

  // Now we have two queues, each of which has ten entries that were
  // added in alternating fashion.  If the time stamps and everthing
  // work out right, then when the two queues are added, the entries
  // from each should appear alternating, too.

  // Add the queues.
  q1.addQueue(q2);


  // Also testing the stream output here...
  std::cout << "The asterisks imply the event data of that queue item is stored as serial data." << std::endl;
  std::cout << q1;

  // This is how we expect the entries to appear.
  int expectedOrder[20] = {0,10,1,9,2,8,3,7,4,6,5,5,6,4,7,3,8,2,9,1};

  int j = 0;
  while (q1.notEmpty()) {

    std::cout << "Entry:" << q1.getFirst() << std::endl;
    if (expectedOrder[j++] != (int)q1.getFirst().getValue("testInt")) out++;
    q1.pop();
  }

  return out;
}

int TestQueueIterator() {

  int out = 0;

  std::vector<MinVR::VRDataIndex> n(10);
  MinVR::VRDataQueue q;

  for (int i = 0; i < 10; i++) {

    char name[10];
    sprintf(name, "NAME%d", i);
    n[i] = MinVR::VRDataIndex(name);

    // To test the ordering features as well, we are spoofing the time
    // stamps, and inverting their order.
    n[i].addData("indexValue", i);
    q.push(10 - i, MinVR::VRDataQueueItem(&n[i]));
  }

  // Now add one more, with a redundant time value.
  MinVR::VRDataIndex nf = MinVR::VRDataIndex("NAMElast");
  nf.addData("indexValue", 100);
  q.push(5, MinVR::VRDataQueueItem(&nf));

  // Now we have a queue with 11 little data index objects in it.

  int j = 0;
  int expectedValue[11] = {9, 8, 7, 6, 5, 100, 4, 3, 2, 1, 0};
  for (MinVR::VRDataQueue::iterator it = q.begin(); it != q.end(); it++) {
    std::cout << "Entry:" << it->second.getData() << std::endl;
    // If the indexValue in the queue item is the same as j, all is well.
    if (expectedValue[j++] != (int)it->second.getData().getValue("indexValue"))
      out++;
  }

  return out;
}


int TestQueueArray() {

  std::string testString = "<VRDataQueue num=\"2\"><VRDataQueueItem timeStamp=\"1454671331220377\"><atestarray type=\"intarray\" separator=\"@\">0@1@2@3@4@5@6@7@8@9@10@11@12@13@14@15@16@17@18@19@20@21@22@23@24@25@26@27@28@29@30@31@32@33@34@35@36@37@38@39@40@41@42@43@44@45@46@47@48@49@50@51@52@53@54@55@56@57@58@59@60@61@62@63@64@65@66@67@68@69@70@71@72@73@74@75@76@77@78@79@80@81@82@83@84@85@86@87@88@89@90@91@92@93@94@95@96@97@98@99</atestarray></VRDataQueueItem><VRDataQueueItem timeStamp=\"1454671331220395\"><d0 type=\"floatarray\">1.200000,2.300000,3.400000,4.500000,5.600000</d0></VRDataQueueItem></VRDataQueue>";

  testString = removeTimeStamps(testString);

  MinVR::VRDataIndex *n = setupQIndex();
  MinVR::VRDataQueue *q = new MinVR::VRDataQueue;

  std::vector<int>e;

  for (int i = 0; i < 100; i++) {
    e.push_back(i);
  }

  n->addData("/george/atestarray", e);
  n->setAttributeValue("/george/atestarray", "separator", "@");

  q->push(n->serialize("atestarray", "/george/"));
  q->push(n->serialize("/donna/d0"));

  std::string output = removeTimeStamps(q->serialize());

  //std::cout << "test:" << testString << std::endl;
  //std::cout << "outp:" << output << std::endl;

  // Test that the new queue is the same as the test string.
  int out = output.compare(testString);
  delete n;
  delete q;

  return out;
}

int TestQueueUnpack() {

  std::string testString;

  // Create an index and a queue.
  MinVR::VRDataIndex *n = setupQIndex();
  MinVR::VRDataQueue *q = new MinVR::VRDataQueue;

  std::vector<int> e;

  for (int i = 0; i < 100; i++) {
    e.push_back(i);
  }

  // Add an array to the index.
  n->addData("/george/atestarray", e);

  // Get the serialized version of an index object.
  testString = n->serialize("/george/atestarray");

  // Put that object into the queue.
  q->push(n->serialize("/george"));

  // Unpack the serialized object.
  MinVR::VRDataIndex index = q->getFirst();

  // Unpack it into a different index.
  std::string output = index.serialize("atestarray");

  // std::cout << "testString:" << testString << std::endl;
  // std::cout << "output    :" << output << std::endl;

  // Does it match?
  int out = testString.compare(output);

  // The "george" name becomes the name of the index itself.
  std::string testName = "george";
  out += testName.compare(index.getName());

  delete n;
  delete q;

  return out;
}

int TestQueueMultipleTimeStamps() {

  std::string testString = "<VRDataQueue num=\"4\"><VRDataQueueItem timeStamp=\"1484015499734567-000\"><vladimir type=\"container\"><earray type=\"intarray\">0,1,2,3,4,5,6,7,8,9</earray></vladimir></VRDataQueueItem><VRDataQueueItem timeStamp=\"1484015499734567-001\"><estragon type=\"container\"><farray type=\"intarray\">10,11,12,13,14,15,16,17,18,19</farray></estragon></VRDataQueueItem><VRDataQueueItem timeStamp=\"1484015499734567-002\"><pozzo type=\"container\"><garray type=\"intarray\">20,21,22,23,24,25,26,27,28,29</garray></pozzo></VRDataQueueItem><VRDataQueueItem timeStamp=\"1484015499734567-003\"><lucky type=\"container\"><harray type=\"intarray\">30,31,32,33,34,35,36,37,38,39</harray></lucky></VRDataQueueItem></VRDataQueue>";

  int out = 0;

  // Create an index and a queue.
  MinVR::VRDataIndex *n = new MinVR::VRDataIndex;
  MinVR::VRDataQueue *q = new MinVR::VRDataQueue;

  std::vector<int> e;
  std::vector<int> f;
  std::vector<int> g;
  std::vector<int> h;

  for (int i = 0; i < 10; i++) {
    e.push_back(i);
    f.push_back(10 + i);
    g.push_back(20 + i);
    h.push_back(30 + i);
  }

  // Add these arrays to the index.
  n->addData("/vladimir/earray", e);
  n->addData("/estragon/farray", f);
  n->addData("/pozzo/garray", g);
  n->addData("/lucky/harray", h);

  // Put that object into the queue.
  q->push(((long long)1484015499734567), n->serialize("/vladimir"));
  q->push(((long long)1484015499734567), n->serialize("/estragon"));
  q->push(((long long)1484015499734567), n->serialize("/pozzo"));
  q->push(((long long)1484015499734567), n->serialize("/lucky"));

  // Make sure it serializes the way we expect.
  out += testString.compare(q->serialize());
  std::cout << "queue:" << q->serialize() << std::endl;


  // Make sure the values with the same timestamps come out in a
  // consistent order.
  out += q->getFirst().serialize().substr(1, 8).compare("vladimir");
  q->pop();

  out += q->getFirst().serialize().substr(1, 8).compare("estragon");
  q->pop();

  out += q->getFirst().serialize().substr(1, 5).compare("pozzo");
  q->pop();

  out += q->getFirst().serialize().substr(1, 5).compare("lucky");
  q->pop();

  return out;
}
