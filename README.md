# Iot_Cloud_Platform Final Project


# 스마트 와인 냉장고 매니저 (Wine_cellar) 🍷
-----
###### 본 프로젝트는 와인 애호가들을 위한 편리한 와인 저장 및 재고 관리 솔루션을 제공하면서, 사용자가 냉장고의 상태를 실시간으로 모니터링하고 와인의 보관 온도를 최적화하여 와인의 맛과 향을 최대화하려는 목적이 있다.
-----


### 1. 온습도 센서를 활용한 와인 상태 모니터링 📲
와인 냉장고는 3층으로 이루어져 있고, 각 층 마다 온습도 센서가 부착이 되어 있어 와인 보관 상태를 앱으로 모니터링 할 수 있다.
### 2. 초음파 센서를 활용한 와인 재고 모니터링 📲
각 층의 옆면 쪽에 초음파 센서를 부착하여 와인 재고 상태를 앱으로 모니터링 할 수 있다. (원래 가정은 ~~무게 센서~~)
### 3. 냉장고 온습도 조절 원격 제어 ↕
사용자가 앱으로 온습도 상태를 보고 원하는 온도를 높이거나 낮게 할 수 있도록, 쿨링팬과 히터 가동 ON,OFF
### 4. 앱 알림 서비스 🔔
온습도의 이상 및 재고 부족 시, 경고 메시지 알림 서비스

