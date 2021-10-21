/*
 * NMEAParser.h
 *
 *  Created on: Aug 12, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#pragma once

#include <nmeaparse/Event.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <exception>

//read class definition for info
constexpr int NMEA_PARSER_MAX_BUFFER_SIZE{2000};

namespace nmea {

    /////////////////////////////////////////////////////////////////////////////////////////
    class NMEAParser;

    /////////////////////////////////////////////////////////////////////////////////////////
    class NMEASentence {
        friend NMEAParser;
      public:
        NMEASentence()=default;
        virtual ~NMEASentence()=default;

      private:
        bool isvalid{false};
      public:
        std::string text;      //whole plaintext of the received command
        std::string name;      //name of the command
        std::vector<std::string> parameters;  //list of parameters from the command
        std::string checksum;
        bool checksumIsCalculated{false};
        uint8_t parsedChecksum{0};
        uint8_t calculatedChecksum{0};

        enum MessageID {    // These ID's are according to NMEA standard.
          Unknown = -1,
          GGA = 0,
          GLL = 1,
          GSA = 2,
          GSV = 3,
          RMC = 4,
          VTG = 5,    // notice missing 6,7
          ZDA = 8
        };

        bool checksumOK() const;

        bool valid() const;

    };

    /////////////////////////////////////////////////////////////////////////////////////////
    class NMEAParseError : public std::exception {
      public:
        NMEAParseError(std::string msg);
        NMEAParseError(std::string msg, NMEASentence n);
        virtual ~NMEAParseError();

      public:
        std::string message;
        NMEASentence nmea;
        std::string what();
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    class NMEAParser {
      public:
        NMEAParser()=default;
        virtual ~NMEAParser()=default;

      private:
        std::unordered_map<std::string, std::function<void(NMEASentence)>> eventTable;
        std::string buffer;
        bool fillingbuffer{false};
        uint32_t maxbuffersize{NMEA_PARSER_MAX_BUFFER_SIZE};    //limit the max size if no newline ever comes... Prevents huge buffer string internally

        //fills the given NMEA sentence with the results of parsing the string.
        void parseText(NMEASentence &nmea, std::string s);
        void onInfo(NMEASentence &n, std::string s);
        void onWarning(NMEASentence &n, std::string s);
        void onError(NMEASentence &n, std::string s);

      public:
        bool log{false};

        Event<void(const NMEASentence &)> onSentence;        // called every time parser receives any NMEA sentence

        //one handler called for any named sentence where name is the "cmdKey"
        void setSentenceHandler(std::string cmdKey, std::function<void(const NMEASentence &)> handler);

        std::string getRegisteredSentenceHandlersCSV();  // show a list of message names that currently have handlers.

        // Byte streaming functions
        void readByte(uint8_t b);
        void readBuffer(uint8_t *b, uint32_t size);
        void readBuffer(std::vector<uint8_t> in){ readBuffer(in.data(), in.size());}
        void readLine(std::string line);

        // This function expects the data to be a single line with an actual sentence in it, else it throws an error.
        void readSentence(std::string cmd);   // called when parser receives a sentence from the byte stream. Can also be called by user to inject sentences.

        static uint8_t calculateChecksum(std::string);    // returns checksum of string -- XOR

    };

}
