


#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
 pinMode(2, INPUT); 

 // Open serial communications and wait for port to open:
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

boolean seenDirty = false;
boolean notSeenDirty = true;
char areYouStillThere[] = "http://www.portal2sounds.com/sound.php?id=2640&stream";

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
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println("X-Frame-Options: GOFORIT");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"2\">");
//          client.println("<meta http-equiv=\"X-Frame-Options\" content=\"GOFORIT\">");
          // output the value of each analog input pin
//          client.println("<iframe id='sound' src='http://www.google.com'></iframe>");
          client.println("<div id='sounds'></div>");
          client.println("Hi Beba!<br/>");
          int sensorBit = digitalRead(2);
          if (sensorBit == HIGH) {
            client.println("I see you!<br/>");
            seenDirty = true;
            if (notSeenDirty) {
              client.print("<script>document.getElementById('sounds').innerHTML = \"");
              client.print("<embed src='http://p1.portal2sounds.com/sound.php?id=303&stream'>");
              client.println("\";</script>");
              notSeenDirty = false;
            }
          } else {
            notSeenDirty = true;
            if (seenDirty) {
//               client.println("<script>document.getElementById('sound').src = '");
//               client.println(areYouStillThere);
//               client.println("';</script>");
               client.print("<script>document.getElementById('sounds').innerHTML = \"");
               client.print("<embed src='http://www.portal2sounds.com/sound.php?id=2640&amp;stream'>");
               client.println("\";</script>");
               seenDirty = false;
            }
            client.println("Where are you hiding?<br/>");
          }
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
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

