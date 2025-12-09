import processing.serial.*;

Serial myPort;        
String val;           

float humidity = 0;   
float temp = 0;       
float light = 0;      

void setup() {
  size(600, 400);
  
  // 포트 자동 선택 (주의: 포트가 안 맞으면 [0]을 [1], [2]로 변경해보세요)
  printArray(Serial.list());
  
  try {
    String portName = Serial.list()[3]; 
    myPort = new Serial(this, portName, 9600);
  } catch (Exception e) {
    println("Error opening serial port. Please check connection.");
  }
}

void draw() {
  background(50); // 어두운 배경
  
  // 데이터 수신
  if (myPort != null && myPort.available() > 0) {  
    val = myPort.readStringUntil('\n'); 
  } 
  
  if (val != null) {
    val = trim(val);
    float[] sensors = float(split(val, ',')); 
    
    if (sensors.length == 3) {
      humidity = sensors[0];
      temp = sensors[1];
      light = sensors[2];
    }
  }
  
  // 시각화 (바 그래프)
  drawBar(100, "Humidity", humidity, 100, color(50, 150, 255)); // 파랑
  drawBar(200, "Temp", temp, 50, color(255, 100, 100));         // 빨강
  
  // 조도는 0~1023이므로 스케일 조정
  float lightScaled = map(light, 0, 1023, 0, 100);
  drawBar(300, "Light", lightScaled, 100, color(255, 200, 50)); // 노랑
  
  // 텍스트 정보 표시
  fill(255);
  textSize(24);
  textAlign(CENTER);
  text("Smart Plant Monitor", width/2, 50);
}

void drawBar(int y, String label, float value, float maxVal, int c) {
  float barWidth = map(value, 0, maxVal, 0, 400);
  
  fill(200);
  textSize(16);
  textAlign(RIGHT);
  text(label, 90, y + 25);
  
  fill(c);
  rect(100, y, barWidth, 30);
  
  fill(255);
  textAlign(LEFT);
  text(nf(value, 0, 1), 100 + barWidth + 10, y + 25);
}
