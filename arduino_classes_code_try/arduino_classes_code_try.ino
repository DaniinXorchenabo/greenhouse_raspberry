#include <SoftwareSerial.h>
#include <StandardCplusplus.h>
#include <serstream>
#include <string>
//#include <vector>
#include <iterator>
#include <map>
#include "DHT.h" ;

using namespace std;

DHT dht(7, DHT11);
SoftwareSerial raspb(53, 51);
SoftwareSerial blut(52,50);

class PinControl{
  public:
    int pin;
    boolean pin_mode_l = LOW;
    boolean now_pin_mode;
    
    PinControl(int pin_now, boolean pin_mode_now, int over_pin_fill){ //конструктор класса
      //over_pin_fill = over_pin_fill;
      pin_mode_l = pin_mode_now;
      starting(pin_now);
    }
    
    PinControl(int pin_now, boolean pin_mode_now){ //конструктор класса
      pin_mode_l = pin_mode_now;
      starting(pin_now);
    }

    PinControl(int pin_now ){ //конструктор класса
      pin_mode_l = LOW;
      starting(pin_now);
    }

    PinControl(){}
    
    /*
    PinControl(int pin_now[], boolean pin_mode_now[], int _size){ //конструктор класса
      *pin_mode_l = new boolean[_size];
      pin = new int[_size];
      
      //pin_mode_h = new boolean[_size];
      //pin = {9,8,7,6,5};
      Serial.println("----------------");
      int i;
      for (i = 0; i <= _size; i++)
      {
        pin[i] = pin_now[i];
        pinMode(pin[i], OUTPUT);
        *pin_mode_l[i] = pin_mode_now[i];
        *pin_mode_h[i] = !pin_mode_now[i];
        //Serial.println(pin_now[i]);
      }
      for (i = 0; i <= _size; i++)
      {
       // pin[i] = pin_now[i];
        Serial.print(pin[i]);
        Serial.print(" ");
        Serial.print(*pin_mode_l[i]);
        Serial.print(" ");
        Serial.println(*pin_mode_h[i]);
      }
      Serial.println("----------------");

      /*
      
      len = sizeof(pin_now)/sizeof(int*);
      *pin_mode_l = new boolean[len];
      *pin_mode_h = new boolean[len];
      int i;
      //Serial.println(typeid(i).name());
      Serial.print("-------- ");
      Serial.println(len);
      //Serial.println(pin);
      for (i = 0; i < sizeof(pin_now)/sizeof(int); i++) {//for (auto pin: example) {
        Serial.println("-=-=");
        pinMode(pin[i], OUTPUT);
        pin_mode_l[i] = pin_mode_now[i];
        pin_mode_h[i] = !pin_mode_now[i];
      }
      pin_write(pin_mode_l);
      
    }
    */
    void edit_status_pin(bool status_pin){//постановка статуса пина
      edit_status_pin(status_pin, 1, false);
    }

    void edit_status_pin(bool status_pin, int now_priority){
      edit_status_pin(status_pin, now_priority, false);
    }
    
    void edit_status_pin(bool status_pin, int now_priority, bool save_priority){ //постановка статуса пина
      if (status_pin){
        pin_write_priority(!pin_mode_l, now_priority, save_priority);
      } else {
        is_work_for_time = false;
        pin_write_priority(pin_mode_l, now_priority, save_priority);
      }
    }
    
    void turn_on_for_time(int delay_t_now){
      Serial.println("turn_on_for_time 1");
      turn_on_for_time(delay_t_now, 1, false);
    }
    
    void turn_on_for_time(int delay_t_now, int now_priority){
      turn_on_for_time(delay_t_now, now_priority, false);
    }
    
    void turn_on_for_time(int delay_t_now, int now_priority, bool save_priority){
      Serial.println("turn_on_for_time 3");
      
      if (pin_write_priority(delay_t_now, now_priority, save_priority, false)){
        delay(10);
        //Serial.println("turn_on_for_time if 1");
       delay_t = delay_t_now;
        time_turn_on = millis();
        is_work_for_time = true;
        pin_write(!pin_mode_l); /**/
      }
      
    }
    
