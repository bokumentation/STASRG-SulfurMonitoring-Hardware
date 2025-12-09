Original Example Code from Vendor


```cpp
// anemometer parameters
volatile byte rpmcount;             // hitung signals
volatile unsigned long last_micros;

unsigned long timeold;
unsigned long timemeasure = 10.00;  // detik
int timetoSleep = 1;                // menit
unsigned long sleepTime = 15;       // menit
unsigned long timeNow;
int countThing = 0;
int GPIO_pulse = 14;                // ESP32 = D14
float rpm, rotasi_per_detik;        // rotasi/detik
float kecepatan_kilometer_per_jam;  // kilometer/jam
float kecepatan_meter_per_detik;    //meter/detik
volatile boolean flag = false;

void ICACHE_RAM_ATTR rpm_anemometer()
{
  flag = true;
}

void setup()
{
  pinMode(GPIO_pulse, INPUT_PULLUP);
  digitalWrite(GPIO_pulse, LOW);
  Serial.begin(9600);
  detachInterrupt(digitalPinToInterrupt(GPIO_pulse));                         // memulai Interrupt pada nol
  attachInterrupt(digitalPinToInterrupt(GPIO_pulse), rpm_anemometer, RISING); //Inisialisasi pin interupt
  rpmcount = 0;
  rpm = 0;
  timeold = 0;
  timeNow = 0;
} // end of setup

void loop()
{
  // Cek apakah flag bernilai true, yang menandakan bahwa kondisi tertentu telah terpenuhi
  if (flag == true)
  {
    // Cek apakah waktu sejak deteksi terakhir sudah lebih dari atau sama dengan 5000 mikrodetik (5 milidetik)
    if (long(micros() - last_micros) >= 5000)
    {
      // rJika ya, maka tambahkan satu ke penghitung rpm (pmcount)
      rpmcount++;
      // Perbarui waktu terakhir ketika magnet terdeteksi dengan waktu saat ini
      last_micros = micros();
    }
    // Setelah memproses, setel flag kembali ke false untuk menandakan bahwa kondisi telah ditangani
    flag = false; // reset flag
  }

  if ((millis() - timeold) >= timemeasure * 1000)
  {
    countThing++;
    detachInterrupt(digitalPinToInterrupt(GPIO_pulse));      // Menonaktifkan interrupt saat menghitung
    rotasi_per_detik = float(rpmcount) / float(timemeasure); // rotasi per detik
    //kecepatan_meter_per_detik = rotasi_per_detik; // rotasi/detik sebelum dikalibrasi untuk dijadikan meter per detik
    kecepatan_meter_per_detik = ((-0.0181 * (rotasi_per_detik * rotasi_per_detik)) + (1.3859 * rotasi_per_detik) + 1.4055); // meter/detik sesudah dikalibrasi dan sudah dijadikan meter per detik
    if (kecepatan_meter_per_detik <= 1.5)
    { // Minimum pembacaan sensor kecepatan angin adalah 1.5 meter/detik
      kecepatan_meter_per_detik = 0.0;
    }

    kecepatan_kilometer_per_jam = kecepatan_meter_per_detik * 3.6; // kilometer/jam
    Serial.print("rotasi_per_detik=");
    Serial.print(rotasi_per_detik);
    Serial.print("   kecepatan_meter_per_detik="); // Minimal kecepatan angin yang dapat dibaca sensor adalah 4 meter/detik dan maksimum 30 meter/detik.
    Serial.print(kecepatan_meter_per_detik);
    Serial.print("   kecepatan_kilometer_per_jam=");
    Serial.print(kecepatan_kilometer_per_jam);
    Serial.println("   ");

    if (countThing == 1) // kirim data per 10 detik sekali
    {
      Serial.println("Mengirim data ke server");
      countThing = 0;
    }
    timeold = millis();
    rpmcount = 0;
    attachInterrupt(digitalPinToInterrupt(GPIO_pulse), rpm_anemometer, RISING); // enable interrupt
  }
} // end of loop

```