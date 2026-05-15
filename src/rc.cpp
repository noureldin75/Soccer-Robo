#include "config.h"
#include "motors.h"
#include "rc.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// --- WiFi AP credentials ---
// phone connects to this hotspot directly, no router needed
static const char* AP_SSID = "SoccerRobot";
static const char* AP_PASS = "12345678";   // min 8 chars for WPA2

static WebServer server(80);

// last command, held until a new one arrives
static char current_cmd = 'S';

// ---------- route handlers ----------

// ESPiX sends GET requests like:  /cmd?var=F
// We also handle the older style:  /F  (direct path)
static void handle_cmd(char cmd) {
    if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R' || cmd == 'S') {
        if (cmd != current_cmd) {
            Serial.printf("[RC] cmd: %c\n", cmd);
            current_cmd = cmd;
        }
    } else if (cmd == 'H') {
        Serial.println("[RC] HORN");
    } else if (cmd == 'G') {
        Serial.println("[RC] LIGHT");
    }
    server.send(200, "text/plain", "OK");
}

// ESPiX Robot Control app hits /cmd?var=X
static void on_cmd() {
    if (server.hasArg("var")) {
        String val = server.arg("var");
        if (val.length() > 0) handle_cmd((char)val[0]);
    } else {
        server.send(400, "text/plain", "missing var");
    }
}

// Some ESPiX versions hit the command as the path directly: /F /B /L /R /S
static void on_forward()  { handle_cmd('F'); }
static void on_backward() { handle_cmd('B'); }
static void on_left()     { handle_cmd('L'); }
static void on_right()    { handle_cmd('R'); }
static void on_stop()     { handle_cmd('S'); }
static void on_horn()     { handle_cmd('H'); }
static void on_light()    { handle_cmd('G'); }

// catch-all so the app doesn't get a 404 and give up
static void on_not_found() {
    String uri = server.uri();
    Serial.printf("[RC] unknown path: %s\n", uri.c_str());

    // try to pull a command out of the path itself (e.g. "/F")
    if (uri.length() == 2) {
        handle_cmd((char)uri[1]);
        return;
    }
    server.send(200, "text/plain", "OK");
}

// ---------- setup / loop ----------

void setup_rc() {
    Serial.println("[RC] starting WiFi AP...");
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.printf("[RC] AP ready — connect your phone to '%s'\n", AP_SSID);
    Serial.printf("[RC] robot IP: %s\n", WiFi.softAPIP().toString().c_str());

    // register routes
    server.on("/cmd",      HTTP_GET, on_cmd);
    server.on("/F",        HTTP_GET, on_forward);
    server.on("/B",        HTTP_GET, on_backward);
    server.on("/L",        HTTP_GET, on_left);
    server.on("/R",        HTTP_GET, on_right);
    server.on("/S",        HTTP_GET, on_stop);
    server.on("/H",        HTTP_GET, on_horn);
    server.on("/G",        HTTP_GET, on_light);
    server.onNotFound(on_not_found);

    server.begin();
    Serial.println("[RC] web server started on port 80");
}

void process_rc() {
    server.handleClient();   // must run every loop to handle HTTP requests

    // convert current command to motor targets
    int left  = 0;
    int right = 0;

    switch (current_cmd) {
        case 'F':                        // forward
            left  =  MAX_PWM;
            right =  MAX_PWM;
            break;
        case 'B':                        // backward
            left  = -MAX_PWM;
            right = -MAX_PWM;
            break;
        case 'L':                        // pivot left
            left  = -MAX_PWM;
            right =  MAX_PWM;
            break;
        case 'R':                        // pivot right
            left  =  MAX_PWM;
            right = -MAX_PWM;
            break;
        case 'S':                        // stop
        default:
            left  = 0;
            right = 0;
            break;
    }

    drive_robot(left, right);
}