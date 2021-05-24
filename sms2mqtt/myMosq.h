//_____g_myMosq.h______________________________________210315_____

#ifndef _MYMOSQ_H_
 #define _MYMOSQ_H_
#include <iostream>                    // cout
#include <string>                      // string
#include "C_mosquitto.hpp"

//_____extension of class Mosquitto_____________________________
class MyMosquitto : public Mosquitto {
 void onConnected() { std::cout << "Connected!" << std::endl; }
 void onError(const char* msg) { std::cout << "Error: " << msg << std::endl; }
 void onMessage(std::string topic, std::string payload);
};

//_____global mosquitto object__________________________________
MyMosquitto g_myMosq=MyMosquitto();

#endif // _MYMOSQ_H_
