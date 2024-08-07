int swauto = 12; // công tắc auto.
int relayauto = 7; // chân điều khiển relay B+.
int relaylow = 5; // chân điều khiển relay +1.
int relayhigh = 4; // chân điều khiển relay +2.
int relayS = 2; // chân điều khiển relay +S.
int rain = A0; // chân nhận tín hiệu từ cảm biến mưa.
int giatricambienmua = 0; // giá trị lưu giữ tín hiệu cảm biến mưa.
unsigned long khoangthoigian;
unsigned long thoigiandau; 
int timestopFLAG = 0; 
void setup(){
  Serial.begin(9600);
  pinMode(swauto, INPUT_PULLUP); // đặt trạng thái trở kéo lên tại chân modeauto.
  pinMode(relayauto, OUTPUT); // 
  digitalWrite(relayauto, LOW); // đặt trạng thái ban đầu chân relayauto ở mức thấp.
  pinMode(relayS, OUTPUT);
  digitalWrite(relayS, LOW); // đặt trạng thái ban đầu chân relayS ở mức thấp.
  pinMode(relaylow, OUTPUT);
  digitalWrite(relaylow, LOW); // đặt trạng thái ban đầu chân relaylow ở mức thấp.
  pinMode(relayhigh, OUTPUT);
  digitalWrite(relayhigh, LOW); // đặt trạng thái ban đầu chân relayhigh ở mức thấp.
  //đặt thời gian đầu bằng 0.
  thoigiandau = millis();
}
// Khi trời không mưa hoặc công tắc auto tắt thì điều khiển ngắt relay.
void norain(){
  digitalWrite(relayS, LOW);
  digitalWrite(relaylow, LOW);
  digitalWrite(relayhigh, LOW);
}
// Khi công tắc auto bật, trời mưa nhẹ thì điều khiển đóng, ngắt relay.
void chedolow(){
  digitalWrite(relayS, HIGH);
  digitalWrite(relayhigh, LOW);
  delay(100);
  digitalWrite(relaylow, HIGH);
}
// Khi công tắc auto bật, trời mưa lớn thì điều khiển đóng, ngắt relay.
void chedohigh(){
  digitalWrite(relayS, HIGH);
  digitalWrite(relaylow, LOW);
  delay(100);
  digitalWrite(relayhigh, HIGH);
}
// Hàm điều khiển thời gian định thời motor
// khi nhận biết giá trị cảm biến mưa.
void settimeint(){
  khoangthoigian = millis() - thoigiandau;
  // Nếu flag = 0, motor đang quay.
  if(timestopFLAG == 0){
    // Nếu motor quay đến điểm ban đầu.
    if (khoangthoigian >= 1450){
      thoigiandau = millis();
      // Nếu giá trị cảm biến mưa nằm trong khoảng này thì cờ có giá trị = 2.
      if ((giatricambienmua > 750) && (giatricambienmua <= 900)){          
        timestopFLAG = 2;
      }
      // Nếu giá trị cảm biến mưa nằm trong khoảng này thì cờ có giá trị = 1.
      else if ((giatricambienmua > 600) && (giatricambienmua <= 750)){          
        timestopFLAG = 1;
      }
    }
    // Nếu motor chưa quay đến điểm ban đầu, đặt cờ có giá trị = 0 thì motor tiếp trục quay.
    else if (khoangthoigian < 1450){
      timestopFLAG = 0;
    }
  }
  // Nếu cờ = 1 thì cho chế độ int định thời 2 giây.
  if(timestopFLAG == 1){
    // Nếu thời gian định thời xong 2 giây thì gán cho cờ = 0 để motor quay.
    if (khoangthoigian >= 2000){
      thoigiandau = millis();
      timestopFLAG = 0;
    }
    // Nếu thời gian định thời chưa xong thì motor vẫn không hoạt động.
    else if (khoangthoigian < 2000){
      timestopFLAG = 1;
    }
  }
  // Nếu cờ = 1 thì cho chế độ int định thời 4 giây.
  if(timestopFLAG == 2){
    // Nếu thời gian định thời xong 4 giây thì gán cho cờ = 0 để motor quay.
    if (khoangthoigian >= 4000){
      thoigiandau = millis();
      timestopFLAG = 0;
    }
    // Nếu thời gian định thời chưa xong thì motor vẫn không hoạt động.
    else if (khoangthoigian < 4000){
      timestopFLAG = 2;
    }
  }
}
void loop() {
  // Khi công tắc auto được bật.
  if (digitalRead(swauto) == 0){
    // Kiểm tra liệu công tắc đã được bật hay chưa.
    while (digitalRead(swauto) == 0){
      // Đọc tín hiệu analog từ cảm biến mưa và lưu giá trị đó vào biến a.
      giatricambienmua = analogRead(rain);
      // Relayauto được kích hoạt, ngắt dòng điện qua chân B+ của công tăc 
      // và đi qua chân B+ của nguồn đến relaylow và relayhigh.
      digitalWrite(relayauto, HIGH);
      // Công tắc đã bật, gửi dữ liệu lên labview.
      Serial.print("*AUTO@");
      Serial.print(giatricambienmua);
      // Nếu trời chưa mưa.
      if (giatricambienmua > 900){
        norain();
      }
      // Nếu trời mưa nhẹ, relaylow được kích hoạt, dòng điện từ relayauto 
      // đi qua chân +1 của motor gạt mưa. Chế độ INT hoạt động.
      else if ((giatricambienmua > 600) && (giatricambienmua <= 900)){
        // 
        settimeint();
        if(timestopFLAG == 0){
          chedolow();
        }
        else if(timestopFLAG == 1 || timestopFLAG == 2 ){
          norain();
        }
      }
      // Nếu trời mưa nhẹ, relaylow được kích hoạt, dòng điện từ relayauto 
      // đi qua chân +1 của motor gạt mưa.
      else if ((giatricambienmua >= 400) && (giatricambienmua <= 600)){
        chedolow();
      }
      // Nếu trời mưa nhẹ, relaylow được kích hoạt, dòng điện từ relayauto 
      // đi qua chân +2 của motor gạt mưa.
      else if ((giatricambienmua > 0) && (giatricambienmua < 400)){
        chedohigh();
      }
    }
  }
  // Khi chưa bật công tắc auto hoặc tắt công tắc auto.
  else{
    // relayauto không kích hoạt.
    digitalWrite(relayauto, LOW);
    norain();
    // Công tắc đã tắt, gửi dữ liệu lên labview.
    Serial.print("*NOAUTO@");
  }
}