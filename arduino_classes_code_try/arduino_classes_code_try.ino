#include <SoftwareSerial.h>
#include <StandardCplusplus.h>
#include <serstream>
#include <string>
//#include <vector>
#include <iterator>
#include <map>
#include "DHT.h" 

using namespace std;

DHT dht(7, DHT11);

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
        analogWrite(pin, filling);
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
    std::map <String, sens_val_strucr> * sensors = NULL; //проверить, что тут: локальные копии или ссылки
    std::map <String, PinControl> * digit_pins = NULL;

    RaspberryPiControl(SoftwareSerial &ras, std::map <String, sens_val_strucr> * sensors_new, std::map <String, PinControl> &pins_new){
      raspb = &ras;
      sensors = sensors_new;
      digit_pins = pins_new;
      pins_new["whiteLed"].edit_status_pin(true);
      raspb->begin(9600);
    }

    void for_debug(std::map <String, sens_val_strucr>& sensors_new){
      //sensors = sensors_new;
      sensors_new["gas"].value = 96;
    }
    
    void raspb_update(){
      over_read_raspb();
    }

    void over_read_raspb(){
      String raspb_data = read_raspb();
      if (raspb_data != ""){
        data_processing(raspb_data);
      }
    }
        
    void data_processing(String str){
      write_raspb("I get: " + str);
      str_raspb = str;
      if (str == "WhiteLedHIGH"){ 
        if (dig_pins.find("whiteLed") != dig_pins.end()){
          digit_pins["whiteLed"].edit_status_pin(true);
        }
      }
      else if (str == "WhiteLedLOW"){
        if (dig_pins.find("whiteLed") != dig_pins.end()){
          digit_pins["whiteLed"].edit_status_pin(false);
        }
      }
      else if (str == "polivHIGH"){
        if (dig_pins.find("poliv") != dig_pins.end()){
          digit_pins["poliv"].turn_on_for_time(3000);
        }
      }
      else if (str == "get_parametrs"){get_val_sens();}
      if (str_raspb != str){ 
      }
    }
    
    void test(){
      Serial.print("test----");
      Serial.println(digit_pins["test"].pin);
      digit_pins["test"].turn_on_for_time(10000);
    }
    
  private:
    String new_str_raspb = "";
    String str_raspb = "";

    void set_value_for_sensor_struct(String key, float value){
      if (sensors->find(key) !=sensors->end()){
        sensors->find(key)->second.value = 68;
      }
    }
    
    float get_value_for_sensor_struct(String key){
      if (sensors->find(key) !=sensors->end()){
        return sensors->find(key)->second.value;
      }
      return -1.0;
    }
    
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




SoftwareSerial raspb(52, 50);
RaspberryPiControl rasClass(raspb, &sensors_val, dig_pins);

void setup(){
  dht.begin();
  Serial.begin(9600);
Serial.println("rtrtr");


 // test = (sens_val_strucr){700, AnalogReadPin(0)};
  
  //dig_pins["whiteLed"] = PinControl(9);
  //dig_pins["fitoLed"] = PinControl(10);
  //dig_pins["poliv"] = PinControl(24); 
  //dig_pins["fan_root"] = PinControl(4);//4
  //mp["air"] = PinControl(30);
  //mp["fan_air"] = PinControl(8);
  //dig_pins["vapor"] = PinControl(12);
  //dig_pins["fan_vapor"] = PinControl(26);
  //dig_pins["test"] = PinControl(13, LOW);
  //dig_pins["test"].turn_on_for_time(10000);

/*
  if (dig_pins.find("test") != dig_pins.end()){
    Serial.println("-=-=-=-=-=-=-");
  }
    if (dig_pins.find("test00000") != dig_pins.end()){
    Serial.println("-=-=-=-=-=-=- 333");
  }
  */
  //sensors_val["poliv_levle"] = (sens_val_strucr){700, AnalogReadPin(0)};
  sensors_val["gas"] = (sens_val_strucr){88, AnalogReadPin(1)};
  
  //sensors_val["vapor_levle"] = (sens_val_strucr){700, AnalogReadPin(2)};
  //sensors_val["humid"] = {69, AnalogReadPin(dht, "h")};
  //sensors_val["temper"] = (sens_val_strucr){23, AnalogReadPin(dht, "t")};
  
  //sensors_val["humid"].cls.test();
  Serial.println("rtrtr ");
  //Serial.println(dig_pins["test"].pin);
  //dig_pins["test"].turn_on_for_time(3000);
  Serial.println(sensors_val["gas"].value);
  rasClass.for_debug();
  sensors_val["gas"].value = 49;
  rasClass.for_debug();
  rasClass.for_debug1();
  Serial.println("----");
  Serial.println(sensors_val["gas"].value);
  //Serial.println(*rasClass.sensors["gas"].value);
  /*Serial.println(sensors_val["gas"].value);
  Serial.println(rasClass.sensors["gas"].value);
  
  rasClass.for_debug(sensors_val);
  Serial.println(rasClass.sensors["gas"].value);
  Serial.println(sensors_val["gas"].value);
  */
}

void loop(){
  //update_pin();
  /*
  update_sensors_value();
  Serial.println(sensors_val["gas"].value);
  Serial.println(sensors_val["poliv_levle"].value);
  Serial.println(sensors_val["vapor_levle"].value);
  */
  //Serial.println(dht.readTemperature());
  //Serial.println();

  delay(1000);  
}
/*
void update_pin(){
  for(auto it = dig_pins.begin(); it != dig_pins.end(); ++it){
    //Serial.println((String)it->first);
    //Serial.println("--------------------------------");
    it->second.update_pin();
  }
}
*/
/*
void update_sensors_value(){
  for(auto it = sensors_val.begin(); it != sensors_val.end(); ++it){
    //sensors_val[(String)it->first].value = it->second.cls.get_value();
    it->second.value = it->second.cls.get_value();
  }
}*/