    void update_pin(){ //метод, который должен постоянно крутиться в loop
      //Serial.println("update");
      if (is_work_for_time){
        //Serial.println("if working");
        if (millis() - time_turn_on > delay_t){
          Serial.println("end_time_work");
          is_work_for_time = false;
          pin_write(pin_mode_l);
        }
      }
    }

    void PWM_mode(int filling){
      analog_write_pin(filling);
    }

    void set_priority(int now_priority){
      priority = now_priority;
    }

    int get_priority(){
      return priority;
    }
    
    int get_filling_pin(){
      return pin_filling;
    }
    
  private:
    unsigned long time_turn_on;
    bool is_work_for_time = false;
    int delay_t;
    int pin_filling = 0;
    //int over_pin_fill = 0;
    int priority = 0;

    void pin_write_priority(boolean pin_mod, int now_priority, bool save_priority){
      pin_write_priority(pin_mod, now_priority, save_priority, true);
    }
    
    bool pin_write_priority(boolean pin_mod, int now_priority, bool save_priority, bool isworking){
      if (now_priority > priority){
        Serial.println("now_priority > priority");
        if (isworking){
          pin_write(pin_mod);
        }
        if (save_priority){
          priority = now_priority;
        }
        return true;
      }
      return false;
    }
    
    void pin_write(boolean pin_mod){
      Serial.println("pin_write");
      if (now_pin_mode != pin_mod){
        Serial.print("if pin_write ");
        Serial.print(pin);
         Serial.print(" ");
        Serial.println(pin_mod);
        //int i;
        //for (i = 0; i < sizeof(pin); i++) {
          digitalWrite(pin, pin_mod);
          now_pin_mode = pin_mod;
        //}
      }
    }

    void analog_write_pin(int filling){
      if (pin_filling != filling){
        pin_filling = filling;
        analogWrite(pin, pin_filling);
      }
    }

    void starting(int pin_now){
      pin = pin_now;
      Serial.print("inicialization pin ");
      Serial.println(pin_now);
      pinMode(pin_now, OUTPUT);
      pin_write(pin_mode_l);
      
    }
};

class AnalogReadPin{
  public:

    AnalogReadPin(){}
    
    AnalogReadPin(int pin_now){
      pin = pin_now;
      pinMode(pin, INPUT);
    }

    AnalogReadPin(DHT &dht_new, String param_dht){
      *dht = dht_new;
      if (param_dht == "t"){
        how_param = true;
      }
      how_param = param_dht;
      DHT_flag = true;
    }

    int get_value(){
      if (DHT_flag){
        if (how_param){
          return (int)return_temperature();
        } else {
          return (int)return_humid();
        }
      } else {
        return (int)pin_analogRead();
      }
    }

    private:
      bool DHT_flag = false;
      bool how_param = false;
      int pin;
      DHT *dht;
      
      int pin_analogRead(){
        return (int)analogRead(pin);
      }

      int return_temperature(){
        return (int)dht->readTemperature();
      }

      int return_humid(){
        return (int)dht->readHumidity();
      }
};

struct sens_val_strucr{
  float value;
  AnalogReadPin cls;
};

std::map <String, PinControl> dig_pins;
std::map <String, sens_val_strucr> sensors_val;

class RaspberryPiControl{
  public:
    SoftwareSerial *raspb;
    String start_day = "None";
    //std::map <String, sens_val_strucr> * sensors = NULL; //проверить, что тут: локальные копии или ссылки
    //std::map <String, PinControl> * digit_pins = NULL;

    RaspberryPiControl(SoftwareSerial &ras){ //, std::map <String, sens_val_strucr> * sensors_new, std::map <String, PinControl> * pins_new
      raspb = &ras;
      //sensors = sensors_new;
      //digit_pins = pins_new;
      //pins_new["whiteLed"].edit_status_pin(true);
      raspb->begin(9600);
    }
    
    void raspb_update(){
      over_read_raspb();
    }

    void over_read_raspb(){ // считывается строка, полученная с разбери
      String raspb_data = read_raspb();
      if (raspb_data != ""){
        data_processing(raspb_data); 
      }
    }
        
