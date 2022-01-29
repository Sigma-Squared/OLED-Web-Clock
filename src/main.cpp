#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include "httpRequest.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, RST_OLED);

const char *ssid = "Skynet";
const char *password = "";

HTTPClient http;

const char *ntpServer = "pool.ntp.org";

struct TzOffset
{
  int tz_offset;
  int dst_offset;
};

TzOffset getTzOffsetByIp(String ip)
{
  TzOffset tzOffset;
  tzOffset.tz_offset = 0;
  tzOffset.dst_offset = 0;

  String timeData = httpGet(http, "http://worldtimeapi.org/api/ip/" + ip);

  StaticJsonDocument<512> timeDataJson;
  auto error = deserializeJson(timeDataJson, timeData.c_str());

  if (error)
  {
    Serial.printf("Failed to parse timezone data: %s", error.c_str());
    return tzOffset;
  }

  tzOffset.tz_offset = timeDataJson["raw_offset"].as<int>();
  tzOffset.dst_offset = timeDataJson["dst_offset"].as<int>();
  return tzOffset;
}

void configureTimeWithNetwork()
{
  String publicIp = httpGet(http, "http://api.ipify.org/");
  TzOffset tzOffset = getTzOffsetByIp(publicIp);
  Serial.printf("Timzeone offset: %i Daylight savings offset: %i\n", tzOffset.tz_offset, tzOffset.dst_offset);

  //init and get the time
  configTime(tzOffset.tz_offset, tzOffset.dst_offset, ntpServer);
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time. Retry network configure...");
    configureTimeWithNetwork();
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

struct tm *timeInfo;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  Wire.begin(SDA_OLED, SCL_OLED, 400000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, false))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  //connect to WiFi
  display.printf("Connecting to %s ", ssid);
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    display.print(".");
    display.display();
  }
  display.print("\nConnected!\n");
  display.display();

  configureTimeWithNetwork();
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  timeInfo = new tm();
}

void loop()
{
  //struct tm timeinfo;
  if (getLocalTime(timeInfo))
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor((128 - 12 * 8) / 2, (64 - 12) / 2);
    display.println(timeInfo, "%H:%M:%S");
    display.setTextSize(1);
    display.setCursor(0, 64 - 8);
    display.println(timeInfo, "%B %d %Y");
    display.display();
  }
  delay(1000);
}