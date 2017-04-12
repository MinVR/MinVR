#include "MyAppController.h"

/// Main method which creates and calls application
int main(int argc, char **argv) {
	MyAppController app(argc, argv, argv[1]);
	app.run();
	return 0;
}
