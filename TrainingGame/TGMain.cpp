#define _USE_MATH_DEFINES
#include <cmath>
#include <array>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <Windows.h>
#include <myo/myo.hpp>
#include "PostJSON.h"

#define TO_LOWERCASE(c) ((c) < 'a' ? (c) + 32: (c))

class DataCollector : public myo::DeviceListener {
public:
  DataCollector()
    : emgSamples(), sampleList(), recording(false)
  {
  }
  // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
  void onUnpair(myo::Myo* myo, uint64_t timestamp)
  {
    // We've lost a Myo.
    // Let's clean up some leftover state.
    emgSamples.fill(0);
  }
  // onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
  void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
  {
    if (recording) {
      sampleList.push_back(emg_data());
      for (int i = 0; i < 8; i++) {
        emgSamples[i] = emg[i];
        sampleList.at(sampleList.size() - 1).data[i] = emg[i];
      }
    }
  }

  // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
  // as a unit quaternion.
  void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
  {
    using std::atan2;
    using std::asin;
    using std::sqrt;
    using std::max;
    using std::min;
    // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
    float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
      1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
    float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
    float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
      1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
    // Convert the floating point angles in radians to a scale from 0 to 18.
    roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
    pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
    yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
  }

  // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
  // For this example, the functions overridden above are sufficient.
  // We define this function to print the current values that were updated by the on...() functions above.
  void print(std::ostream* out)
  {
    if (emgSamples[0] == 0 && emgSamples[1] == 0 && emgSamples[2] == 0 && emgSamples[3] == 0 && emgSamples[4] == 0 && emgSamples[5] == 0 && emgSamples[6] == 0 && emgSamples[7] == 0) {
      return;
    }
    // Print out the EMG data.
    for (size_t i = 0; i < emgSamples.size(); i++) {
      std::ostringstream oss;
      oss << static_cast<int>(emgSamples[i]);
      std::string emgString = oss.str();
      *out << emgString << ',';
    }
    //*out << roll_w << ',' << pitch_w << ',' << yaw_w;
    *out << '\n';
  }

  // Get and clear the list of emg_data
  std::vector<emg_data> getAndClearEMGDataList() {
    auto result = std::vector<emg_data>();
    for (int i = 0; i < sampleList.size(); i++) {
      result.push_back(sampleList.at(i));
    }
    sampleList.clear();
    recording = false;
    return result;
  }

  // Whether or not we are currently recording
  bool recording;
  // The values of this array is set by onEmgData() above.
  std::array<int8_t, 8> emgSamples;
  // These values are set by onOrientationData() and onPose() above.
  int roll_w, pitch_w, yaw_w;
  // The list of emg data collected
  std::vector<emg_data> sampleList;
};

// sends Myo armband data to the NN server
void sendData(char c, DataCollector *dc) {
  std::vector<emg_data> samples = dc->getAndClearEMGDataList();
  json j = make_emg_json(samples);
  postJSON(HOST_URL, j);
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

void main(int argc, char *argv[]) {
  char nextChar = '$'; // should always be lowercase
  unsigned int numChars = 0;

  /// set up network communication code

  // initialize of DeviceListener
  DataCollector dc;
  // initialize the Myo Hub
  myo::Hub *hub = initMyo(&dc);

  // Configure console
  DWORD mode;
  HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(handle, &mode);
  SetConsoleMode(handle, 0);

  bool done = false;
  // main loop to prompt for characters
  while (!done) {
    /// select nextChar

    // prompt for nextChar
    printf("Please form a gesture for any letter from 'a' -> 'e' then press any key to record. Press any key again to stop recording.\nType 'Q' to quit.\n");
    char input = getchar();

    // is it a special command?
    if (input == 'Q') {
      done = true;
    }
    else { // record
      dc.recording = true;
      // wait for next keypress
      input = getchar();
      // send Myo data to NN Server
      sendData(nextChar, &dc);
      numChars++;
    }
  }
  printf("Read in %d characters.\n", numChars);
  SetConsoleMode(handle, mode);
}