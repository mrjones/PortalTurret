// Adapted from arduino WebServer example.

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);
EthernetServer server(80);


// The pin the output of the motion sensor is attached to
//  HIGH => motion
//  LOW => no motion
const int MOTION_SENSOR_PIN = 2;

void setup() {
  pinMode(MOTION_SENSOR_PIN, INPUT); 

  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

boolean sawMotionLastTime = false;
//String kAreYouStillThereUrl = "http://www.portal2sounds.com/sound.php?id=2640&stream";
//String kISeeYouUrl = "http://p1.portal2sounds.com/sound.php?id=303&stream";

String neg1 = "308"; //Is anyone there?
String neg2 = "347"; //Are you still there?
String neg3 = "280"; //Are you coming back?
String whereAreYou[] = {neg1, neg2, neg3};
int whereAreYouIndex = 0;

String pos1 = "303"; //I see you
String pos2 = "336"; //Hey
String pos3 = "300"; //Hello? Friend.
String pos4 = "331"; //Hey hey hey
String pos5 = "319"; //There you are
String pos6 = "330"; //Hey!
String pos7 = "304"; //Hello!

String seeingYou[] = {pos1, pos2, pos3, pos4, pos5, pos6, pos7};
int seeingYouIndex = 0;

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<meta http-equiv=\"refresh\" content=\"2\">");
          client.println("<div id='sounds'></div>");
          int sensorBit = digitalRead(2);
          if (sensorBit == HIGH) {
            client.println("I see you!<br/>");
            if (!sawMotionLastTime) {
              // New motion
              client.print("<script>document.getElementById('sounds').innerHTML = \"<embed src='http://p1.portal2sounds.com/sound.php?id=");
              client.print(seeingYou[seeingYouIndex++%7]);
              client.println("&stream'>\";</script>");
              sawMotionLastTime = true;
            }
          } else {
            client.println("Where are you hiding?<br/>");
            if (sawMotionLastTime) {
              client.print("<script>document.getElementById('sounds').innerHTML = \"<embed src='http://p1.portal2sounds.com/sound.php?id=");
              client.print(whereAreYou[whereAreYouIndex++%3]);
              client.println("&stream'>\";</script>");
              sawMotionLastTime = false;
            }
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

