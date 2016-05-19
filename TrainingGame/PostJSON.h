#pragma once


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

#define HOST_URL "http://104.131.142.121/trainer/letter"


using namespace nlohmann;
using namespace std;

#define EMG_DATAPOINTS 8
typedef struct emg_data {
  int data[EMG_DATAPOINTS];
} emg_data;

//void get(const char* host) {
//  printf("Testing GET from '%s'\n", host);
//
//  CURL *curl;
//  CURLcode res;
//
//  curl = curl_easy_init();
//  if (curl) {
//    curl_easy_setopt(curl, CURLOPT_URL, host);
//    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//    res = curl_easy_perform(curl);
//    if (res != CURLE_OK)
//      fprintf(stderr, "curl_easy_perform() failed: %s\n",
//        curl_easy_strerror(res));
//    else
//      printf("\n");
//
//    curl_easy_cleanup(curl);
//  }
//}

void postJSON(const char* host, json j) {
  CURL *curl;
  CURLcode res;

  const char* jdump = j.dump().c_str();
  char* output = (char*)malloc(sizeof(char) * strlen(jdump));
  for (int i = 0; i < strlen(jdump); i++)
    output[i] = jdump[i];

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl) {
    struct curl_slist *headers;
    headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");

    curl_easy_setopt(curl, CURLOPT_URL, host);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
    else
      printf("\n");
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  free(output);
}

vector<emg_data> parse_csv_file(const char* filename) {

  vector<emg_data> alldata;
  FILE *fp;

  fopen_s(&fp, filename, "r");
  if (fp == NULL) {
    printf("Unable to open file '%s'\n", filename);
    exit(1);
  }

#define BUFFER_SIZE 256
  char buffer[BUFFER_SIZE];
  while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
    emg_data emg;
    char* tok;
    strtok_s(buffer, ",", &tok);
    int idx = 0;
    while (tok != NULL) {
      sscanf_s(tok, "%d", &emg.data[idx++]);
      strtok_s(NULL, ",", &tok);
    }

    alldata.push_back(emg);
  }

  return alldata;
}

json make_emg_json(vector<emg_data> all_emg) {
  json j;

  std::vector<json> all_emg_json;
  for (auto & emg : all_emg) {
    std::vector<int> emg_vec;
    for (int i = 0; i < EMG_DATAPOINTS; i++)
      emg_vec.push_back(emg.data[i]);

    json emg_json(emg_vec);
    all_emg_json.push_back(emg_json);
  }

  j["emg"] = all_emg_json;

  return j;
}

void print_emg_data(vector<emg_data> all_emg) {
  for (auto & emg : all_emg) {
    printf("%3d", emg.data[0]);
    for (int i = 1; i < EMG_DATAPOINTS; i++)
      printf(", %3d", emg.data[i]);
    printf("\n");
  }
}

//int main1(int argc, char** argv) {
//
//  if (argc != 3) {
//    printf("Invalid Arguments\n");
//    printf("Usage: ./json-test.cpp letter emg-csv\n");
//    exit(1);
//  }
//
//  char* letter = argv[1];
//  char* csvfile = argv[2];
//
//  printf("Letter: %s\n", letter);
//  printf("CSV File: %s\n\n", csvfile);
//
//  vector<emg_data> all_emg = parse_csv_file(csvfile);
//  json j = make_emg_json(letter, all_emg);
//
//  postJSON(HOST_URL, j);
//
//  return 0;
//}