![initial](https://github.com/ahastuart/Wine_cellar/assets/117140125/f0d691cb-4dd0-4182-90a2-716d09e0bbef)


# 실행 결과 및 설명

<img src="https://github.com/ahastuart/Wine_cellar/assets/117140125/b9d059b3-a346-4465-80d1-ebaea40c69ae" width="30%" height="30%"/>
<br>앱 초기화면

<br>
<img src="https://github.com/ahastuart/Wine_cellar/assets/117140125/cc1f4c6a-6898-421f-9732-6cbc332a6934" width="30%" height="30%"/>
<br> 앱 목록조회 (사물: MyMy)

<br>
<img src="https://github.com/ahastuart/Wine_cellar/assets/117140125/3af1d2cf-e3d7-4b8f-b858-25fe04a358ff" width="30%" height="30%"/>
<br> 앱 상태조회 (1,3층의 온습도 + 재고 / 냉각팬과 히터의 상태)

<br>
<img src="https://github.com/ahastuart/Wine_cellar/assets/117140125/f7021b36-fc42-49f0-9578-ac79a03db20e" width="30%" height="30%"/>
<br> 앱 상태변경 (냉각팬 상태가 ON으로 바뀜)

<br>
<img src="https://github.com/ahastuart/Wine_cellar/assets/117140125/7edc49d5-af4d-49fd-bcb3-975799e4dd16" width="30%" height="30%"/>
<br> 앱 로그조회


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
1. DynamoDB에서 테이블 - 테이블 생성 - 테이블 이름 *(WineTable)* - 파티션 키 *(deviceId: 문자열) - 정렬 키 (time: 번호) <br>
2. IntelliJ IDEA에서 프로젝트 생성 ( 프로젝트 이름: *LoggingWineLambda* / Runtime: *java11* / SDK: *11버전* ) <br>
3. build.gradle 파일에서 의존성 추가 후, 변경사항 반영 -> App 파일 수정 -> AppTest 파일 주석처리 -> 컴파일일
-----
src/main/java/helloworld/App.java 파일 Code <br>
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
-json 예시 <br>
```
{
  "previous": {
    "state": {
      "reported": {
        "temperature": "2",
        "humidity": "3",
        "temperature3": "3",
        "humidity3": "8",
        "inventory" : "1",
        "inventory3" : "1",
        "LED": "OFF",
        "LED3": "ON"
      }
    }
  },
  "current": {
    "state": {
      "reported": {
        "temperature": "9",
        "humidity": "48",
        "temperature3": "26",
        "humidity3": "74",
        "inventory" : "0",
        "inventory3" : "0",
        "LED": "OFF",
        "LED3": "OFF"
      }
    }
  },
  "timestamp": 1575178117,
  "device":"MyMy"
}
```
-----
4. 람다함수 *(LoggingWineLambda)* 함수 생성 후, 배포 -> AmazonDynamoDBFullAccess 정책 권한 추가  <br>
5. AWS IoT Core에서 메시지 라우팅 - 규칙 - 규칙생성 - SQL문 입력 - lambda함수 *(LoggingWineLambda)* 선택 후 생성
   SELECT * FROM '$aws/things/MyMy/shadow/update/accepted'  <br>


# 7. API Gateway를 통한 REST API 구축하기
## 7-1. 디바이스 목록 조회
- Lambda Name: ListingWineLambda  <br>
- version: java 11  <br>
-----
App.java Code
```
package helloworld;
import java.util.List;
import com.amazonaws.services.iot.AWSIot;
import com.amazonaws.services.iot.AWSIotClientBuilder;
import com.amazonaws.services.iot.model.ListThingsRequest;
import com.amazonaws.services.iot.model.ListThingsResult;
import com.amazonaws.services.iot.model.ThingAttribute;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;

public class App implements RequestHandler<Object, String> {

    @Override
    public String handleRequest(Object input, Context context) {

        // AWSIot 객체를 얻는다.
        AWSIot iot = AWSIotClientBuilder.standard().build();

        // ListThingsRequest 객체 설정.
        ListThingsRequest listThingsRequest = new ListThingsRequest();

        // listThings 메소드 호출하여 결과 얻음.
        ListThingsResult result = iot.listThings(listThingsRequest);

        return getResultStr(result);
    }


    /**
     * ListThingsResult 객체인 result로 부터 ThingName과 ThingArn을 얻어서 Json문자 형식의
     * 응답모델을 만들어 반환한다.
     * {
     * 	"things": [
     *	     {
     *			"thingName": "string",
     *	      	"thingArn": "string"
     *	     },
     *		 ...
     *	   ]
     * }
     */
    private String getResultStr(ListThingsResult result) {
        List<ThingAttribute> things = result.getThings();

        String resultString = "{ \"things\": [";
        for (int i =0; i<things.size(); i++) {
            if (i!=0)
                resultString +=",";
            resultString += String.format("{\"thingName\":\"%s\", \"thingArn\":\"%s\"}",
                    things.get(i).getThingName(),
                    things.get(i).getThingArn());

        }
        resultString += "]}";
        return resultString;
    }

}
```
-----
1. Lambda 함수 생성 후, AWSIoTFullAccess 권한 추가 -> 테스트  <br>
2. API Gateway에서 API 생성 - REST API 생성 - API 이름 *(wine-api)* - API 생성  <br>
3. 리소스 아래에 /를 선택 -> 리소스 생성 -> 리소스 이름 *(devices)*  <br>
4. 메서드 섹션 - 메소드 생성 - Get - 통합유형에서 Lambda함수(ListingWineLambda) 선택 - 메서드 생성 후 테스트  <br>
5. 리소스에서 /devices 선택 -> CORS 활성화 -> 모든 체크박스 선택 -> 저장  <br>
6. API 배포 - 새 스테이지 - prod - 배포  <br>


## 7-2. 디바이스 상태 조회
- Lambda Name: GetWineLambda  <br>
- version: java 11  <br>
-----
App.java Code
```
package helloworld;

import com.amazonaws.services.iotdata.AWSIotData;
import com.amazonaws.services.iotdata.AWSIotDataClientBuilder;
import com.amazonaws.services.iotdata.model.GetThingShadowRequest;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;

/**
 * Handler for requests to Lambda function.
 */
public class App implements RequestHandler<Event, String> {

    public String handleRequest(final Event event, final Context context) {

        AWSIotData iotData = AWSIotDataClientBuilder.standard().build();

        GetThingShadowRequest getThingShadowRequest =
                new GetThingShadowRequest()
                        .withThingName(event.device);

        String output = new String(
                iotData.getThingShadow(getThingShadowRequest).getPayload().array());

        return output;
    }
}

class Event {
    public String device;
}
```
-json 예시 <br>
```
{
  "device" : "MyMy"
}
```
-----
1. Lambda 함수 생성 후, AWSIoTFullAccess 권한 추가 -> 테스트  <br>
2. API Gateway에서 이전에 생성한 wine-api를 선택 -> /devices 선택 -> 리소스 생성 -> 리소스 이름 : *{devices}*  <br>
3. 메서드 섹션 - 메소드 생성 - Get - 통합유형에서 Lambda함수(GetWineLambda) 선택 - Lambda 프록시 통합은 선택안된 상태 유지 - 메서드 생성 후 테스트  <br>
4. 통합 요청 - 템플릿 생성 - 콘텐츠 유형: application/json - 메서드 요청 매스스루 - 템플릿 본문에 다음과 같이 입력 후 생성  <br>
```
 {
     "device": "$input.params('device')"
 }
```
6. 리소스에서 /devices/{devices} 선택 -> CORS 활성화 -> 모든 체크박스 선택 -> 저장  <br>
7. API 배포 - 새 스테이지 - prod - 배포  <br>


## 7-3. 디바이스 상태 변경
- Lambda Name: UpdateWineLambda  <br>
- version: java 11  <br>
-----
App.java Code
```
package helloworld;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.iotdata.AWSIotData;
import com.amazonaws.services.iotdata.AWSIotDataClientBuilder;
import com.amazonaws.services.iotdata.model.UpdateThingShadowRequest;
import com.amazonaws.services.iotdata.model.UpdateThingShadowResult;
import com.fasterxml.jackson.annotation.JsonCreator;

/**
 * Handler for requests to Lambda function.
 */
public class App implements RequestHandler<Event, String> {

    public String handleRequest(final Event event, final Context context) {
        AWSIotData iotData = AWSIotDataClientBuilder.standard().build();

        String payload = getPayload(event.tags);

        UpdateThingShadowRequest updateThingShadowRequest  =
                new UpdateThingShadowRequest()
                        .withThingName(event.device)
                        .withPayload(ByteBuffer.wrap(payload.getBytes()));

        UpdateThingShadowResult result = iotData.updateThingShadow(updateThingShadowRequest);
        byte[] bytes = new byte[result.getPayload().remaining()];
        result.getPayload().get(bytes);
        String output = new String(bytes);

        return output;
    }

    private String getPayload(ArrayList<Tag> tags) {
        String tagstr = "";
        for (int i=0; i < tags.size(); i++) {
            if (i !=  0) tagstr += ", ";
            tagstr += String.format("\"%s\" : \"%s\"", tags.get(i).tagName, tags.get(i).tagValue);
        }
        return String.format("{ \"state\": { \"desired\": { %s } } }", tagstr);
    }
}

class Event {
    public String device;
    public ArrayList<Tag> tags;

    public Event() {
        tags = new ArrayList<Tag>();
    }
}

class Tag {
    public String tagName;
    public String tagValue;

    @JsonCreator
    public Tag() {
    }

    public Tag(String n, String v) {
        tagName = n;
        tagValue = v;
    }
}

```
-json 예시 <br>
```
{
  "device": "MyMy",
  "tags" : [
    {
      "tagName": "temperature",
      "tagValue": "10"
    },
    {
      "tagName": "humidity",
      "tagValue": "33"
    },
    {
      "tagName": "temperature3",
      "tagValue": "25"
    },
    {
      "tagName": "humidity3",
      "tagValue": "72"
    },
    {
      "tagName": "inventory",
      "tagValue": "100"
    },
    {
      "tagName": "inventory3",
      "tagValue": "0"
    },
    {
      "tagName": "LED",
      "tagValue": "OFF"
    },
    {
      "tagName": "LED3",
      "tagValue": "OFF"
    }
  ]
}
```
-----
1. Lambda 함수 생성 후, AWSIoTFullAccess 권한 추가 -> 테스트  <br>
2. API Gateway에서 이전에 생성한 wine-api를 선택 -> /{device} 선택  메서드 섹션 - 메소드 생성 - PUT - 통합유형에서 Lambda함수(GetWineLambda) 선택- 메서드 생성  <br>
3. 모델 - 모델 생성 - 모델 이름: UpdateWineInput - 콘텐츠 유형: application/json - 스키마 정의
```
 {
      "$schema": "http://json-schema.org/draft-04/schema#",
      "title": "UpdateWineInput",
  	 "type" : "object",
      "properties" : {
      	"tags" : {
      		"type": "array",
             "items": {
                       "type": "object",
                       "properties" : {
                         	"tagName" : { "type" : "string"},
                         	"tagValue" : { "type" : "string"}
             		}
             }
         }
     }
 }
```
4. /{devices} PUT 메서드 선택 - 통합 요청 - 템플릿 생성 - 콘텐츠 유형: application/json - 템플릿 생성에서 UpdateWineInput 선택택 - 템플릿 본문에 다음과 같이 입력 후 생성  <br>
```
 #set($inputRoot = $input.path('$'))
 {
     "device": "$input.params('device')",
     "tags" : [
     ##TODO: Update this foreach loop to reference array from input json
         #foreach($elem in $inputRoot.tags)
         {
             "tagName" : "$elem.tagName",
             "tagValue" : "$elem.tagValue"
         } 
         #if($foreach.hasNext),#end
         #end
     ]
 }
```
6. 리소스에서 /devices/{devices} 선택 -> CORS 활성화 -> 모든 체크박스 선택 -> 저장  <br>
7. API 배포 - 새 스테이지 - prod - 배포  <br>


## 7-4. 디바이스 로그 조회
- Lambda Name: LogWineLambda  <br>
- version: java 11  <br>
-----
App.java Code
```
package helloworld;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Iterator;
import java.util.TimeZone;


import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;

import com.amazonaws.services.dynamodbv2.AmazonDynamoDB;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClientBuilder;
import com.amazonaws.services.dynamodbv2.document.DynamoDB;
import com.amazonaws.services.dynamodbv2.document.Item;
import com.amazonaws.services.dynamodbv2.document.ItemCollection;
import com.amazonaws.services.dynamodbv2.document.QueryOutcome;
import com.amazonaws.services.dynamodbv2.document.Table;
import com.amazonaws.services.dynamodbv2.document.spec.QuerySpec;
import com.amazonaws.services.dynamodbv2.document.utils.NameMap;
import com.amazonaws.services.dynamodbv2.document.utils.ValueMap;

/**
 * Handler for requests to Lambda function.
 */
public class App implements RequestHandler<Event, String> {
    private DynamoDB dynamoDb;
    private String DYNAMODB_TABLE_NAME = "WineTable";

    public String handleRequest(final Event input, final Context context) {

        this.initDynamoDbClient();
        Table table = dynamoDb.getTable(DYNAMODB_TABLE_NAME);

        long from=0;
        long to=0;
        try {
            SimpleDateFormat sdf = new SimpleDateFormat ( "yyyy-MM-dd HH:mm:ss");
            sdf.setTimeZone(TimeZone.getTimeZone("Asia/Seoul"));

            from = sdf.parse(input.from).getTime() / 1000;
            to = sdf.parse(input.to).getTime() / 1000;
        } catch (ParseException e1) {
            e1.printStackTrace();
        }

        QuerySpec querySpec = new QuerySpec()
                .withKeyConditionExpression("deviceId = :v_id and #t between :from and :to")
                .withNameMap(new NameMap().with("#t", "time"))
                .withValueMap(new ValueMap().withString(":v_id",input.device).withNumber(":from", from).withNumber(":to", to));

        ItemCollection<QueryOutcome> items=null;
        try {
            items = table.query(querySpec);
        }
        catch (Exception e) {
            System.err.println("Unable to scan the table:");
            System.err.println(e.getMessage());
        }
        String output = getResponse(items);

        return output;
    }

    private String getResponse(ItemCollection<QueryOutcome> items) {

        Iterator<Item> iter = items.iterator();
        String response = "{ \"data\": [";
        for (int i =0; iter.hasNext(); i++) {
            if (i!=0)
                response +=",";
            response += iter.next().toJSON();
        }
        response += "]}";
        return response;
    }

    private void initDynamoDbClient() {
        AmazonDynamoDB client = AmazonDynamoDBClientBuilder.standard().build();

        this.dynamoDb = new DynamoDB(client);
    }

}

class Event {
    public String device;
    public String from;
    public String to;
}

```
-json 예시 <br>
```
{ "device": "MyMy", "from":"2019-12-01 14:28:10", "to": "2023-12-09 22:13:58"}
```
-----
1. Lambda 함수 생성 후, AmazonDynamoDBFullAccess 권한 추가 -> 테스트  <br>
2. API Gateway에서 이전에 생성한 wine-api를 선택 -> /{device} 선택 - 리소스 생성 - 리소스 이름: log
3. 메서드 섹션 - 메소드 생성 - 메서드 유형: Get - 통합 유형에서 Lambda함수 *(LogWineLambda)* 선택
4. 메서드 실행 - 메서드 요청 - 메서드 요청 설정의 편집 - URL 쿼리 문자열 파라미터 - 이름에 from과 to입력 (필수옵션)
5. /log GET 메서드 선택 - 통합 요청 - 템플릿 생성 - 콘텐츠 유형: application/json - 템플릿 생성에서 UpdateWineInput 선 - 템플릿 본문에 다음과 같이 입력 후 생성  <br>
```
 {
   "device": "$input.params('device')",
   "from": "$input.params('from')",
   "to":  "$input.params('to')"
 }
```
6. 리소스에서 /devices/{devices}/log 선택 -> CORS 활성화 -> 모든 체크박스 선택 -> 저장  <br>
7. API 배포 - 새 스테이지 - prod - 배포  <br>



