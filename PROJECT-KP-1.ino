#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>
int buzzer = 8;
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);
RTC_DS3231 rtc;

// Inisialisasi SoftwareSerial untuk DFPlayer Mini
SoftwareSerial mySerial(2, 3); // RX, TX

// Jadwal waktu kegiatan untuk setiap hari
const char* schedule[7] = {
    "10:31:00", // Jam 1
    "10:00:00", // Jam 2
    "10:33:00", // Jam 3
    "14:00:00", // Jam 4
    "16:00:00", // Jam 5
    "10:23:00"  // Jam 6
    "10:23:00"  // Jam 7
};

// Pemetaan file MP3 untuk setiap jadwal
const int mp3Files[7] = {
    1, // File MP3 untuk Jam 1
    2, // File MP3 untuk Jam 2
    3, // File MP3 untuk Jam 3
    4, // File MP3 untuk Jam 4
    5, // File MP3 untuk Jam 5
    6,
    7  // File MP3 untuk Jam 6
};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin();
  lcd.backlight();
  lcd.print("Selamat Datang.!");
  delay(1000);
  lcd.clear();
  pinMode(buzzer, OUTPUT); 


  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Inisialisasi DFPlayer Mini
  mySerial.begin(9600);
  mp3_set_serial(mySerial); // Ubah ke komunikasi serial SoftwareSerial
  delay(100);
  mp3_set_volume(15); // Set volume (0~30)
  for (int i = 0; i < 3; i++) {
    tone(buzzer, 5000);  // Menghasilkan suara dengan frekuensi 1000 Hz
    delay(150);  // Durasi suara pertama
    noTone(buzzer);  // Matikan suara
    delay(150);  // Jeda singkat
    tone(buzzer, 1500);  // Menghasilkan suara dengan frekuensi 1500 Hz
    delay(150);  // Durasi suara kedua
    noTone(buzzer);  // Matikan suara
    delay(150);  // Jeda singkat
  } 
  digitalWrite(buzzer, HIGH);
}

void loop() {
  DateTime now = rtc.now();
  
  // Format waktu saat ini menjadi string
  char currentTime[9];
  sprintf(currentTime, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  
  // Mendapatkan nama hari sekarang
  const char* dayNames[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
  int currentDayIndex = now.dayOfTheWeek();
  const char* currentDayName = dayNames[currentDayIndex];

  Serial.print(currentDayName);
  Serial.print(" ");

  lcd.setCursor(5,0);
  lcd.print(currentDayName);
  lcd.setCursor(4,1);
  lcd.print(currentTime);

  Serial.println(currentTime); // Tampilkan waktu saat ini di Serial Monitor

  // Periksa jadwal hari ini
  for (int i = 0; i < 7; i++) {
    if (strcmp(currentTime, schedule[i]) == 0) {
      Serial.print("Waktu pada jadwal hari ini: ");
      Serial.print(currentDayName);
      Serial.println(schedule[i]);

      // Mainkan file MP3 jika waktu sesuai jadwal
      mp3_play(mp3Files[i]);  // Mainkan file MP3 yang sesuai
      break; // Keluar dari loop setelah menemukan waktu pada jadwal
    }
  }

  delay(1000);
}
