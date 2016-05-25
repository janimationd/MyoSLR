
#ifndef EMG_UPLOAD_HPP
#define EMG_UPLOAD_HPP

using namespace std;

// The host to send the data to
#define HOST_URL "http://104.131.142.121/trainer/letter"

// Our EMG Datapoint Structure
#define EMG_DATAPOINTS 8
typedef struct emg_data {
   int data[EMG_DATAPOINTS]; 
} emg_data;

#define BUFFER_SIZE 256


/* Uploads the given emg data with the given letter.
 * Creates a JSON object from the data and makes an HTTP POST to the server.
 */
void upload_emg_data(const char* letter, vector<emg_data> all_emg);

/* Parses the given emg CSV file, gathering all the emg data, as well as
 * extracting the letter from the file name.
 * Expects the letter to be embedded into the file name surrounded by underscores ('_')
 * For example: Johnathan_A_l1.csv would be the letter 'A' 
 */
void upload_emg_csv(const char* filename);

/* Utility function that neatly prints out all the emg data in the given vector
 */
void print_emg_data(vector<emg_data> all_emg);


#endif

