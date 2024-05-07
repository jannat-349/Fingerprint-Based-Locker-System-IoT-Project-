#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "taplock-b48b0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "S9SrV7PtVhy895q7Yf8tdpcR1d0qEhppupWOopwA"
#define WIFI_SSID "CSE LAB 201"
#define WIFI_PASSWORD "cseju_std7"

#define RXp2 16
#define TXp2 17

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, SERIAL_FULL, TXp2);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // connect to Firebase
}

void loop()
{
  if (Serial1.available() >= 2)
  {
    String bookingNumber = Serial1.readStringUntil('\n'); // Read until newline character
    String payAmount = Serial1.readStringUntil('\n');     // Read until newline character

    if (bookingNumber.length() == 0 || payAmount.length() == 0)
    {
      return; // Exit loop if booking number or pay amount is zero
    }

    // Push data to Firebase
    Firebase.pushInt("/booking_number", bookingNumber.toInt());
    Firebase.pushInt("/amount", payAmount.toInt());

    // Check for Firebase errors
    if (Firebase.failed())
    {
      Serial.print("Firebase error: ");
      Serial.println(Firebase.error());
    }

    // Retrieve status from Firebase
    bool status = Firebase.getBool("/status");

    // Check for Firebase errors when retrieving status
    if (Firebase.failed())
    {
      Serial.print("Firebase error: ");
      Serial.println(Firebase.error());
    }
    else
    {
      // Send status to Arduino
      Serial1.println(status ? "true" : "false");
    }

    delay(1000); // Adjust delay as needed
  }
}
