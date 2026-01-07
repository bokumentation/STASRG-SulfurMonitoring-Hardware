#include "esp32-hal.h"
#include <Arduino.h>
#include "SoftwareSerial.h"

// #define RX2 21 // pin RX2 [PUTIH TX]
// #define TX2 20 // pin TX2 [KUNING RX]

String data, arah_angin, s_angin;
int a, b;

#define BAUD_RATE 9600
SoftwareSerial SerialSW(21, 20);

void setup() {
  // Serial1.begin(9600, SERIAL_8N1, RX2, TX2);
  SerialSW.begin(BAUD_RATE);
  // .begin(BAUD_RATE);
  printf("hello world \n");
}

void loop() {
  printf("Di dalam fungsi loops \n");
  printf("Jika serial1.available: \n");

  if (SerialSW.available()) // Jika ada data yang diterima dari sensor
  {
    data = SerialSW.readString(); // data yang diterima dari sensor berawalan tanda * dan diakhiri tanda #, contoh *1#
    a = data.indexOf("*"); // a adalah index tanda *
    b = data.indexOf("#"); // b adalah index tanda #
    s_angin = data.substring(a + 1, b); // membuang tanda * dan # sehingga di dapat nilai dari arah angin
    // arah_angin = "Inisiasi awal";

    if (s_angin.equals("1")) { // jika nilai dari sensor 1 maka arah angin utara
      arah_angin = "utara     ";
    }
    if (s_angin.equals("2")) {
      arah_angin = "timur laut";
    }
    if (s_angin.equals("3")) {
      arah_angin = "timur     ";
    }
    if (s_angin.equals("4")) {
      arah_angin = "tenggara  ";
    }
    if (s_angin.equals("5")) {
      arah_angin = "selatan   ";
    }
    if (s_angin.equals("6")) {
      arah_angin = "barat daya";
    }
    if (s_angin.equals("7")) {
      arah_angin = "barat     ";
    }
    if (s_angin.equals("8")) {
      arah_angin = "barat laut";
    }

    printf("UART: %s \n", s_angin.c_str());
    printf("Arah angin: %s \n", arah_angin.c_str());
  }
  delay(1000);
}
