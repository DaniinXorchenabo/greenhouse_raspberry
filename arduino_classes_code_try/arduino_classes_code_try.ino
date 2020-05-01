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

int POLIV_DELAY = 10000;
int POLIV_LOW_LEVEL = 30;
int8_t VAPOR_ACTIVATION_LEVLE = 60;
int8_t VAPOR_DISACTIVATION_LEVLE = VAPOR_ACTIVATION_LEVLE + 20;
int8_t GAS_ACTIVATION_LEVLE = 40;
int8_t GAS_DISACTIVATION_LEVLE = GAS_ACTIVATION_LEVLE - 15;

class ActivateWork{
  public:
    String key = "";
    String key_senser = "";
    int low_level = 50;
    int high_level = 50;
    int8_t how_func = 1;

    ActivateWork(){}
    ActivateWork(String key_n, String key_senser_n, int low_level_n, int high_level_n){
      key = key_n;
      key_senser = key_senser_n;
      low_level = low_level_n;
      high_level = high_level_n;
    }
    ActivateWork(String key_n, String key_senser_n, int low_level_n, int high_level_n, int8_t how_func_n){
      key = key_n;
      key_senser = key_senser_n;
      low_level = low_level_n;
      high_level = high_level_n;
      how_func = how_func_n;
    }

    void activate_change(){
      activate_change(how_func, key);
    }
    void activate_change(int8_t how_func_n){
      activate_change(how_func_n, key);
    }
    void activate_change_key(String key_n){
      activate_change(how_func, key_n);
    }
    void activate_change(int8_t how_func_n, String key_n){
      if (how_func_n == 1){
        standart_activation_func(key_n);
      } else if (how_func_n == 2){
        revers_standart_activation_func(key_n);
      } else if (how_func_n == 3){
        poliv_activation_func(key_n);
      } else if (how_func_n == 100){
        test_activ_func();
      }
    }
    
    void standart_activation_func(){
      standart_activation_func(key);
    }
    void standart_activation_func(String key_n);

    void revers_standart_activation_func(String key_n);

    void poliv_activation_func(String key_n);
    
    void test_activ_func();
};

class PinControl{
  public:
    int8_t pin = 43;
    boolean pin_mode_l = LOW;
    boolean now_pin_mode = pin_mode_l;
    //void (*activate_func_from_out)(String) = NULL;
    ActivateWork *activate_work = NULL;

    PinControl(int pin_now, ActivateWork &activate_work_n){ //конструктор класса , bool useless1, bool useless2 , void (*func)(String)
      //activate_func_from_out = func;
      activate_work = &activate_work_n;
      pin_mode_l = LOW;
      starting(pin_now);
    }
    PinControl(int pin_now, boolean pin_mode_now, ActivateWork &activate_work_n){ //конструктор класса
      //activate_func_from_out = func;
      activate_work = &activate_work_n;
      pin_mode_l = pin_mode_now;
      //Serial.println("************************ " + (String)activate_work->how_func);
      starting(pin_now);
    }
    PinControl(int pin_now, boolean pin_mode_now, int over_pin_fill, ActivateWork &activate_work_n){ //конструктор класса
      //activate_func_from_out = func;
      //over_pin_fill = over_pin_fill;
      activate_work = &activate_work_n;
      
      pin_mode_l = pin_mode_now;
      starting(pin_now);
    }
    
    PinControl(int pin_now, boolean pin_mode_now, int over_pin_fill){ //конструктор класса
      //over_pin_fill = over_pin_fill;
      pin_mode_l = pin_mode_now;
      starting(pin_now);
    }
    PinControl(int pin_now, boolean pin_mode_now){ //конструктор класса
      pin_mode_l = pin_mode_now;
      starting(pin_now);
    }
    PinControl(int pin_now){ //конструктор класса
      pin_mode_l = LOW;
      starting(pin_now);
    }

    PinControl(){}
    
    void run_activation_func(String key){
      if (activate_work != NULL){
        activate_work->activate_change_key(key);
      } else {
         //Serial.println("NULL activate func");
      }
    }

    static void static_edit_status_pin(String key, bool status_pin, int now_priority, bool save_priority);

