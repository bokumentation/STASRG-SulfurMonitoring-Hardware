// wind_direction.cpp
#include "wind_direction.h"

/*
IMPLEMENTASI SENSOR ARAH ANGIN MENGGUNAKAN WIND DIRECTION SENSOR UART
STATUS: NOT TESTED

Latest Update: 2026-01-09

DESKRIPSI:
Cara kerja:
1. Inisialisasi komunikasi serial software pada pin RX=12, TX=33 dengan baudrate 9600
2. Pada fungsi loop, baca data dari serial software
3. Data yang diterima berbentuk string dengan format *x# dimana x adalah nilai arah angin (1-8)
4. Konversi nilai x menjadi arah angin dalam bentuk string (utara, timur laut, timur, tenggara, selatan, barat daya,
barat, barat laut)
5. Tampilkan hasil pembacaan arah angin ke serial monitor setiap 1 detik
*/

// --- SYSTEM INCLUDE ----
#include "SoftwareSerial.h"
#include "esp32-hal.h"
#include <Arduino.h>

// --- USER INCLUDE ----
#include "board_pins.h"

// String data, arah_angin, s_angin;
struct data_wind_direction_t {
    String arah_angin, data, s_angin;
    int temp_a, temp_b;
};

SoftwareSerial my_Windir_SWSerial(PIN_SENSOR_WIND_DIR_RX, PIN_SENSOR_WIND_DIR_TX); // RX=12, TX=33
data_wind_direction_t wind_dir_data;

void inisialisasiWinDir()
{
    my_Windir_SWSerial.begin(BAUD_RATE);
    printf("Inisialisasi Wind Direction Sensor\n");

    while (1) {
        if (my_Windir_SWSerial.available()) {
            wind_dir_data.data = my_Windir_SWSerial.readString();

            // a adalah index tanda * / Mencari posisi char *
            wind_dir_data.temp_a = wind_dir_data.data.indexOf("*");

            // b adalah index tanda # / Mencari posisi char #
            wind_dir_data.temp_b = wind_dir_data.data.indexOf("#");

            // membuang tanda * dan # sehingga di dapat nilai dari arah angin
            // kata lain: mengambil substring antara posisi a+1 dan b. Misal data=*3#, maka diambil karakter ke 1 yaitu 3.
            wind_dir_data.s_angin = wind_dir_data.data.substring(wind_dir_data.temp_a + 1, wind_dir_data.temp_b);

            if (wind_dir_data.s_angin.equals("1")) { // jika nilai dari sensor 1 maka arah angin utara
                wind_dir_data.arah_angin = "utara     ";
            }
            if (wind_dir_data.s_angin.equals("2")) {
                wind_dir_data.arah_angin = "timur laut";
            }
            if (wind_dir_data.s_angin.equals("3")) {
                wind_dir_data.arah_angin = "timur     ";
            }
            if (wind_dir_data.s_angin.equals("4")) {
                wind_dir_data.arah_angin = "tenggara  ";
            }
            if (wind_dir_data.s_angin.equals("5")) {
                wind_dir_data.arah_angin = "selatan   ";
            }
            if (wind_dir_data.s_angin.equals("6")) {
                wind_dir_data.arah_angin = "barat daya";
            }
            if (wind_dir_data.s_angin.equals("7")) {
                wind_dir_data.arah_angin = "barat     ";
            }
            if (wind_dir_data.s_angin.equals("8")) {
                wind_dir_data.arah_angin = "barat laut";
            }

            printf("UART: %s \n", wind_dir_data.s_angin.c_str());
            printf("Arah angin: %s \n", wind_dir_data.arah_angin.c_str());
        }
        delay(1000);
    }
}
