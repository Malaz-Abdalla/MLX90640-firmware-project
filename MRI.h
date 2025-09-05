#ifndef MRI_H
#define MRI_H

struct mostRecentinfo {
  float mostRecentAvg, mostRecentMin, mostRecentMax, mostRecentSD;
  char mostRecentTimestamp[32];
};

extern mostRecentinfo MRFI;
extern mostRecentinfo MRWI;

#endif