    void data_processing(String str){ // обработка строки
      write_raspb("I get: " + str);
      str_raspb = str;
      if (str == "WhiteLedHIGH"){ 
        if (dig_pins.find("whiteLed") != dig_pins.end()){
          dig_pins.find("whiteLed")->second.edit_status_pin(true);
        }
      }
      else if (str == "WhiteLedLOW"){
        if (dig_pins.find("whiteLed") != dig_pins.end()){
          dig_pins.find("whiteLed")->second.edit_status_pin(false);
        }
      }
      else if (str == "polivHIGH"){
        if (dig_pins.find("poliv") != dig_pins.end()){
          dig_pins.find("poliv")->second.turn_on_for_time(3000);
          //get_digitPin_obj("poliv")->turn_on_for_time(3000);
        }
      }
      else if (str == "get_parametrs"){get_val_sens();}
      else if (str == "Give_start_day"){write_raspb("start_day " + start_day);}
      
      if (str_raspb != str){ 
      }
    }
    
    void test(){
      Serial.println("---");
      //Serial.println(digit_pins->find("test")->second.pin);
      //digit_pins->find("test")->second.turn_on_for_time(10000);
      sensors_val["gas"].value = 137.0;
    }
    
  private:
    String new_str_raspb = "";
    String str_raspb = "";

    void set_value_for_sensor_struct(String key, float value){
      if (sensors_val.find(key) !=sensors_val.end()){
        sensors_val.find(key)->second.value = 68;
      }
    }
    
    float get_value_for_sensor_struct(String key){
      if (sensors_val.find(key) !=sensors_val.end()){
        return sensors_val.find(key)->second.value;
      }
      return -1.0;
    }
    
    /*
    PinControl get_digitPin_obj(String key){
      if (digit_pins->find(key) != digit_pins->end()){
        return digit_pins->find(key)->second;
      }
      return digit_pins->find(key)->second;
    }
    */
    
    void write_raspb(String text){
      raspb->println(text);
    }

    String read_raspb(){
      if (raspb->available()){
        return (String)raspb->readString();
      }
      return "";
    }

    String get_val_sens(){
      String ret = "/";
      for(auto it = sensors_val.begin(); it != sensors_val.end(); ++it){
        ret += (String)it->first;
        ret += ":";
        ret += (String)it->second.value;
        ret += "/";
      }
      return ret;
    }

};
RaspberryPiControl rasClass(raspb);

class EspControl{
  public:
  
    EspControl(){}
    
    void update_esp(){
      regular_send(); //отправка данных на esp
      over_read(); //обработка данных, полученных от ESP
    }
    
  private:
    String esp_string = "";
    unsigned long time_turn_on;

    //=======! отправка данных на esp !=======
    void regular_send(){
      if (millis() - time_turn_on > 2000){
        time_turn_on = millis();
        esp_write(generate_answer());
      }
    }

    String generate_answer(){
      String answer;
      if (dig_pins.find("fitoLed") != dig_pins.end()){
        answer = "/" + (String)dig_pins["fitoLed"].get_filling_pin();
        answer = answer + answer + "/";
      } else {
        answer = "/no_date/no_date/";
      }
      answer += "/" + (String)return_sensor_val("temp") + "/" + (String)return_sensor_val("hum") + answer;
      return answer + "3/";
    }

    float return_sensor_val(String key){
      if (sensors_val.find(key) != sensors_val.end()){
        return sensors_val[key].value;
      }
      return 0.0;
    }

    void esp_write(String send_data){
      //Serial3.println(send_data);
    }
    
    //=======! прием данных с ESP !=======

    void over_read(){
      String new_str = esp_read();
      if (new_str != ""){
        data_processing(new_str);
      }
    }

