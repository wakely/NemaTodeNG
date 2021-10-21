/*
 * GPSFix.h
 *
 *  Created on: Jul 23, 2014
 *      Author: Cameron Karlsson
 *
 *  See the license file included with this source.
 */

#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <sstream>

namespace nmea {

    class GPSSatellite;
    class GPSAlmanac;
    class GPSFix;
    class GPSService;

    /// =========================== GPS SATELLITE =====================================
    class GPSSatellite {
      public:
        GPSSatellite()=default;

        //satellite data
        double   snr{};       // 0-99 dB
        uint32_t prn{};       // id - 0-32
        double   elevation{}; // 0-90 deg
        double   azimuth{};   // 0-359 deg

        std::string toString();
        operator std::string();
    };

    /// =========================== GPS ALMANAC =======================================
    class GPSAlmanac {
        friend GPSService;

      public:
        GPSAlmanac()=default;

      private:
        uint32_t visibleSize{};
        uint32_t lastPage{};
        uint32_t totalPages{};
        uint32_t processedPages{};

        void clear();      //will remove all information from the satellites
        void updateSatellite(GPSSatellite sat);

      public:
        //mapped by prn
        std::vector<GPSSatellite> satellites;

        double averageSNR();
        double minSNR();
        double maxSNR();
        double percentComplete();

    };

    /// =========================== GPS TIMESTAMP =====================================
    // UTC time
    class GPSTimestamp {
      public:
        GPSTimestamp()=default;

      private:
        std::string monthName(uint32_t index);

      public:
        int32_t hour{};
        int32_t min{};
        double  sec{};

        int32_t month{1};
        int32_t day{1};
        int32_t year{1970};

        // Values collected directly from the GPS
        double  rawTime{0};
        int32_t rawDate{0};

        time_t getTime();

        // Set directly from the NMEA time stamp
        // hhmmss.sss
        void setTime(double raw_ts);

        // Set directly from the NMEA date stamp
        // ddmmyy
        void setDate(int32_t raw_date);

        std::string toString();
    };

    /// =========================== GPS FIX ============================================
    class GPSFix {
        friend GPSService;

      public:
        GPSFix()=default;
        virtual ~GPSFix() = default;

      private:
        bool haslock{false};
        bool setlock(bool b);    //returns true if lock status **changed***, false otherwise.

      public:
        GPSAlmanac almanac;
        GPSTimestamp timestamp;

        char status    {'V'};      // Status: A=active, V=void (not locked)
        uint8_t type   {1};     // Type: 1=none, 2=2d, 3=3d
        uint8_t quality{0};  // Quality:
        //    0 = invalid
        //    1 = GPS fix (SPS)
        //    2 = DGPS fix
        //    3 = PPS fix
        //    4 = Real Time Kinematic (RTK)
        //    5 = Float RTK
        //    6 = estimated (dead reckoning) (2.3 feature)

        double dilution{};           // Combination of Vertical & Horizontal
        double horizontalDilution{}; // Horizontal dilution of precision, initialized to 100, best =1, worst = >20
        double verticalDilution{};   // Vertical is less accurate

        double altitude{};           // meters
        double latitude{};           // degrees N
        double longitude{};          // degrees E
        double speed{};              // km/h
        double travelAngle{};        // degrees true north (0-360)
        int32_t trackingSatellites{};
        int32_t visibleSatellites{};

        bool locked();
        double horizontalAccuracy();
        double verticalAccuracy();
        bool hasEstimate();

        std::chrono::seconds timeSinceLastUpdate();  // Returns seconds difference from last timestamp and right now.

        std::string toString();
        operator std::string();

        static std::string travelAngleToCompassDirection(double deg, bool abbrev = false);
    };

}

