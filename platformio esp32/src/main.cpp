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
#define API_KEY "AIzaSyAy-h4OBQjRfZBo78gI4WEOvIjAtVlRGm8"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "bootcamp-iot-3f0d2"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "ouba.bnina@gmail.com"
#define USER_PASSWORD "123456"
#define APP_MAIN_FIRESTORE_COLLECTION_ID         "Window"
#define APP_MAIN_FIRESTORE_DOCUMENT_ID           "WDirection"

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

void setup() {
  // put your setup code here, to run once:
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