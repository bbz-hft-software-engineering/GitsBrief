#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WLAN Konfiguration
const char* ssid = "Mirhendkeiwlan";
const char* password = "Einsbisacht";

// SendGrid API Key
const char* apiKey = "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";

// SendGrid API URL
const char* sendgridUrl = "https://api.sendgrid.com/v3/mail/send";

// Absender-Email
const char* senderEmail = "laura.ehrat@bbz-sh.ch";

// Empfänger-Emails
const char* lauraEmail = "laura.ehrat@gmail.com";
const char* lisaEmail = "lisa.lott@sunrise.ch";
const char* matthiasEmail = "steinemann.matthias@gmail.com";

// Sensor-Pins
const int lauraPin = D7;
const int lisaPin = D5;
const int matthiasPin = D3;

// Variable zum Verfolgen des Sensorstatus
bool lauraEmailSent = false;
bool lisaEmailSent = false;
bool matthiasEmailSent = false;

bool lauraSensorWasHigh = false;
bool lisaSensorWasHigh = false;
bool matthiasSensorWasHigh = false;

void sendEmail(const char* recipientEmail, const char* subject, const char* body) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(sendgridUrl);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", String("Bearer ") + apiKey);

        // JSON-Daten für die SendGrid API
        StaticJsonDocument<1024> doc;
        doc["personalizations"][0]["to"][0]["email"] = recipientEmail;
        doc["from"]["email"] = senderEmail;
        doc["subject"] = subject;
        doc["content"][0]["type"] = "text/plain";
        doc["content"][0]["value"] = body;

        String requestBody;
        serializeJson(doc, requestBody);

        Serial.print("Sending email to ");
        Serial.print(recipientEmail);
        Serial.print(" with subject '");
        Serial.print(subject);
        Serial.println("'...");

        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode == 202) { // 202 Accepted
            Serial.println("E-Mail erfolgreich gesendet");
        } else {
            String response = http.getString();
            Serial.print("Fehler beim Senden der E-Mail: ");
            Serial.println(httpResponseCode);
            Serial.println(response);
        }

        http.end();
    } else {
        Serial.println("Nicht mit WLAN verbunden");
    }
}

void setup() {
    // Serielle Kommunikation starten
    Serial.begin(9600);

    // Debug-Ausgabe starten
    Serial.println("Starte Setup...");

    // WLAN verbinden
    WiFi.begin(ssid, password);
    Serial.print("Verbinde mit WLAN: ");
    Serial.println(ssid);

    int maxAttempts = 20; // Anzahl der Versuche zum Verbinden
    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nVerbunden mit WLAN");
        Serial.print("IP-Adresse: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFehler: WLAN-Verbindung konnte nicht hergestellt werden.");
    }

    // Sensor-Pins als Input konfigurieren
    pinMode(lauraPin, INPUT);
    pinMode(lisaPin, INPUT);
    pinMode(matthiasPin, INPUT);
    Serial.println("Sensor-Pins konfiguriert");

    Serial.println("Setup abgeschlossen");

    // Tests für die E-Mail-Funktion
    Serial.println("Starte Tests...");

    // Test 1: E-Mail an Laura
    Serial.println("Test 1: Sende E-Mail an Laura");
    sendEmail(lauraEmail, "Test 1", "Dies ist ein Test.");
    Serial.println("E-Mail an Laura gesendet");

    // Test 2: E-Mail an Lisa
    Serial.println("Test 2: Sende E-Mail an Lisa");
    sendEmail(lisaEmail, "Test 2", "Dies ist ein Test.");
    Serial.println("E-Mail an Lisa gesendet");

    // Test 3: E-Mail an Matthias
    Serial.println("Test 3: Sende E-Mail an Matthias");
    sendEmail(matthiasEmail, "Test 3", "Dies ist ein Test.");
    Serial.println("E-Mail an Matthias gesendet");

    Serial.println("Tests abgeschlossen");
}

void loop() {
    // Sensorwerte lesen
    int lauraSensorValue = digitalRead(lauraPin);
    int lisaSensorValue = digitalRead(lisaPin);
    int matthiasSensorValue = digitalRead(matthiasPin);

    // Sensorstatus in der Konsole ausgeben
    Serial.print("Laura Sensorwert: ");
    Serial.println(lauraSensorValue);
    Serial.print("Lisa Sensorwert: ");
    Serial.println(lisaSensorValue);
    Serial.print("Matthias Sensorwert: ");
    Serial.println(matthiasSensorValue);

    // Überprüfen, ob Lauras Sensor belegt war und jetzt frei ist, und ob die E-Mail bereits gesendet wurde
    if (lauraSensorValue == LOW && lauraSensorWasHigh && !lauraEmailSent) {
        Serial.println("Laura hat Post, sende E-Mail...");
        sendEmail(lauraEmail, "Laura hat Post", "Hallo Laura\n\nDu hast Post erhalten. Vergiss nicht diese zu leeren. Falls du länger nicht da bist, überlege dir ob diese durch deine Mitbewohner kontrolliert werden sollte.");
        lauraEmailSent = true;
        lauraSensorWasHigh = false;
    }

    // Überprüfen, ob Lauras Sensor belegt ist
    if (lauraSensorValue == HIGH) {
        lauraSensorWasHigh = true;
        lauraEmailSent = false;
    }

    // Überprüfen, ob Lisas Sensor belegt war und jetzt frei ist, und ob die E-Mail bereits gesendet wurde
    if (lisaSensorValue == LOW && lisaSensorWasHigh && !lisaEmailSent) {
        Serial.println("Lisa hat Post, sende E-Mail...");
        sendEmail(lisaEmail, "Lisa hat Post", "Hallo Lisa\n\nDu hast Post erhalten. Vergiss nicht diese zu leeren. Falls du länger nicht da bist, überlege dir ob diese durch deine Mitbewohner kontrolliert werden sollte.");
        lisaEmailSent = true;
        lisaSensorWasHigh = false;
    }

    // Überprüfen, ob Lisas Sensor belegt ist
    if (lisaSensorValue == HIGH) {
        lisaSensorWasHigh = true;
        lisaEmailSent = false;
    }

    // Überprüfen, ob Matthias' Sensor belegt war und jetzt frei ist, und ob die E-Mail bereits gesendet wurde
    if (matthiasSensorValue == LOW && matthiasSensorWasHigh && !matthiasEmailSent) {
        Serial.println("Matthias hat Post, sende E-Mail...");
        sendEmail(matthiasEmail, "Matthias hat Post", "Hallo Matthias\n\nDu hast Post erhalten. Vergiss nicht diese zu leeren. Falls du länger nicht da bist, überlege dir ob diese durch deine Mitbewohner kontrolliert werden sollte.");
        matthiasEmailSent = true;
        matthiasSensorWasHigh = false;
    }

    // Überprüfen, ob Matthias' Sensor belegt ist
    if (matthiasSensorValue == HIGH) {
        matthiasSensorWasHigh = true;
        matthiasEmailSent = false;
    }

    delay(1000); // Lange Pause, um die Debug-Ausgaben zu verlangsamen
}
