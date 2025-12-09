
/*
  프로젝트: 완벽한 반려식물 매니저 (조도 + 온습도)
  
  기능:
  - 조도(CdS) 측정 -> 어두우면 파란불 (일조량 부족)
  - 습도(DHT11) 측정 -> 건조하면 빨간불 (물 부족)
  - 온도(DHT11) 측정 -> 고온/저온이면 경보음
  - 모두 정상이면 초록불 (행복함)
*/
/*
// 라이브러리 불러오기 (설치 필요)
#include <DHT.h>

// --- 핀 번호 설정 ---
#define DHTPIN 2        // 온습도 센서 (D2)
#define DHTTYPE DHT11   // 센서 종류 (DHT11)

const int CDS_PIN    = A0; // 조도 센서 (A0)

// RGB LED 핀 설정 (공통 캐소드 기준)
const int RGB_RED    = 4;  // 빨간색 핀
const int RGB_GREEN  = 5;  // 초록색 핀
const int RGB_BLUE   = 6;  // 파란색 핀

const int BUZZER_PIN = 8;  // 온도 경고 부저

// --- 기준값 설정 (환경에 맞게 수정하세요) ---
const int LIGHT_LIMIT = 300; // 이보다 낮으면 어두움 (0~1023)
const int HUMID_LIMIT = 40;  // 습도가 40% 미만이면 건조함
const int TEMP_HIGH   = 30;  // 30도 이상이면 더움
const int TEMP_LOW    = 18;  // 18도 미만이면 추움

// 센서 객체 생성
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin(); // 온습도 센서 시작

  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("식물 매니저 시스템 시작...");
}

void loop() {
  delay(2000); // DHT11 센서는 읽는데 시간이 좀 필요합니다 (최소 2초)

  // 1. 센서 값 읽기
  float h = dht.readHumidity();    // 습도 (%)
  float t = dht.readTemperature(); // 온도 (섭씨)
  int light = analogRead(CDS_PIN); // 조도 (0~1023)

  // 센서가 고장났는지 확인
  if (isnan(h) || isnan(t)) {
    Serial.println("DHT 센서 오류! 연결을 확인하세요.");
    return;
  }

  // 시리얼 모니터에 현재 상태 출력
  Serial.print("습도: "); Serial.print(h); Serial.print(" %\t");
  Serial.print("온도: "); Serial.print(t); Serial.print(" C\t");
  Serial.print("밝기: "); Serial.println(light);

  // 2. 상태 판단 및 LED/부저 제어
  bool problem = false; // 문제가 있는지 체크하는 변수

  // LED 초기화 (모두 끔)
  digitalWrite(RGB_RED, LOW);
  digitalWrite(RGB_GREEN, LOW);
  digitalWrite(RGB_BLUE, LOW);

  // [습도 체크] 너무 건조하면 빨간불 켜기 (RGB_RED)
  if (h < HUMID_LIMIT) {
    digitalWrite(RGB_RED, HIGH); 
    Serial.println(">> 경고: 너무 건조해요! 물을 주세요.");
    problem = true;
  }

  // [조도 체크] 너무 어두우면 파란불 켜기 (RGB_BLUE)
  // 만약 건조하면서 어둡다면, 빨강+파랑이 켜져서 보라색이 됩니다.
  if (light < LIGHT_LIMIT) {
    digitalWrite(RGB_BLUE, HIGH);
    Serial.println(">> 경고: 햇빛이 부족해요.");
    problem = true;
  }

  // [온도 체크] 너무 덥거나 추우면 부저 소리
  if (t >= TEMP_HIGH || t <= TEMP_LOW) {
    tone(BUZZER_PIN, 1000, 500); // 0.5초간 삑!
    Serial.println(">> 위험: 온도가 적절하지 않아요!");
    problem = true;
  }

  // [종합 판정] 아무 문제가 없으면 초록불 (RGB_GREEN)
  if (problem == false) {
    digitalWrite(RGB_GREEN, HIGH);
  } 
  // 문제가 있는 경우(problem == true)에는 위에서 켠 빨강이나 파랑이 유지됩니다.
}

*/






/*
  프로젝트: 식물 관리 매니저 (Processng + App 듀얼 연동)
  
  기능:
  - 센서(DHT11, CdS) 값을 측정합니다.
  - PC(Processing)를 위해 USB 시리얼로 데이터를 전송합니다.
  - 스마트폰(Flutter)을 위해 블루투스로 데이터를 전송합니다.
  - 데이터 포맷: "습도,온도,조도" (예: 45.0,26.5,300)
*/

#include <DHT.h>
#include <SoftwareSerial.h> // 블루투스 통신용

// --- 핀 번호 설정 ---
#define DHTPIN 2        
#define DHTTYPE DHT11   

// 블루투스 (HC-06) 연결 핀
const int BT_RX = 10;
const int BT_TX = 11;

const int CDS_PIN    = A0; 
const int RGB_RED    = 4;  // 빨강 (습도 경고)
const int RGB_GREEN  = 5;  // 초록 (정상)
const int RGB_BLUE   = 6;  // 파랑 (조도 경고)
const int BUZZER_PIN = 8;  // 부저 (온도 경고)

// --- 기준값 설정 ---
const int LIGHT_LIMIT = 300; 
const int HUMID_LIMIT = 40;  
const int TEMP_HIGH   = 30;  
const int TEMP_LOW    = 18;  

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial btSerial(BT_RX, BT_TX); // RX, TX 객체 생성

void setup() {
  Serial.begin(9600);   // PC(Processing) 통신용
  btSerial.begin(9600); // 스마트폰(Flutter) 통신용
  dht.begin();

  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // 시작 메시지는 PC에만 출력 (데이터 파싱 오류 방지)
  Serial.println("System Started."); 
}

void loop() {
  delay(2000); // 2초 대기

  // 1. 센서 값 읽기
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int light = analogRead(CDS_PIN);

  // 센서 에러 체크
  if (isnan(h) || isnan(t)) {
    Serial.println("Error: DHT Sensor failed");
    return;
  }

  // 2. 데이터 포맷팅 (CSV: 습도,온도,조도)
  // 예: 45.00,26.50,500
  String data = String(h) + "," + String(t) + "," + String(light);
  
  // 3. 듀얼 전송 (PC와 스마트폰 양쪽으로 발송)
  Serial.println(data);   // -> USB 케이블 -> Processing
  btSerial.println(data); // -> 블루투스 -> Flutter App

  // 4. LED/부저 자동 제어
  controlActuators(h, t, light);
}

void controlActuators(float h, float t, int light) {
  bool problem = false;
  
  // LED 초기화
  digitalWrite(RGB_RED, LOW);
  digitalWrite(RGB_GREEN, LOW);
  digitalWrite(RGB_BLUE, LOW);

  // 습도 체크
  if (h < HUMID_LIMIT) {
    digitalWrite(RGB_RED, HIGH); 
    problem = true;
  }
  
  // 조도 체크
  if (light < LIGHT_LIMIT) {
    digitalWrite(RGB_BLUE, HIGH);
    problem = true;
  }
  
  // 온도 체크 (부저)
  if (t >= TEMP_HIGH || t <= TEMP_LOW) {
    // 소리가 너무 자주 나지 않게 짧게 처리
    tone(BUZZER_PIN, 1000, 100); 
    problem = true;
  }

  // 정상 상태
  if (problem == false) {
    digitalWrite(RGB_GREEN, HIGH);
  } 
}