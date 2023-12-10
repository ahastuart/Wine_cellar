# Iot_Cloud_Platform Final Project


# 스마트 와인 냉장고 매니저 (Wine_cellar) 🍷
-----
###### 본 프로젝트는 <span style="color:red">와인 애호가</span>들을 위한 편리한 <span style="color:red">와인 저장 및 재고 관리</span> 솔루션을 제공하면서, 사용자가 냉장고의 상태를 실시간으로 모니터링하고 <span style="color:red">와인의 보관 온도를 최적화</span>하여 와인의 맛과 향을 최대화하려는 목적이 있다.
-----


### 1. 온습도 센서를 활용한 와인 상태 모니터링
와인 냉장고는 3층으로 이루어져 있고, 각 층 마다 온습도 센서가 부착이 되어 있어 와인 보관 상태를 앱으로 모니터링 할 수 있다.
### 2. 초음파 센서를 활용한 와인 재고 모니터링
각 층의 옆면 쪽에 초음파 센서를 부착하여 와인 재고 상태를 앱으로 모니터링 할 수 있다. (원래 가정은 ~~무게 센서~~)
### 3. 냉장고 온습도 조절 원격 제어
사용자가 앱으로 온습도 상태를 보고 원하는 온도를 높이거나 낮게 할 수 있도록, 쿨링팬과 히터 가동 ON,OFF
### 4. 앱 알림 서비스
온습도의 이상 및 재고 부족 시, 경고 메시지 알림 서비스



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
1. IoT Core에서 보안 - 정책 - [생성] 선택 *(정책이름: AllowWine)* <br>
2. 작업 필드에 iot:* 입력 -> 리소스 ARN 필드에 * -> [허용] -> 생성



# 3. X.509 인증서 사용하여 디바이스 인증
AWS IoT Core는 X.509 인증서를 사용하여 디바이스를 인증한다. <br>
1. Arduino IDE의 파일 - 예제 - ArduinoECCX08 - Tools - ECCX08CSR 선택하여 업로드 <br>
2. Common Name에만 앞으로 만들 사물 이름*(MyMy)* 적은 후, csr 생성 -> 따로 csr.txt파일로 저장하기



# 4. 레지스트리에 디바이스 등록
1. AWS IoT Core에서 관리 - 사물 - 사물생성 - 단일 사물생성 *(사물이름: MyMY)* - 이름 없는 섀도우 - csr 업로드 - 정책 연결 *(AllowWine)* -  사물 생성



# 5. 디바이스 구성 및 테스트
1. AWS IoT Core에서 보안 - 인증서 - *[드롭다운]* 활성화 - 다운로드 <br>
2. Arduino_code 폴더에 있는 파일 열기 <br>
3. arduino_secret.h 탭에서 개인 WiFi SSID와 Password 설정 -> 엔드포인트 설정 *(AWS IoT Core - 설정에서 확인 가능)* -> 인증서 파일 복사 붙여넣기 *(앞서 다운로드 파일 열기)* <br>



# 6. AWS DynamoDB 테이블 생성 및 Lambda 함수 정의
DynamoDB에서 테이블 - 테이블 생성 - 테이블 이름 *(WineTable)* - 파티션 키 *(deviceId: 문자열) - 정렬 키 (time: 번호) <br>
IntelliJ IDEA에서 프로젝트 생성 ( 프로젝트 이름: *LoggingWineLambda* / Runtime: *java11* / SDK: *11버전* ) <br>
build.gradle 파일에서 의존성 추가 후, 변경사항 반영
src/main/java/helloworld/App.java 파일

```
package helloworld;

import java.text.SimpleDateFormat;
import java.util.TimeZone;

import com.amazonaws.services.dynamodbv2.AmazonDynamoDB;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClientBuilder;
import com.amazonaws.services.dynamodbv2.document.DynamoDB;
import com.amazonaws.services.dynamodbv2.document.Item;
import com.amazonaws.services.dynamodbv2.document.spec.PutItemSpec;
import com.amazonaws.services.dynamodbv2.model.ConditionalCheckFailedException;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;

public class App implements RequestHandler<Document, String> {
    private DynamoDB dynamoDb;
    private String DYNAMODB_TABLE_NAME = "WineTable";

    @Override
    public String handleRequest(Document input, Context context) {
        this.initDynamoDbClient();
        context.getLogger().log("Input: " + input);

        //return null;
        return persistData(input);
    }

    private String persistData(Document document) throws ConditionalCheckFailedException {

        // Epoch Conversion Code: https://www.epochconverter.com/
        SimpleDateFormat sdf = new SimpleDateFormat ( "yyyy-MM-dd HH:mm:ss");
        sdf.setTimeZone(TimeZone.getTimeZone("Asia/Seoul"));
        String timeString = sdf.format(new java.util.Date (document.timestamp*1000));


        if (document.current.state.reported.inventory.equals(
                document.previous.state.reported.inventory) &&
                document.current.state.reported.inventory3.equals(
                        document.previous.state.reported.inventory3) &&
                document.current.state.reported.LED.equals(
                        document.previous.state.reported.LED) &&
                document.current.state.reported.LED3.equals(
                        document.previous.state.reported.LED3) &&
                document.current.state.reported.temperature.equals(
                        document.previous.state.reported.temperature) &&
                document.current.state.reported.temperature3.equals(
                        document.previous.state.reported.temperature3) &&
                document.current.state.reported.humidity3.equals(
                        document.previous.state.reported.humidity3) &&
                document.current.state.reported.humidity.equals(
                        document.previous.state.reported.humidity)) {
            return null;
        }

        return this.dynamoDb.getTable(DYNAMODB_TABLE_NAME)
                .putItem(new PutItemSpec().withItem(new Item().withPrimaryKey("deviceId", document.device)
                        .withLong("time", document.timestamp)
                        .withString("temperature", document.current.state.reported.temperature)
                        .withString("humidity", document.current.state.reported.humidity)
                        .withString("temperature3", document.current.state.reported.temperature3)
                        .withString("humidity3", document.current.state.reported.humidity3)
                        .withString("inventory", document.current.state.reported.inventory)
                        .withString("inventory3", document.current.state.reported.inventory3)
                        .withString("LED", document.current.state.reported.LED)
                        .withString("LED3", document.current.state.reported.LED3)
                        .withString("timestamp",timeString)))
                .toString();
    }

    private void initDynamoDbClient() {
        AmazonDynamoDB client = AmazonDynamoDBClientBuilder.standard().withRegion("ap-northeast-2").build();

        this.dynamoDb = new DynamoDB(client);
    }

}

class Document {
    public Thing previous;
    public Thing current;
    public long timestamp;
    public String device;       // AWS IoT에 등록된 사물 이름
}

class Thing {
    public State state = new State();
    public long timestamp;
    public String clientToken;

    public class State {
        public Tag reported = new Tag();
        public Tag desired = new Tag();

        public class Tag {
            public String temperature;
            public String humidity;
            public String temperature3;
            public String humidity3;
            public String inventory;
            public String inventory3;
            public String LED;
            public String LED3;

        }
    }
}
```





