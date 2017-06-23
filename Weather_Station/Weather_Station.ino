/**
 * This example is designed to work with the Weather Report Widget
 * by George Stephanis.  Code is available at the following URL:
 *
 * https://gist.github.com/georgestephanis/0f3d2f8c8559ce7d883d632569a8f738
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Initialize our connection to the sensor...
DHT dht( D4, DHT11, 11 );

// Network details and credentials and such.
const char* wifissid = "";
const char* wifipass = "";
const char* wpuser   = "";
const char* wppass   = "";
const char* uuid     = ""; // the uuid for the weather station as given by the plugin
String url           = "https://wp-of-things.pw/wp-json/wordpress-of-things/v1/weather-station/" + String( uuid );
// You can get the domain's fingerprint here: https://www.grc.com/fingerprints.htm
String fingerprint   = "ED:00:E3:79:18:1F:EB:11:13:F5:D0:7F:77:EA:C1:9A:49:16:13:9A";

void setup() {
  // Begin a Serial interface for logging and debugging over UART.
  Serial.begin( 74880 );

  Serial.println();
  Serial.println();
  Serial.printf( "Connecting to %s ", wifissid );

  // Connect to WiFi
  WiFi.begin( wifissid, wifipass );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay( 250 );
    Serial.print( "." );
  }

  Serial.println();
  Serial.println( "WiFi connected" );
  Serial.print( "IP address: " );
  Serial.println( WiFi.localIP() );
}

void loop() {
  // Read the current humidity and temperature.
  float humidity = dht.readHumidity();
  float temp_f   = dht.readTemperature( true ); // true gives us farenheit, false will give us celsius.

  // If something went wrong, which may happen occasionally, wait a bit and try again.
  if ( isnan( humidity ) || isnan( temp_f ) ) {
    Serial.println( "Failed to read from DHT sensor!" );
    delay( 500 );
    return;
  }

  // Track the current Temperature and Humidity so we can see them in a terminal.
  Serial.print( "Temp: " + String( temp_f ) + ", ");
  Serial.println( "Humidity: " + String( humidity ) + "%" );

  // We got the data, now let's send it out!
  HTTPClient http;

  http.begin( url + "?temperature=" + String( temp_f ) + "&humidity=" + String( humidity ), fingerprint );
  http.setAuthorization( wpuser, wppass );
  http.setUserAgent( "George Stephanis | WordCamp Kyoto 2017 | WordPress of Things" );

  int httpCode = http.POST("");

  // If the request didn't return a 200 OK, log what went wrong!
  if ( httpCode != HTTP_CODE_OK ) {
    Serial.printf( "[HTTP] POST... failed, error %d: %s\n", httpCode, http.errorToString( httpCode ).c_str() );
    Serial.println( http.getString() );
  }

  // Take a break for five seconds.
  delay( 5000 );
}

