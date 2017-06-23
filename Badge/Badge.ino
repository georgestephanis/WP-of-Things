/**
 * This example code runs a loop of 16 Neopixels connected
 * to an ESP8266 chip.  It was written for my badge, but it
 * can just as easily run a Neopixel Ring attached to a
 * Wemos D1 Mini.
 * 
 * The endpoint queries is provided by the "Hugh" plugin by
 * Michael Arestad and George Stephanis.
 * 
 * https://wordpress.org/plugins/hugh/
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>

// Initialize our Neopixel Ring to have 16 lights, connected on pin D5.
Adafruit_NeoPixel neoring = Adafruit_NeoPixel( 16, D5 );

// Network details and some global variables to reuse.
String url           = "http://wp-of-things.pw/wp-json/hugh/v1/colors/?limit=1";

String prevColor; // Store this as a global so `loop()` can remember what the current color is.

void setup() {
  // Initialize our Neopixel Ring
  neoring.begin();
  neoring.setBrightness( 30 );
  neoring.show();

  // Connect to WiFi
  WiFi.begin( wifissid, wifipass );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay( 250 );
  }
}

void loop() {
  // Declare our HTTP Client and point it at our endpoint.
  HTTPClient http;

  http.begin( url );
  http.setUserAgent( "George Stephanis | WordCamp Kyoto 2017 | WordPress of Things" );

  // If it wasn't a good request, try again later.
  if ( HTTP_CODE_OK != http.GET() ) {
    delay( 5000 );
    return;
  }

  // Get the response data, and instead of parsing the JSON, just extract the color.
  // Example response data:
  // [{"color":"#b70000","label":"","time":1497797277}]

  String payload = http.getString();
  int start = payload.indexOf( "\"color\":\"" ) + 9;
  String color = payload.substring( start );
  color = color.substring( 0, color.indexOf( '"' ) );

  // If the color has changed since we last did something, update it!
  if ( color != prevColor ) {
    prevColor = color;

    // Split the string representation of the color into red, green, blue values.
    long long number = strtol( &color[1], NULL, 16 );
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

    // Recreate the color in the expected format
    uint32_t newColor = neoring.Color( r, g, b );

    // Iterate through all the Neopixels, lighting up one at a time.
    for ( int i = 0; i < neoring.numPixels(); i++ ) {
      neoring.setPixelColor( i, newColor );
      neoring.show();
      delay( 20 );
    }
  }

  // Take a break for a second.
  delay( 1000 );
}

