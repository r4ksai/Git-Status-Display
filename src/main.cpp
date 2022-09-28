#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CS_PIN    5

MD_MAX72XX rawDisplay = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Loading Animation
#define ANIMATION_DELAY 75	// milliseconds
#define MAX_FRAMES      4   // number of animation frames

const uint8_t pacman[MAX_FRAMES][18] =  // ghost pursued by a pacman
{
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00 },
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c },
};
const uint8_t DATA_WIDTH = (sizeof(pacman[0])/sizeof(pacman[0][0]));

uint32_t prevTimeAnim = 0;  // remember the millis() value in animations
int16_t idx;                // display index (column)
uint8_t frame;              // current animation frame
uint8_t deltaFrame;         // the animation frame offset for the next frame


const char* ssid = "Git Status Tracker";
const char* password = "123456789";

WebServer server(80);

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

void handleHome() {
  String homePage = "<!DOCTYPE html><html>\n";
  homePage +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  homePage +="<title>Git Status Tracker</title>\n";
  homePage +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  homePage +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  homePage +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  homePage +="</style>\n";
  homePage +="</head>\n";
  homePage +="<body>\n";
  homePage +="<h1>Git Status Tracker</h1>\n";
  homePage +="<h3>Configure the Device</h3>\n";
  homePage +="<form>\n";
  homePage +="<input placeholder=\"SSID\" /> \n";
  homePage +="<input placeholder=\"Password\" /> \n";
  homePage +="<input type=\"submit\" value=\"Submit\" /> \n";
  homePage +="</form>\n";
  homePage +="</body>\n";
  homePage +="</html>\n";

  server.send(200, "text/html", homePage);
}

void accessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  delay(100);
  IPAddress IP(192, 168, 0, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(IP, IP, NMask);
}

void setup(void)
{
  accessPoint();
  server.on("/", handleHome);
  server.onNotFound(handleNotFound);
  server.begin();
  rawDisplay.begin();
  rawDisplay.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/15);
  rawDisplay.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  rawDisplay.clear();
}

void loading() {
  static boolean bInit = true;  // initialise the animation

  // Is it time to animate?
  if (millis()-prevTimeAnim < ANIMATION_DELAY)
    return;

  prevTimeAnim = millis();      // starting point for next time

  rawDisplay.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // Initialize
  if (bInit)
  {
    rawDisplay.clear();
    idx = 0;
    frame = 0;
    deltaFrame = 1;
    bInit = false;

    // Lay out the dots
    for (uint8_t i=0; i<MAX_DEVICES; i++)
    {
      rawDisplay.setPoint(3, (i*COL_SIZE) + 3, true);
      rawDisplay.setPoint(4, (i*COL_SIZE) + 3, true);
      rawDisplay.setPoint(3, (i*COL_SIZE) + 4, true);
      rawDisplay.setPoint(4, (i*COL_SIZE) + 4, true);
    }
  }

  // clear old graphic
  for (uint8_t i=0; i<DATA_WIDTH; i++)
    rawDisplay.setColumn(idx-DATA_WIDTH+i, 0);
  // move reference column and draw new graphic
  idx++;
  for (uint8_t i=0; i<DATA_WIDTH; i++)
    rawDisplay.setColumn(idx-DATA_WIDTH+i, pacman[frame][i]);

  // advance the animation frame
  frame += deltaFrame;
  if (frame == 0 || frame == MAX_FRAMES-1)
    deltaFrame = -deltaFrame;

  // check if we are completed and set initialise for next time around
  bInit = (idx == rawDisplay.getColumnCount()+DATA_WIDTH);

  rawDisplay.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

}

void loop(void)
{
  server.handleClient();
  loading();
}

