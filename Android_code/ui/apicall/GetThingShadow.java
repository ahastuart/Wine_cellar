package com.example.android_resapi.ui.apicall;

import android.app.Activity;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import org.json.JSONException;
import org.json.JSONObject;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import com.example.android_resapi.R;
import com.example.android_resapi.httpconnection.GetRequest;
import com.example.android_resapi.ui.DeviceActivity;

public class GetThingShadow extends GetRequest {
    final static String TAG = "AndroidAPITest";
    String urlStr;
    boolean startBtnPressed;
    public GetThingShadow(Activity activity, String urlStr, boolean startBtnPressed) {
        super(activity);
        this.urlStr = urlStr;
        this.startBtnPressed = startBtnPressed;
    }

    @Override
    protected void onPreExecute() {
        try {
            Log.e(TAG, urlStr);
            url = new URL(urlStr);

        } catch (MalformedURLException e) {
            Toast.makeText(activity,"URL is invalid:"+urlStr, Toast.LENGTH_SHORT).show();
            e.printStackTrace();
            activity.finish();
        }
    }

    @Override
    protected void onPostExecute(String jsonString) {
        if (jsonString == null)
            return;
        Map<String, String> state = getStateFromJSONString(jsonString);

        //Log.e("STARTBTNPRESSED", String.valueOf(startBtnPressed));
        if (!startBtnPressed) {
            //Log.e("ACTIVITY", String.valueOf(activity));
            String reported_inventory = state.get("reported_inventory");
            String reported_inventory3 = state.get("reported_inventory3");

            if (reported_inventory != null && Integer.parseInt(reported_inventory) <= 15) {
                Toast.makeText(activity, "스파클링 와인 재고 부족 !", Toast.LENGTH_SHORT).show();
            }

            if (reported_inventory3 != null && Integer.parseInt(reported_inventory3) <= 15) {
                Toast.makeText(activity, "레드 와인 재고 부족 !", Toast.LENGTH_SHORT).show();
            }
        }

        TextView reported_tempTV = activity.findViewById(R.id.reported_temperature);
        TextView reported_humTV = activity.findViewById(R.id.reported_humidity);
        TextView reported_temp3TV = activity.findViewById(R.id.reported_temperature3);
        TextView reported_hum3TV = activity.findViewById(R.id.reported_humidity3);
        TextView reported_invenTV = activity.findViewById(R.id.reported_inventory);
        TextView reported_inven3TV = activity.findViewById(R.id.reported_inventory3);
        TextView reported_ledTV = activity.findViewById(R.id.reported_LED);
        TextView reported_led3TV = activity.findViewById(R.id.reported_LED3);

        reported_tempTV.setText(state.get("reported_temperature"));
        reported_humTV.setText(state.get("reported_humidity"));
        reported_temp3TV.setText(state.get("reported_temperature3"));
        reported_hum3TV.setText(state.get("reported_humidity3"));
        reported_invenTV.setText(state.get("reported_inventory"));
        reported_inven3TV.setText(state.get("reported_inventory3"));
        reported_ledTV.setText(state.get("reported_LED"));
        reported_led3TV.setText(state.get("reported_LED3"));

        TextView desired_tempTV = activity.findViewById(R.id.desired_temperature);
        TextView desired_humTV = activity.findViewById(R.id.desired_humidity);
        TextView desired_temp3TV = activity.findViewById(R.id.desired_temperature3);
        TextView desired_hum3TV = activity.findViewById(R.id.desired_humidity3);
        TextView desired_invenTV = activity.findViewById(R.id.desired_inventory);
        TextView desired_inven3TV = activity.findViewById(R.id.desired_inventory3);
        TextView desired_ledTV = activity.findViewById(R.id.desired_LED);
        TextView desired_led3TV = activity.findViewById(R.id.desired_LED3);

        desired_tempTV.setText(state.get("desired_temperature"));
        desired_humTV.setText(state.get("desired_humidity"));
        desired_temp3TV.setText(state.get("desired_temperature3"));
        desired_hum3TV.setText(state.get("desired_humidity3"));
        desired_invenTV.setText(state.get("desired_inventory"));
        desired_inven3TV.setText(state.get("desired_inventory3"));
        desired_ledTV.setText(state.get("desired_LED"));
        desired_led3TV.setText(state.get("desired_LED3"));

//        TextView desired_ledTV = activity.findViewById(R.id.desired_led);
//        TextView desired_tempTV = activity.findViewById(R.id.desired_temp);
//        desired_tempTV.setText(state.get("desired_temperature"));
//        desired_ledTV.setText(state.get("desired_LED"));

    }

    protected Map<String, String> getStateFromJSONString(String jsonString) {
        Map<String, String> output = new HashMap<>();
        try {
            // 처음 double-quote와 마지막 double-quote 제거
            jsonString = jsonString.substring(1,jsonString.length()-1);
            // \\\" 를 \"로 치환
            jsonString = jsonString.replace("\\\"","\"");
            Log.i(TAG, "jsonString="+jsonString);
            JSONObject root = new JSONObject(jsonString);
            JSONObject state = root.getJSONObject("state");
            JSONObject reported = state.getJSONObject("reported");
            String tempValue = reported.getString("temperature");
            String humValue = reported.getString("humidity");
            String temp3Value = reported.getString("temperature3");
            String hum3Value = reported.getString("humidity3");
            String invenValue = reported.getString("inventory");
            String inven3Value = reported.getString("inventory3");
            String ledValue = reported.getString("LED");
            String led3Value = reported.getString("LED3");
            output.put("reported_temperature", tempValue);
            output.put("reported_humidity",humValue);
            output.put("reported_temperature3", temp3Value);
            output.put("reported_humidity3", hum3Value);
            output.put("reported_inventory", invenValue);
            output.put("reported_inventory3", inven3Value);
            output.put("reported_LED", ledValue);
            output.put("reported_LED3",led3Value);

            JSONObject desired = state.getJSONObject("desired");
            String d_tempValue = desired.getString("temperature");
            String d_humValue = desired.getString("humidity");
            String d_temp3Value = desired.getString("temperature3");
            String d_hum3Value = desired.getString("humidity3");
            String d_invenValue = desired.getString("inventory");
            String d_inven3Value = desired.getString("inventory3");
            String d_ledValue = desired.getString("LED");
            String d_led3Value = desired.getString("LED3");
            output.put("desired_temperature", d_tempValue);
            output.put("desired_humidity",d_humValue);
            output.put("desired_temperature3", d_temp3Value);
            output.put("desired_humidity3", d_hum3Value);
            output.put("desired_inventory", d_invenValue);
            output.put("desired_inventory3", d_inven3Value);
            output.put("desired_LED", d_ledValue);
            output.put("desired_LED3", d_led3Value);
//            String desired_tempValue = desired.getString("temperature");
//            String desired_ledValue = desired.getString("LED");
//            output.put("desired_temperature", desired_tempValue);
//            output.put("desired_LED",desired_ledValue);

        } catch (JSONException e) {
            Log.e(TAG, "Exception in processing JSONString.", e);
            e.printStackTrace();
        }
        return output;
    }
}
