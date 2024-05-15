
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 * Copyright (c) 2023 mobizt
 *
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
#include <Deneyap_HareketAlgilama.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Eren YILDIRIM"
#define WIFI_PASSWORD "samsunsporellibes"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCasq-_Dckwyc9x9a_DrvXUw8Pgyseyeiw"

/* 3. Define the RTDB URL */
#define DATABASE_URL "thief-alarm-deneyap-default-rtdb.europe-west1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "eren@gmail.com"
#define USER_PASSWORD "testtesttest"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
WiFiMulti multi;
#endif
const char* ssid = "Eren YILDIRIM";
const char* password = "samsunsporellibes";

// Telegram BOT bilgileri.
#define BOTtoken "7119578524:AAF7sJPmjsbjJcS3iI-8Ei6-JExDG3BtfCA"
#define CHAT_ID "1334967091"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
Gesture Hareket;
bool hareketAlgilandi = false;
void setup()
{
 Serial.begin(115200);
  client.setInsecure();
  
  if (!Hareket.begin(0x32)) {
    Serial.println("I2C bağlantısı başarısız ");
    while (1);
  }
  

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

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Or use legacy authenticate method
  // config.database_url = DATABASE_URL;
  // config.signer.tokens.legacy_token = "<database secret>";

  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // The WiFi credentials are required for Pico W
  // due to it does not have reconnect feature.
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  config.wifi.clearAP();
  config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;

  // You can use TCP KeepAlive in FirebaseData object and tracking the server connection status, please read this for detail.
  // https://github.com/mobizt/Firebase-ESP-Client#about-firebasedata-object
  // fbdo.keepAlive(5, 5, 1);

  /** Timeout options.

  //Network reconnect timeout (interval) in ms (10 sec - 5 min) when network or WiFi disconnected.
  config.timeout.networkReconnect = 10 * 1000;

  //Socket connection and SSL handshake timeout in ms (1 sec - 1 min).
  config.timeout.socketConnection = 10 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  Note:
  The function that starting the new TCP session i.e. first time server connection or previous session was closed, the function won't exit until the
  time of config.timeout.socketConnection.

  You can also set the TCP data sending retry with
  config.tcp_data_sending_retry = 1;

  */
  bot.sendMessage(CHAT_ID, "Cihaz aktif oldu.", "");
}

void loop()
{
bool gestureDurum = Hareket.readGesture();
  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (gestureDurum == 1 && !hareketAlgilandi)
  {
    Serial.println("HAREKET ALGILANDI");
    bot.sendMessage(CHAT_ID, "Hareket algılandı ⚠", "");
    hareketAlgilandi = true;

    Serial.printf("Set bool... %s\n", Firebase.RTDB.setBool(&fbdo, F("/alarm/bool"), true) ? "ok" : fbdo.errorReason().c_str());

  }
   else if (gestureDurum == 0 && hareketAlgilandi) {
    Serial.printf("Set bool... %s\n", Firebase.RTDB.setBool(&fbdo, F("/alarm/bool"), false) ? "ok" : fbdo.errorReason().c_str());
    hareketAlgilandi = false;
  }

  // Eğer hareket algılanmamışsa ve daha önce hareket algılandıysa, tekrar algılama yapılabilsin diye hareket algılandı değişkenini sıfırla
  if (gestureDurum == 0 && hareketAlgilandi) {
    hareketAlgilandi = false;
  }
  }