#include <NMEA2000.h>
#include <NMEA2000_esp32.h>
#include <N2kMessages.h>

tNMEA2000_esp32 NMEA2000;

void HandlePGN127250(const tN2kMsg &N2kMsg) {
    unsigned char SID;
    double Heading;
    double Deviation;
    double Variation;
    tN2kHeadingReference HeadingReference = N2khr_true;

    // Parse the PGN 127250 message
    if (ParseN2kHeading(N2kMsg, SID, Heading, &Deviation, &Variation, &HeadingReference)) {
        // Convert heading from radians to degrees
        double HeadingDegrees = Heading * 180.0 / M_PI;

        Serial.print("SID: ");
        Serial.print(SID);
        Serial.print(", Heading: ");
        Serial.print(HeadingDegrees);
        Serial.print(" degrees");

        if (HeadingReference == N2khr_magnetic) {
            Serial.print(" (Magnetic)");
        } else if (HeadingReference == N2khr_true) {
            Serial.print(" (True)");
        }

        Serial.println();

        if (!isnan(Deviation)) {
            Serial.print("Deviation: ");
            Serial.println(Deviation * 180.0 / M_PI);
        }

        if (!isnan(Variation)) {
            Serial.print("Variation: ");
            Serial.println(Variation * 180.0 / M_PI);
        }
    } else {
        Serial.println("Failed to parse PGN 127250.");
    }
}

void HandlePGN129025(const tN2kMsg &N2kMsg) {
    double Latitude, Longitude;

    if (ParseN2kPositionRapid(N2kMsg, Latitude, Longitude)) {
        Serial.print("PGN 129025 - Latitude: ");
        Serial.print(Latitude, 6);
        Serial.print(", Longitude: ");
        Serial.println(Longitude, 6);
    }
}

void N2kMessageHandler(const tN2kMsg &N2kMsg) {
    switch (N2kMsg.PGN) {
        case 127250: // Vessel Heading
            HandlePGN127250(N2kMsg);
            break;
        case 129025: // Position Rapid Update
            HandlePGN129025(N2kMsg);
            break;
        default:
            Serial.print("Unhandled PGN: ");
            Serial.println(N2kMsg.PGN);
            break;
    }
}

void SetupNMEA2000() {
    NMEA2000.SetMode(tNMEA2000::N2km_ListenOnly);
    NMEA2000.EnableForward(false);
    NMEA2000.SetMsgHandler(N2kMessageHandler);

    NMEA2000.Open();
}

void setup() {
    Serial.begin(115200);

    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Cores: %d\n", ESP.getChipCores());
    Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("Heap Size: %d bytes\n", ESP.getHeapSize());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("Sketch Size: %d bytes\n", ESP.getSketchSize());
    Serial.printf("Free Sketch Space: %d bytes\n", ESP.getFreeSketchSpace());    

    SetupNMEA2000();
}

void loop() {
    NMEA2000.ParseMessages();
}
