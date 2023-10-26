#include <Arduino.h>

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Ooredoo 4G_39D94B"
#define WIFI_PASSWORD "18201033"

/* 2. Define the API Key */
#define API_KEY ""

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID ""

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL ""
#define USER_PASSWORD ""
#define APP_MAIN_FIRESTORE_COLLECTION_ID         ""
#define APP_MAIN_FIRESTORE_DOCUMENT_ID           ""

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
int relay1 = 4;
int relay2 = 18;
int pir = 2;
bool taskCompleted = false;

unsigned long dataMillis = 0;
String Up_status = "false";
String Down_status = "false";
int  Motion = 0;
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif
\
WebServer server(80);
const char* host = "esp32";
const char* loginIndex = 
  "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
      "<tr>"
        "<td colspan=2>"
          "<center><font size=4><b>ESP32 Login Page</b></font></center>"
          "<br>"
        "</td>"
        "<br>"
        "<br>"
      "</tr>"
      "<td>Username:</td>"
      "<td><input type='text' size=25 name='userid'><br></td>"
      "</tr>"
      "<br>"
      "<br>"
      "<tr>"
        "<td>Password:</td>"
        "<td><input type='Password' size=25 name='pwd'><br></td>"
        "<br>"
        "<br>"
      "</tr>"
      "<tr>"
        "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
      "</tr>"
    "</table>"
  "</form>"
  "<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
  "</script>";


const char* espInfoPage = 
  "<div id='prg'>progress: 0%%</div>"
  "<h1>ESP Info</h1>"
  "<p>Chip Model: %s</p>"
  "<p>SDK Version: %s</p>"
  "<p>CPU Frequency (MHz): %d</p>"
  "<p>Flash Chip Size (MB): %d</p>";


const char* serverIndex = 
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
      "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
        "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
      "</form>"
      "<form method='GET' action='/espInfoPage'>"
  "<input type='submit' value='Info'>"
  "</form>"
      "<script>"
        "$('form').submit(function(e){"
        "e.preventDefault();"
        "var form = $('#upload_form')[0];"
        "var data = new FormData(form);"
        " $.ajax({"
        "url: '/update',"
        "type: 'POST',"
        "data: data,"
        "contentType: false,"
        "processData:false,"
        "xhr: function() {"
        "var xhr = new window.XMLHttpRequest();"
        "xhr.upload.addEventListener('progress', function(evt) {"
        "if (evt.lengthComputable) {"
        "var per = evt.loaded / evt.total;"
        "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
        "}"
        "}, false);"
        "return xhr;"
        "},"
        "success:function(d, s) {"
        "console.log('success!')" 
        "},"
        "error: function (a, b, c) {"
        "}"
        "});"
        "});"
  "</script>";

void setup() {
Serial.begin(115200);
pinMode(relay1, OUTPUT);
pinMode(relay2, OUTPUT);
pinMode(pir, INPUT);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    multi.addAP(WIFI_SSID, WIFI_PASSWORD);
    multi.run();
#else
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
        if (millis() - ms > 10000)
            break;
#endif
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

  if (!MDNS.begin(host)) { // http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  char* serverIndexT[1024];
/*sprintf(serverIndexT, serverIndex,
  BOARD_TYPE, // Include the board type here
  ESP.getSdkVersion(),
  ESP.getCpuFreqMHz(),
  ESP.getFlashChipSize() / (1024 * 1024)// Include the board type here
);*/
  /* Return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);//401 inexistant
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/espInfoPage", HTTP_GET, []() {
  String espInfoPage = "<h1>ESP Info</h1>";
  espInfoPage += "<p>Chip Model: " + String(BOARD_TYPE) + "</p>";
  espInfoPage += "<p>SDK Version: " + String(ESP.getSdkVersion()) + "</p>";
  espInfoPage += "<p>CPU Frequency (MHz): " + String(ESP.getCpuFreqMHz()) + "</p>";
  espInfoPage += "<p>Flash Chip Size (MB): " + String(ESP.getFlashChipSize() / (1024 * 1024)) + "</p>";
  server.send(200, "text/html", espInfoPage);
});

  /* Handling uploading firmware file */
  server.on("/update", HTTP_POST,[]() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  },[]() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { // Start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* Flashing firmware to ESP */
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { // True to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  server.begin();
  // put your setup code here, to run once:

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // The WiFi credentials are required for Pico W
    // due to it does not have reconnect feature.
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    config.wifi.clearAP();
    config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

#if defined(ESP8266)
    // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
    fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    // You can use TCP KeepAlive in FirebaseData object and tracking the server connection status, please read this for detail.
    // https://github.com/mobizt/Firebase-ESP-Client#about-firebasedata-object
    // fbdo.keepAlive(5, 5, 1);
}


void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
   if (Firebase.ready() && (millis() - dataMillis > 20 || dataMillis == 0))
    {

        dataMillis = millis();
        String documentPath = "Window/WDirection";
        String documentPath2 = "PIR/MotionD";
        String mask1 = "Up";
        String mask2 = "Down";
        FirebaseJson content;
                //Set the document content to write (transform)
        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        Serial.print("Get a document... ");

        if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str())){
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
             // Create a FirebaseJson object and set content with received payload
            FirebaseJson payload;
            payload.setJsonData(fbdo.payload().c_str());        
            // Get the data from FirebaseJson object
            FirebaseJsonData jsonData1, jsonData2, jsonData3;
            payload.get(jsonData1, "fields/Up/booleanValue", true);
            //Serial.println(jsonData.stringValue);
            Up_status=jsonData1.stringValue;
            payload.get(jsonData2, "fields/Down/booleanValue", true);
            Down_status=jsonData2.stringValue;
            Serial.println(Up_status);
            Serial.println(Down_status);
            Motion = digitalRead(pir);
            Serial.println(Motion);
            if (Motion==1)
            {
                content.set("fields/IsSomeoneHere/booleanValue", 1==1);
                if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "IsSomeoneHere" /* updateMask */))

                {Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());}
                else
                Serial.println(fbdo.errorReason());
                delay(1000);
            }
            else
                {content.set("fields/IsSomeoneHere/booleanValue", 1==0);
                if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "IsSomeoneHere" /* updateMask */))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        delay(1000);
            }
           if(Up_status == "true")
           {
            digitalWrite(relay1, HIGH);
            digitalWrite(relay2, LOW);
            delay(20000);
            content.set("fields/Up/booleanValue", 1==0);
            digitalWrite(relay2, HIGH);
            if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "Up" /* updateMask */))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
    }

        //Set the write object write operation type.
        //fb_esp_firestore_document_write_type_update,
        //fb_esp_firestore_document_write_type_transform
        

       

        //Set the document mask field paths that will be updated
        //Use comma to separate between the field paths
       
            else{
              if (Down_status == "true")
              {
                digitalWrite(relay2, HIGH);
                digitalWrite(relay1, LOW);
                delay(20000);
                digitalWrite(relay1, HIGH);
                content.set("fields/Down/booleanValue", 1==0);
                if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "Down" /* updateMask */))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
    }
              }
            }
        //Set the update document content
 
}
}