    static void static_turn_on_for_time(String key, int delay_t_now, int now_priority, bool save_priority);
    
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
    
    void update_pin(String key){ //метод, который должен постоянно крутиться в loop
      run_activation_func(key);
      if (is_work_for_time){
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
  
    void add_priority(int t){}
    
    void set_priority(int now_priority){
      if (now_priority < 40){
        int kostil = 0;
        if (now_priority > 9){
          kostil = priority % (int)my_pow(10, now_priority/10 + 1) / (int)my_pow(10, now_priority/10); 
          kostil = (int)my_pow(10, now_priority/10) * kostil;
          priority -= kostil;
        } else {
          priority -= priority % (int)my_pow(10, now_priority/10 + 1);
        }
        //Serial.println("&&&&&&&%$$$$________" + (String)priority);
        kostil = (int)my_pow(10, (int)(now_priority/10));
        kostil *= now_priority % 10;
        priority += kostil;
      }
    }

    int get_priority(){
      return priority;
    }
    
    int get_filling_pin(){
      return pin_filling;
    }

    bool get_working_pin(){ // возвращает 1 если пин в режиме работы, 0 - если пин не работает
      return (bool)(pin_mode_l != now_pin_mode);
    }
    
  private:
    unsigned long time_turn_on;
    bool is_work_for_time = false;
    int delay_t;
    int pin_filling = 0;
    //int over_pin_fill = 0;
    int priority = 10000; // любое число|с сайта|с мобильного приложения|с разбери|с датчиков или по времени|

    void pin_write_priority(boolean pin_mod, int now_priority, bool save_priority){
      pin_write_priority(pin_mod, now_priority, save_priority, true);
    }
    
    bool pin_write_priority(boolean pin_mod, int now_priority, bool save_priority, bool isworking){
      //now_priority = |номер ячейки в строке| значение этой ячейки|
      if (levle_priority(now_priority)){
        //Serial.print("now_priority >= priority " + String(pin) + "\t");
        if (isworking){
          pin_write(pin_mod);
        }
        return true;
      }
      return false;
    }
    
    void pin_write(boolean pin_mod){
      //Serial.println("pin_write, if " +(String)now_pin_mode + " != " + (String)pin_mod);
      if (now_pin_mode != pin_mod){
        //Serial.println("if pin_write " + (String)pin + " " + (String)pin_mod);
        digitalWrite(pin, pin_mod);
        now_pin_mode = pin_mod;
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

    int levle_priority(int got_priority){
      got_priority =  (int)pow(10, got_priority / 10) * got_priority % 10;
      if (priority % 10000 <= got_priority){
        return true;
      }
    }

    int my_pow(int num, int _step){
      int new_num = 1;
      for (int i = 0; i < _step; ++i){
        new_num *= num;
      }
      return new_num;
    }

};

class AnalogReadPin{
  public:

    AnalogReadPin(){}

    AnalogReadPin(int pin_now, bool def_n){
      pin = pin_now;
      def = def_n;
      pinMode(pin, INPUT);
    }
    
    AnalogReadPin(int pin_now){
      pin = pin_now;
      pinMode(pin, INPUT);
    }

    AnalogReadPin(DHT &dht_new, String param_dht){
      Serial.println("===8=0=9==-0=");
      *dht = dht_new;
      if (param_dht == "t"){
        how_param = true;
      } else {
        how_param = false;
      }
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
      bool def = false;
      int8_t pin;
      DHT *dht;
      
      int pin_analogRead(){
        if (def){
          return (int)-1;
        }
        return (int)analogRead(pin);
      }

      int return_temperature(){
        int data = (int)dht->readTemperature();
        if (data != 0){
          return data;
        }
        return (int)-1;
      }

      int return_humid(){
        int data = (int)dht->readHumidity();
        if (data != 0){
          return data;
        }
        return (int)-1;
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

    RaspberryPiControl(SoftwareSerial &ras){
      raspb = &ras;
      raspb->begin(9600);
      write_raspb("I started (uno)");
    }
    
    void raspb_update(){
      over_read_raspb();
      regular_send();
    }
        

  private:
    String new_str_raspb = "";
    String str_raspb = "";
    unsigned long time_turn_on;

    
    
    //=======! чтение с распберри !=======
    void over_read_raspb(){ // считывается строка, полученная с разбери
      while (raspb->available() > 0){
        String raspb_data = read_raspb();
        if (raspb_data != ""){
          raspb_data.trim();
          data_processing(raspb_data); 
        }
      }
    }
    
    String read_raspb(){
      Serial.print("-");
      
      if (!raspb->isListening()){
        bool useless = raspb->listen();
      }
      
      if (raspb->available()){
        Serial.println("разбери что-то прислал...");
        String data = "";
        data = raspb->readString();
        /* 
        while (raspb->available() > 0){
          data += (String)raspb->peek();
          if (raspb->read() == '\n'){break;}
        }*/
        //delay(50);
        Serial.println("raspb_get: " + data);
        return data;
      }
      return "";
    }

    //=======! Обработка полученных данных !=======
    void data_processing(String str){ // обработка строки
      write_raspb("I get: " + str);
      Serial.println("Получил от разбери: " + str);
      str_raspb = str;
      if (str == "WhiteLedHIGH"){ 
        if (dig_pins.find("whiteLed") != dig_pins.end()){
          dig_pins.find("whiteLed")->second.edit_status_pin(true, 11, true);
        }
      }
      else if (str == "WhiteLedLOW"){
        if (dig_pins.find("whiteLed") != dig_pins.end()){
          dig_pins.find("whiteLed")->second.edit_status_pin(false, 11, true);
        }
      }
      else if (str == "polivHIGH"){
        if (dig_pins.find("poliv") != dig_pins.end()){
          dig_pins.find("poliv")->second.turn_on_for_time(POLIV_DELAY, 11);
        }
      }
      else if (str == "get_parametrs"){write_raspb(get_val_sens());}
      else if (str == "Give_start_day"){write_raspb("start_day " + start_day);}
      
      if (str_raspb != str){ 
      }
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

    //=======! отправление данеых !=======
    void regular_send(){
      if (millis() - time_turn_on > 2000){
        time_turn_on = millis();
        write_raspb("something");
      }
    }
    
    void write_raspb(String text){
      raspb->println(text);
      time_turn_on = millis();
    }

};
RaspberryPiControl rasClass(raspb);

class EspControl{
  public:
  
    EspControl(){
      Serial3.begin(9600); //115200
      Serial3.flush();
    }
    
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
        answer = "/" + (String)dig_pins.find("fitoLed")->second.get_filling_pin();
        answer = answer + answer + "/";
      } else {
        answer = "/no_date/no_date/";
      }
      answer = "/" + (String)(int)return_sensor_val("temp") + "/" + (String)(int)return_sensor_val("hum") + answer;
      return answer + "3/";
    }

    int return_sensor_val(String key){
      if (sensors_val.find(key) != sensors_val.end()){
        return (int)sensors_val.find(key)->second.value;
      }
      return 0;
    }

    void esp_write(String send_data){
      Serial.println("send to esp: " + send_data);
      Serial3.println(send_data);
    }
    
    //=======! прием данных с ESP !=======

    void over_read(){
      String new_str = esp_read();
      Serial.println("-=-=-="+new_str);
      if (new_str != ""){
        new_str += "---";
        new_str  = new_str.substring(0, 1);
        Serial.println("esp get^ " + new_str);
        data_processing(new_str);
      }
    }

    void data_processing(String priem_c_ESP_str){

      if (priem_c_ESP_str == "3") {//свет включить
        if (dig_pins.find("fitoLed") != dig_pins.end()){
          dig_pins.find("fitoLed")->second.edit_status_pin(true, 31, true);
        }
      }
      if (priem_c_ESP_str == "2") {//свет выключить
        if (dig_pins.find("fitoLed") != dig_pins.end()){
          dig_pins.find("fitoLed")->second.edit_status_pin(false, 31, true);
        }
      }
      if (priem_c_ESP_str == "7") {/*digitalWrite(40,HIGH);   reset_esp = 1;*/} //resrt
      if (priem_c_ESP_str == "4") {  //полив
        if (dig_pins.find("poliv") != dig_pins.end()){
          dig_pins.find("poliv")->second.turn_on_for_time(POLIV_DELAY, 31);
        }
      } 
      if (priem_c_ESP_str == "5") {} //включить авто
      if ((priem_c_ESP_str == "6")) {reset_priority();} //выключить авто
    }

    void reset_priority(){
      for(auto it = dig_pins.begin(); it != dig_pins.end(); ++it){
        it->second.set_priority(30);
      }
    }
 
    String esp_read(){
      if (Serial3.available() > 0){
        Serial3.setTimeout(50);
        String data = (String)Serial3.readString();
        delay(5);
        Serial3.flush();
        return data;
      }
      return "";
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

    void test(String data){
      parser_data(data, "/");
    }
    
  private:
    bool is_last_mode_avto = true; //чтобы каждый раз не перезаписывать приоритет пинов

    void tolking_with_android(){
      over_blut_read();
    }

    //=======! Чтение !=======
    void over_blut_read(){
      String blut_data = blut_read();
      if (blut_data != ""){
        data_processing(blut_data); 
      }
    }

    String blut_read(){
      //blut->listen();
      if (blut->available()){
        String date = (String)blut->readString();
        return date;
      }
      return "";
    }

    //=======! Обработка полученных данных !=======
    void data_processing(String blut_data){
      if (blut_data.substring(0,8) == "set_day:"){
        String start_day = blut_data.substring(9);
      }
      else if (blut_data.substring(0,1) == "/"){
        if (get_segment_for_ans(blut_data, "/", 1) == "1"){  
          // работаем в ручном реживе (-1 в автоматическом)
          is_last_mode_avto = false;
          parser_data(blut_data, "/");
        } else if (get_segment_for_ans(blut_data, "/", 1) == "-1"){
          //работаем в автоматическом режиме
          if (!is_last_mode_avto){
            reset_priority();
            is_last_mode_avto = true;
          }
        }
      }
    }

    void reset_priority(){
      for(auto it = dig_pins.begin(); it != dig_pins.end(); ++it){
        it->second.set_priority(20);
      }
    }

    String get_segment_for_ans(String data, String divider, int number){
      int first_ind = -1;
      int second_ind = data.indexOf(divider);
      data.trim();
      data += ">>>";
      int len_data = data.length();
      int count = 0;
      while (second_ind != -1){
        if (second_ind == -1 || len_data < second_ind || len_data <= first_ind){
          break;
        }
        ++count;
        if (count == number){
          return data.substring(first_ind + 1, second_ind);
        }
        first_ind = second_ind;
        second_ind = data.indexOf(divider, first_ind + 1);
      } 
      return (String)"";
    }

    void parser_data(String data, String divider){
      //ручной режим работы
      int first_ind = -1;
      int second_ind = data.indexOf(divider);
      int counter = 0;
      data.trim();
      data += ">>>";
      int len_data = data.length();
      while (second_ind != -1){
        if (second_ind == -1 || len_data < second_ind || len_data <= first_ind){
          break;
        }
        String sub_str = data.substring(first_ind + 1, second_ind);
        processing_data_by_one(counter, sub_str);
        first_ind = second_ind;
        second_ind = data.indexOf(divider, first_ind + 1);
        ++counter;
      } 
    }

    void processing_data_by_one(int number_data, String status_pin){
      if (number_data == 0) {
        //это статус авто или нет
      } else if (number_data == 1) {
        //нашреватель
      } else if (number_data == 2) {
        //охлодитель
      } else if (number_data == 3) {
        //вентилятор на проветривание
      } else if (number_data == 4) {
        //вентилятор корней
      } else if (number_data == 5) {
        //полив
      } else if (number_data == 6) {
        //испаритель
      }
      
    }

    
    //=======! Отправка данных обратоно !=======
    void blut_write(String messenge){
      blut->println(messenge);
    }

    String generate_answer(){
      //answer = "нагреватель охлодитель вентилятор_на_проветривание вентилятор_корней полив испаритель"
      String answer = "-1 -1 " + formating_status_pin("fan_air") + " ";
      answer += formating_status_pin("fan_root") + " -1 ";
      return answer + formating_status_pin("vapor");
    }

    String formating_status_pin(String key){
      bool _status = get_status_pin(key);
      if (_status == true){
        return String("1");
      }
      return String("-1");
    }

    bool get_status_pin(String key){
      if (dig_pins.find(key) != dig_pins.end()){
        return (bool)dig_pins[key].get_working_pin();
      }
      return false;
    }

    //=======! Изменение статуса пина (включение или выключение) !=======
    void set_status_pin_for_str(String key, String _st){
      if (dig_pins.find(key) != dig_pins.end()){
        if (_st == "1"){
          //включить
          set_status_pin(key, true, true);
        } else if (_st == "-1"){
          //выключить
          set_status_pin(key, false, true);
        }
        if (_st != "3" && _st != "0"){}
      }
    }
    
    void set_status_pin(String key, bool working){
      set_status_pin(key, working, true);
    }

    void set_status_pin(String key, bool working, bool save_priority){
      if (dig_pins.find(key) != dig_pins.end()){
        if (key != "poliv"){
          dig_pins.find(key)->second.edit_status_pin(working, 21, save_priority);
        } else {
          dig_pins.find(key)->second.turn_on_for_time(POLIV_DELAY, 21);
        }
      }
    }
    
};

/*
по функции активации:
  - по датчику или по времени
  - по разпберри
  - по мобильному приложению
  - по сайту
*/

EspControl espControl;
//Bluetooth bluetooth(blut);

ActivateWork activate_vapor("vapor", "hum", VAPOR_ACTIVATION_LEVLE, VAPOR_DISACTIVATION_LEVLE, 1); //и для испарителя и для его вентилятора
ActivateWork activate_root_fan("fan_root", "gas", GAS_DISACTIVATION_LEVLE, GAS_ACTIVATION_LEVLE, 2);
ActivateWork activate_poliv("poliv", "root_hum", POLIV_LOW_LEVEL, 0, 3);
ActivateWork activate_test("test", "test", 1, 2, 100);



String serial_data = "";
void setup(){
  //dht.begin();

  Serial.begin(9600);
  dht.begin();
  Serial.println("^trtr");
  Serial.println("free memory: " + (String)memoryFree());
  
  dig_pins["whiteLed"] = PinControl(8, LOW);
  dig_pins["fitoLed"] = PinControl(9, LOW);

  
  //dig_pins["air"] = PinControl(30);
  //dig_pins["fan_air"] = PinControl(33);
  dig_pins["vapor"] = PinControl(30, activate_vapor);
  dig_pins["fan_root"] = PinControl(28, activate_root_fan);//4
  dig_pins["fan_vapor"] = PinControl(26, activate_vapor);
  dig_pins["poliv"] = PinControl(24, activate_poliv); 
  
  dig_pins["test"] = PinControl(13, (boolean)LOW); //, activate_test

  Serial.println("000");
  sensors_val["temp"] = (sens_val_strucr){23, AnalogReadPin(dht, (String)"t")};
  Serial.println("000");
  sensors_val["hum"] = (sens_val_strucr){70, AnalogReadPin(dht, (String)"h")};
  Serial.println("000");
  sensors_val["gas"] = (sens_val_strucr){5, AnalogReadPin(0, true)};
  Serial.println("000");
  sensors_val["level_poliv"] = (sens_val_strucr){500, AnalogReadPin(1)};
  sensors_val["level_vapor"] = (sens_val_strucr){500, AnalogReadPin(2)};
  sensors_val["root_hum"] = (sens_val_strucr){80, AnalogReadPin(3, true)};
  /*
  Serial.println("rtrtr ");
  //rasClass.test();
  Serial.println("rtrtr ");
  Serial.println(sensors_val["gas"].value);
  */

/*
dig_pins.find("test")->second.set_priority(39);//19000
dig_pins.find("test")->second.set_priority(5);//11405
dig_pins.find("test")->second.set_priority(0);//11400

dig_pins.find("test")->second.set_priority(31);//11000

dig_pins.find("test")->second.set_priority(25);//11500
dig_pins.find("test")->second.set_priority(24);//11400
dig_pins.find("test")->second.set_priority(5);//11405
//dig_pins.find("test")->second.set_priority(0);//11400
dig_pins.find("test")->second.set_priority(35);//15405
*/

//
dig_pins["fitoLed"].edit_status_pin(true);
dig_pins["whiteLed"].edit_status_pin(false);
//dig_pins["poliv"].edit_status_pin(false, 39, true);
Serial.println("free memory: " + (String)memoryFree());
}


void loop(){
    
  update_pin();
  rasClass.raspb_update();
  //bluetooth.update_blut();
  espControl.update_esp();
  update_sensors_value();
  delay(500);
  
}

void update_pin(){
  for(auto it = dig_pins.begin(); it != dig_pins.end(); ++it){
    //Serial.println((String)it->first);
    //Serial.println("--------------------------------");
    it->second.update_pin(it->first);
  }
}


void update_sensors_value(){
  int data;
  for(auto it = sensors_val.begin(); it != sensors_val.end(); ++it){
    data = (int)it->second.cls.get_value();
    if (data != -1){
      it->second.value = (float)data;
    }
    Serial.print(String(it->first) + " " + String(it->second.value) + "\t");
  }
  Serial.println();
}

/*
void vapor_activation_func(){ // String key, String sensor_key, int low_level, int high_level
  String key = "";
  String sensor_key = "";
  int low_level = 50;
  int high_level = 50 ;
  if (sensors_val.find("hum") !=  sensors_val.end()){
    if (sensors_val.find("hum")->second.value <= VAPOR_ACTIVATION_LEVLE){
      PinControl::static_edit_status_pin(key, true, 1, true);
    } else if (sensors_val.find("hum")->second.value >= VAPOR_DISACTIVATION_LEVLE){
      PinControl::static_edit_status_pin(key, false, 1, true);
    }
  }    
}
*/

void PinControl::static_edit_status_pin(String key, bool status_pin, int now_priority, bool save_priority){
  if (dig_pins.find(key) != dig_pins.end()){
    //Serial.print((String)key + ":\t");
    if (key.length() < 7){
      //Serial.print("\t");
    }
    dig_pins.find(key)->second.edit_status_pin(status_pin, now_priority, save_priority);

  }
}

void PinControl::static_turn_on_for_time(String key, int delay_t_now, int now_priority, bool save_priority){
  if (dig_pins.find(key) != dig_pins.end()){
      Serial.print((String)key + ": (on time)\t");
      dig_pins.find(key)->second.turn_on_for_time(delay_t_now, now_priority, save_priority);
  }
}

void ActivateWork::standart_activation_func(String key_n){
  if (sensors_val.find(key_senser) !=  sensors_val.end()){
    if (sensors_val.find(key_senser)->second.value <= low_level){
      PinControl::static_edit_status_pin(key_n, true, 0, true);
    }
    else if (sensors_val.find(key_senser)->second.value >= how_func){
      PinControl::static_edit_status_pin(key_n, false, 0, true);
    }
  }    
}

void ActivateWork::revers_standart_activation_func(String key_n){
  if (sensors_val.find(key_senser) !=  sensors_val.end()){
    if (sensors_val.find(key_senser)->second.value >= how_func){
      PinControl::static_edit_status_pin(key_n, true, 0, true);
    }
    else if (sensors_val.find(key_senser)->second.value < low_level){
      PinControl::static_edit_status_pin(key_n, false, 0, true);
    }
  } 
}

void ActivateWork::poliv_activation_func(String key_n){
  if (sensors_val.find(key_senser) !=  sensors_val.end()){
    if (sensors_val.find(key_senser)->second.value <= low_level){
      PinControl::static_turn_on_for_time(key_n, POLIV_DELAY, 0, false);
    }
  }
}

void ActivateWork::test_activ_func(){
  String key = "simple";
  Serial.println("-------------------------start 13");
  PinControl::static_edit_status_pin("test", true, 0, false);
  delay(3000);
  PinControl::static_edit_status_pin("test", false, 0, false);
  Serial.println("----------------------------end 13");
}

extern int __bss_end;
extern void *__brkval;
// Функция, возвращающая количество свободного ОЗУ
int memoryFree() { // from: https://alexgyver.ru/lessons/code-optimisation/
  int freeValue;
  if ((int)__brkval == 0)
    freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)__brkval);
  return freeValue;
}
