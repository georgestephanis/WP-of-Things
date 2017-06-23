/**
 * This example contacts a WooCommerce site and finds out how many
 * orders have come in, that have yet to ship, and then displays
 * that on a ring of Neopixels.
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>

// Initialize our Neopixel Ring to have 16 lights, connected on pin D5.
Adafruit_NeoPixel neoring = Adafruit_NeoPixel( 16, D5 );

// Network details and credentials and such.
const char* wifissid = "";
const char* wifipass = "";
const char* wpuser   = "";
const char* wppass   = "";
String url           = "https://wp-of-things.pw/wp-json/wc/v2/orders/?status=processing";
// You can get the domain's fingerprint here: https://www.grc.com/fingerprints.htm
String fingerprint   = "ED:00:E3:79:18:1F:EB:11:13:F5:D0:7F:77:EA:C1:9A:49:16:13:9A";

void setup() {
  // Initialize our Neopixel Ring
  neoring.begin();
  neoring.setBrightness( 50 );
  setAllPixelsToColor( 0xFF, 0x88, 0 ); // orange

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
  // Declare our HTTP Client and point it at our endpoint.
  HTTPClient http;

  http.begin( url, fingerprint );
  http.setAuthorization( wpuser, wppass );
  http.setUserAgent( "George Stephanis | WordCamp Kyoto 2017 | WordPress of Things" );

  // Collect these response headers:
  const char* headerKeys[] = { "X-WP-Total" };
  size_t headerKeysSize = sizeof( headerKeys ) / sizeof( char* );
  http.collectHeaders( headerKeys, headerKeysSize );

  int httpCode = http.GET();

  // If the request didn't return a 200 OK, log what went wrong!
  if ( httpCode != HTTP_CODE_OK ) {
    Serial.printf( "[HTTP] GET... failed, error %d: %s\n", httpCode, http.errorToString( httpCode ).c_str() );
    Serial.println( http.getString() );
    setAllPixelsToColor( 0xFF, 0, 0 );
    delay( 5000 );
    return;
  }

  // The number of orders is returned in the X-WP-Total header. Let's check that.
  String qty = http.header( headerKeys[0] );

  // Report in how many we've got.
  Serial.println( "Found " + qty + " order(s)." );

  // If we have more orders than we have pixels to light up, blink to indicate that!
  if ( qty.toInt() > neoring.numPixels() ) {
    for ( int i = 0; i < 10; i++ ) {
      setAllPixelsToColor( 0, 0, 0 );
      delay( 1000 );
      setAllPixelsToColor( 0, 0xFF, 0 );
      delay( 1000 );
    }
    return;
  }

  // Otherwise, just light up however many we do have.
  setXPixelsToColor( qty.toInt(), 0, 0xFF, 0 );

  delay( 5000 );
}

// Set a specified number of Neopixels to a specific rgb color. Turn all others off.
void setXPixelsToColor( int x, int r, int g, int b ) {
  uint32_t color = neoring.Color( r, g, b );
  uint32_t black = neoring.Color( 0, 0, 0 );

  // Build the settings for all the Neopixels.
  for ( int i = 0; i < neoring.numPixels(); i++ ) {
    neoring.setPixelColor( i, ( i < x ) ? color : black );
  }
  // Send the signal!  Light them up!
  neoring.show();
}

// Similar, but this just sets every single pixel to the specified color.
void setAllPixelsToColor( int r, int g, int b ) {
  setXPixelsToColor( neoring.numPixels(), r, g, b );
}

