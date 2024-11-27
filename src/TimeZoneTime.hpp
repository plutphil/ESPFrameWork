// https://mischianti.org/2020/08/08/network-time-protocol-ntp-timezone-and-daylight-saving-time-dst-with-esp8266-esp32-or-arduino/
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <time.h>
#include <Timezone.h> // https://github.com/JChristensen/Timezone

/**
 * Input time in epoch format and return tm time format
 * by Renzo Mischianti <www.mischianti.org>
 */
static tm getDateTimeByParams(long time)
{
    struct tm *newtime;
    const time_t tim = time;
    newtime = localtime(&tim);
    return *newtime;
}
/**
 * Input tm time format and return String with format pattern
 * by Renzo Mischianti <www.mischianti.org>
 */
static String getDateTimeStringByParams(tm *newtime, char *pattern = (char *)"%d/%m/%Y %H:%M:%S")
{
    char buffer[30];
    strftime(buffer, 30, pattern, newtime);
    return buffer;
}

/**
 * Input time in epoch format format and return String with format pattern
 * by Renzo Mischianti <www.mischianti.org>
 */
static String getEpochStringByParams(long time, char *pattern = (char *)"%d/%m/%Y %H:%M:%S")
{
    //    struct tm *newtime;
    tm newtime;
    newtime = getDateTimeByParams(time);
    return getDateTimeStringByParams(&newtime, pattern);
}

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
// NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
int GTMOffset = 0; // SET TO UTC TIME
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", GTMOffset*60*60, 60*60*1000);
// NTPClient timeClient(ntpUDP, "pool.ntp.org", GTMOffset*60*60, 60*60*1000);
NTPClient timeClient(ntpUDP, "pool.ntp.org", GTMOffset * 60 * 60, 60 * 60 * 1000);
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};   // Central European Standard Time
Timezone CE(CEST, CET);
bool gottime = false;
String ntpserverhost = "europe.pool.ntp.org";
void setup_timezonetime()
{
    ntpserverhost = getAddDefault("NTP_SERVER", "europe.pool.ntp.org");
    timeClient = NTPClient(ntpUDP, ntpserverhost.c_str(), GTMOffset * 60 * 60, 60 * 60 * 1000);
    timeClient.begin();
    if (timeClient.update())
    {
        Serial.print("Adjust local clock");
        unsigned long epoch = timeClient.getEpochTime();
        setTime(epoch);
    }
    else
    {
        Serial.print("NTP Update not WORK!!");
    }
}
unsigned long int timer1s = 0;
unsigned long int timer12h = 0;
void loop_timezontime()
{
    // I print the time from local clock but first I check DST
    // to add hours if needed

    if (timer12h < millis())
    {
        timer12h = millis() + 1000 * 60 * 60 * 12;
    }
    if (timer1s < millis())
    {
        timer1s = millis() + 1000;
        if (gottime)
        {
            // Serial.println(getEpochStringByParams(CE.toLocal(now())));
        }
        else
        {
            if (timeClient.update())
            {
                Serial.print("Adjust local clock");
                unsigned long epoch = timeClient.getEpochTime();
                setTime(epoch);
                gottime = true;
            }
            else
            {
                Serial.print("NTP Update not WORK!!");
            }
        }
    }

    // delay(1000);
}