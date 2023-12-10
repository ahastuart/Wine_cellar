# Iot_Cloud_Platform Final Project


# 스마트 와인 냉장고 매니저 (Wine_cellar) 🍷
-----
###### 본 프로젝트는 <span style="color:9f0a28">와인 애호가</span>들을 위한 편리한 <span style="color:9f0a28">와인 저장 및 재고 관리</span> 솔루션을 제공하면서, 사용자가 냉장고의 상태를 실시간으로 모니터링하고 <span style="color:9f0a28">와인의 보관 온도를 최적화</span>하여 와인의 맛과 향을 최대화하려는 목적이 있다.
-----
<br>

### 1. 온습도 센서를 활용한 와인 상태 모니터링
와인 냉장고는 3층으로 이루어져 있고, 각 층 마다 온습도 센서가 부착이 되어 있어 와인 보관 상태를 앱으로 모니터링 할 수 있다.
### 2. 초음파 센서를 활용한 와인 재고 모니터링
각 층의 옆면 쪽에 초음파 센서를 부착하여 와인 재고 상태를 앱으로 모니터링 할 수 있다. (원래 가정은 ~~무게 센서~~)
### 3. 냉장고 온습도 조절 원격 제어
사용자가 앱으로 온습도 상태를 보고 원하는 온도를 높이거나 낮게 할 수 있도록, 쿨링팬과 히터 가동 ON,OFF
### 4. 앱 알림 서비스
온습도의 이상 및 재고 부족 시, 경고 메시지 알림 서비스

<br>

# 1. 라이브러리 설치
- WiFiNINA (or WiFi101 for the MKR1000)
- ArduinoBearSSL
- ArduinoECCX08
- ArduinoMqttClient
- Arduino Cloud Provider Examples
- DHT sensor library


# 2. AWS IoT 정책 생성
디바이스가 MQTT 주제 구독, 게시 등의 AWS IoT 작업을 수행할 수 있는 권한을 부여하는데 사용한다. <br>
그래서 이를 생성하여 디바이스 인증서에 연결하여 AWS IoT 작업을 수행할 수 있게 된다. <br>
과정: IoT Core에서 보안 - 정책 - [생성] 선택 *(정책이름: AllowWine)* <br>
작업 필드에 iot:* 입력 -> 리소스 ARN 필드에 * -> [허용] -> 생성



