
#include <stdio.h>
#include <curl/curl.h>
#include "json.hpp"
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include "emg-upload.hpp"

using namespace nlohmann;
using namespace std;

// Call back function for curl; used to save server responses into a string
size_t save_response_callback(void *contents, size_t size, size_t nmemb, string *output) {
   size_t newLength = size*nmemb;
   size_t oldLength = output->size();
   try {
      output->resize(oldLength + newLength);
   }
   catch(bad_alloc &e){
      //handle memory problem
      return 0;
   }
   
   copy((char*)contents,(char*)contents+newLength,output->begin()+oldLength);
   return size*nmemb; 
} 

// Makes an HTTP Get request to the given host for testing
void get(const char* host) {
   printf("Testing GET from '%s'\n", host);

   CURL *curl;
   CURLcode res;
   
   curl = curl_easy_init();
   string response;
   if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, host);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_response_callback); // set our callback function
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // set where to store the server response
      res = curl_easy_perform(curl);
      if(res != CURLE_OK)
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
      else {
         printf("Reponse:\n'%s'\n", response.c_str());
         printf("\n");
      }
 
      curl_easy_cleanup(curl);
   }
} 

// Makes an HTTP Post with the given JSON object to the given host
void postJSON(const char* host, json j) {
   CURL *curl;
   CURLcode res;
   
   /* Copy the json string into our own malloc'd string
    * NOTE: This
    */
   const char* jdump = j.dump().c_str();
   char* output = (char*)malloc(sizeof(char) * strlen(jdump));
   for(int i = 0; i < strlen(jdump); i++)
      output[i] = jdump[i];
   
   // Initialize curl
   curl_global_init(CURL_GLOBAL_ALL);
   curl = curl_easy_init();
   string response;
   if(curl) {
      
      // Create header specifying this packet as JSON data
      struct curl_slist *headers;
      headers = NULL;
      headers = curl_slist_append(headers, "Content-Type: application/json");
      headers = curl_slist_append(headers, "Accept: application/json");
      headers = curl_slist_append(headers, "charsets: utf-8");
   
      curl_easy_setopt(curl, CURLOPT_URL, host);            // set the destination host
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output);   // set the POST payload (also implicitly defines the request as a POST request)
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);  // set the HTTP headers
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_response_callback); // set our callback function
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // set where to store the server response

      /* Send the request
       */
      res = curl_easy_perform(curl);
      if(res != CURLE_OK)
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
      else {
         printf("Reponse:\n'%s'\n", response.c_str());
         printf("\n");
      }
      curl_easy_cleanup(curl);
   }
   else {
      printf("Failed to initialize libcurl!\n");
   } 
   

   curl_global_cleanup();
   free(output);
} 

// Parses the given EMG csv file into a vector of emg data
vector<emg_data> parse_csv_file(const char* filename) {
   
   // Open the csv file for reading
   FILE* fp = fopen(filename, "r");
   if(fp == NULL) {
      printf("Unable to open file '%s'\n", filename);
      exit(1);
   }
   
   vector<emg_data> alldata;
   // Read through the CSV file line by line
   char buffer[BUFFER_SIZE];
   while(fgets(buffer, BUFFER_SIZE, fp) != NULL) {
      emg_data emg;
      // Tokenize the CSV by splitting on ','
      char* tok = strtok(buffer, ",");
      int idx = 0;
      while(tok != NULL) {
         // convert each data point to an int and add it to the datapoint
         sscanf(tok, "%d", &emg.data[idx++]);
         tok = strtok(NULL, ",");
      } 

      alldata.push_back(emg);
   } 
    
   fclose(fp);
   return alldata;
} 


/* Given a letter and a vector of emg_data this will make a JSON object containing that data
 * JSON Example: {"letter":"c","emg":[[1,2,3,4,5,6],[1,2,3,4,5,6]]}
 */
json make_emg_json(const char* letter, vector<emg_data> all_emg) {
   json j;
   j["letter"] = letter;
   
   // The goal is to make a 2D array of emg data points in the json object
   // To do so we will first convert the vector<emg_data> into a vector<json>
   // which our json library can interpret.
   std::vector<json> all_emg_json; 
   for (auto & emg : all_emg) {
      std::vector<int> emg_vec;
      for(int i = 0; i < EMG_DATAPOINTS; i++)
         emg_vec.push_back(emg.data[i]);
      
      json emg_json(emg_vec);
      all_emg_json.push_back(emg_json);
   }
   
   j["emg"] = all_emg_json;

   return j;
} 



/* Uploads the emg data with the given letter
 */
void upload_emg_data(const char* letter, vector<emg_data> all_emg) {
   json j = make_emg_json(letter, all_emg);
   postJSON(HOST_URL, j);
} 

/* Uploads the given EMG data, expects filename to be a CSV file with the 
 * letter it represents surrounded by '_' in the filename.
 * For example: Johnathan_A_l1.csv
 */
void upload_emg_csv(const char* filename) {
   printf("Uploading CSV '%s'\n", filename);
   vector<emg_data> all_emg = parse_csv_file(filename);

   // get letter from filename
   char letter_buf[BUFFER_SIZE];
   strcpy(letter_buf, filename);
   // tokenizes the string because it's easier than doing a string split in C/C++ imo
   char* letter = strtok(letter_buf, "_");

   if(letter == NULL)

   letter = strtok(NULL, "_");
   
   upload_emg_data(letter, all_emg);
} 


// Prints the vector of emg_data for debugging
void print_emg_data(vector<emg_data> all_emg) {
   for (auto & emg : all_emg) {
      printf("%3d", emg.data[0]);
      for(int i = 1; i < EMG_DATAPOINTS; i++)
         printf(", %3d", emg.data[i]);
      printf("\n");
   }       
} 



int main(int argc, char** argv) {

   if(argc < 2) {
      printf("Invalid Arguments\n")  ;
      printf("Usage: ./emg-upload FILE [FILE...]\n");
      exit(1);
   } 

   for(int i = 1; i < argc; i++) {
      upload_emg_csv(argv[i]);
   } 

   return 0;
} 