    void data_processing(String priem_c_ESP_str){

      if (priem_c_ESP_str == "3") {//свет включить
        if (dig_pins.find("fitoLed") != dig_pins.end()){
          dig_pins["fitoLed"].edit_status_pin(true, 50, true);
        }
      }
      if (priem_c_ESP_str == "2") {//свет выключить
        if (dig_pins.find("fitoLed") != dig_pins.end()){
          dig_pins["fitoLed"].edit_status_pin(false, 50, true);
        }
      }
      if (priem_c_ESP_str == "7") {/*digitalWrite(40,HIGH);   reset_esp = 1;*/} //resrt
      if (priem_c_ESP_str == "4") {  //полив
        if (dig_pins.find("poliv") != dig_pins.end()){
          dig_pins["poliv"].turn_on_for_time(3000, 50);
        }
      } 
      if (priem_c_ESP_str == "5") {} //включить авто
      if ((priem_c_ESP_str == "6")) {reset_priority();} //выключить авто
    }

    void reset_priority(){
      for(auto it = dig_pins.begin(); it != dig_pins.end(); ++it){
        int prt = it->second.get_priority();
        if (prt >= 50 && prt < 100){
          it->second.set_priority(prt - 50);
        }
      }
    }
 
    String esp_read(){
      /*
      if (Serial3.available()){
        return Serial3.readString();
      }
      return "";*/
    }

};

class Bluetooth{
  public:
    SoftwareSerial *blut;

    Bluetooth(SoftwareSerial &blut_obj){
      blut = &blut_obj;
      blut->begin(9600);
    }

    void update_blut(){
      tolking_with_android();
    }

  private:

    void tolking_with_android(){
      over_blut_read();
    }
    
    String blut_read(){
      if (blut->available()){
        return (String)blut->readString();
      }
      return "";
    }

    void over_blut_read(){
      String blut_data = blut_read();
      if (blut_data != ""){
        data_processing(blut_data); 
      }
    }

    void data_processing(String blut_data){
      if (blut_data.substring(0,8) == "set_day:"){
        String start_day = blut_data.substring(9);
      }
      else if (blut_data.substring(0,1) == "/"){
        
      }
    }

    String parser_data(String divider){
      //int count = 
      
    }
    
    void blut_write(String messenge){
      blut->println(messenge);
    }

    String generate_answer(){
      
    }
    
};




Bluetooth bluetooth(blut);


void setup(){
  dht.begin();
  Serial.begin(9600);
Serial.println("rtrtr");
String anssss = "000000";
Serial.println(anssss.indexOf("9-")); // работает
Serial.println("rtrtr");

 // test = (sens_val_strucr){700, AnalogReadPin(0)};
  
  //dig_pins["whiteLed"] = PinControl(9);
  //dig_pins["fitoLed"] = PinControl(10);
  //dig_pins["poliv"] = PinControl(24); 
  //dig_pins["fan_root"] = PinControl(4);//4
  //dig_pins["air"] = PinControl(30);
  //dig_pins["fan_air"] = PinControl(8);
  //dig_pins["vapor"] = PinControl(12);
  //dig_pins["fan_vapor"] = PinControl(26);
  dig_pins["test"] = PinControl(13, LOW);
  //dig_pins["test"].turn_on_for_time(10000);


  sensors_val["temp"] = (sens_val_strucr){88, AnalogReadPin(dht, "t")};
  sensors_val["hum"] = (sens_val_strucr){88, AnalogReadPin(dht, "h")};
  sensors_val["gas"] = (sens_val_strucr){88, AnalogReadPin(1)};
  
  Serial.println("rtrtr ");
  rasClass.test();
  Serial.println("rtrtr ");
  Serial.println(sensors_val["gas"].value);

}

void loop(){
  update_pin();
  rasClass.raspb_update();
  //update_sensors_value();
  delay(500);
  Serial.println("looping.....");
}

void update_pin(){
  for(auto it = dig_pins.begin(); it != dig_pins.end(); ++it){
    //Serial.println((String)it->first);
    //Serial.println("--------------------------------");
    it->second.update_pin();
  }
}
/**/
/*
void update_sensors_value(){
  for(auto it = sensors_val.begin(); it != sensors_val.end(); ++it){
    //sensors_val[(String)it->first].value = it->second.cls.get_value();
    it->second.value = it->second.cls.get_value();
  }
}*/

