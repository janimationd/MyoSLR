#include <stdio.h>
#include <iostream>
#include <string>
#include <myo/myo.hpp>

#define TO_LOWERCASE(c) ((c) < 'a' ? (c) + 32: (c))

// sends Myo armband data along with a character to the NN server
void sendData(char c) {

}

// starts up the Myo, returns a pointer to the Myo Hub
myo::Hub* initMyo(myo::DeviceListener* dl) {
  try {
    myo::Hub* hub = new myo::Hub();
    printf("Attempting to find a myo...\n");
    myo::Myo *myo = hub->waitForMyo(10000);
    
    // if we time out, say so, then exit
    if (myo == NULL) {
      printf("Timed out while waiting for a myo to connect.\n");
      exit(1);
    }

    printf("Myo found.\n");
    // instantiate our Preprocessor module, which should be an extension of the Myo SDK's DeviceListener class
    hub->addListener(dl);

    return hub;
  }
  catch (...) {
    printf("Error starting myo!\n");
    exit(2);
  }
}

int main(int argc, char *argv[]) {
  char nextChar = '$'; // should always be lowercase
  unsigned int numChars = 0;
  char *address;

  // retrieve server address from command line arguments
  if (argc == 2) {
    address = argv[1];
  }
  else {
    printf("\nUsage:  %s nn_serv\n\n\tnn_serv - the IP address of the neural network server.\n\n", argv[0]);
    return 1;
  }

  /// set up network communication code

  // initialize of DeviceListener
  myo::DeviceListener dl;
  // initialize the Myo Hub
  myo::Hub *hub = initMyo(&dl);

  // main loop to prompt for characters
  while (true) {
    /// select nextChar

    // prompt for nextChar
    printf("Please form the gesture for '%c', type a single character, then press enter.\n", nextChar);
    std::string inputString;
    std::cin >> inputString;

    // is it a special command?
    if (inputString.length() > 1) {
      if (inputString == "help" || inputString == "Help") {
        ///print help
        continue;
      }
      else if (inputString == "quit" || inputString == "Quit") {
        printf("%u gestures trained.\n", numChars);
        return 0;
      }
      else {
        printf("Unknown command: %s", inputString.c_str());
        continue;
      }
    }
    else { // it's a single character
      /// collect data from our DeviceListener
      // send Myo data to NN Server
      sendData(nextChar);
      numChars++;
    }
  }

  return 0;
}
