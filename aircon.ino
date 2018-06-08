#include <SPI.h>
#include <Ethernet.h>
#include <Stewitter.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x5D, 0x69 };                //MACアドレス
Stewitter twitter("785f0ba2cbce20d28bf65137bb71d205");
String post_msg;                                                    //投稿用メッセージ char型に変換しなければならない

String recieved_msg;                                                //受信したメッセージを格納
String last_msg_id;                                                 //最後に受信したメッセージのID
String auth_from = "never_despair4";                   //コマンドを許可する送信者ID　@無しで記入
boolean f_done = false;                                             //コマンドを実行したか
boolean f_post = false;                                             //投稿が正常に行われたか
byte pos_splitter_2;                                                //2つめの'|'の場所
String command[] = {"turnon", "temp", "28", "relay off"};
                                                                    //反応するコマンド一覧。配列の先頭から実行される
                                                                    //追加したら92行目付近のswitch case文も修正

byte pin_led = 7;
byte pin_relay = 6;

void setup(){
  delay(2000);
  Ethernet.begin(mac);
  Serial.begin(9600);
  pinMode(pin_led, OUTPUT);
  pinMode(pin_relay, OUTPUT);

}

void loop(){
  twitter_get();
  delay(60000);                                            //60秒で1回に制限
}

void twitter_get(){
  if (twitter.lastMention()) {                             // twitterに接続して、最後のメンションを取得
    int status = twitter.wait();                           // 完了を待つ
    if (status == 200) {                                   // status が 200なら正常に完了
      recieved_msg = twitter.response();                   // 最後のメンションを取得

      if( recieved_msg.indexOf("API_CALL_FAILED") == 0){   //ステータス200でもエラーが有る　アクセス回数制限等
        Serial.println(recieved_msg);
      }
      else{
        check_msg();
      }
    }
    else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  }
  else {
    Serial.println("connection failed.");
  }
}

void check_msg(){                                          //新規投稿があったかチェック
  byte pos_splitter_1 = 14;
       pos_splitter_2 = recieved_msg.indexOf('|',pos_splitter_1 + 1);
  byte pos_splitter_3 = recieved_msg.indexOf('|',pos_splitter_2 + 1);

  String latest_msg_id = recieved_msg.substring(pos_splitter_1 + 1, pos_splitter_2 );
  String latest_msg_from = recieved_msg.substring(pos_splitter_2 + 1, pos_splitter_3);

  Serial.println("");
  Serial.println(recieved_msg);
  Serial.println(latest_msg_id);                                          //メッセージID
  Serial.println(latest_msg_from);                                        //送信者
  if(last_msg_id != latest_msg_id ){                                       //新規投稿があったか
    Serial.println("New message.");
    last_msg_id = latest_msg_id;
    if( latest_msg_from == auth_from ){                                    //権限のある送信者がarduinoに送ったなら
      Serial.println("authority OK");
      recieved_msg.replace(auth_from, " ");                                //コマンドがユーザーIDに含まれているかもしれないので除去
      check_command();
    }
  }
  else{
    Serial.println("NO UPDATE");
  }
}

void check_command(){
  //25度でエアコンをオン
  long ARRAY_HIGHLOW[] = { 4866928, 3540, 3412, 936, 808, 928, 2544, 932, 812, 928, 2540, 936, 2548, 924, 2548, 936, 2540, 932, 2544, 932, 808, 932, 2540, 940, 804, 932, 2540, 932, 2548, 936, 2540, 932, 2544, 932, 2548, 932, 804, 932, 2544, 928, 812, 928, 812, 928, 804, 936, 804, 928, 812, 928, 812, 928, 804, 936, 2544, 932, 804, 936, 804, 936, 804, 928, 812, 928, 804, 936, 804, 3544, 3408, 932, 812, 928, 2540, 940, 804, 932, 2540, 940, 2540, 936, 2540, 932, 2544, 940, 2540, 932, 804, 932, 2548, 932, 804, 928, 2548, 936, 2540, 932, 2540, 944, 2532, 940, 2540, 936, 804, 936, 2540, 936, 804, 936, 800, 932, 804, 936, 804, 936, 804, 936, 804, 928, 804, 936, 2540, 944, 796, 936, 804, 936, 804, 936, 796, 940, 804, 936, 800, 3544, 3408, 940, 13836, 3544, 3408, 940, 796, 936, 804, 940, 2540, 932, 804, 936, 804, 936, 796, 940, 800, 940, 804, 936, 800, 932, 804, 936, 2540, 936, 800, 936, 804, 940, 800, 932, 808, 932, 804, 936, 804, 936, 2540, 932, 2544, 932, 804, 936, 2540, 936, 2540, 932, 812, 928, 804, 936, 804, 936, 2544, 932, 2540, 936, 804, 928, 2548, 932, 2544, 932, 808, 932, 804, 3536, 3420, 928, 808, 932, 808, 936, 2544, 924, 812, 928, 812, 928, 804, 928, 812, 928, 812, 928, 812, 928, 804, 928, 2552, 932, 804, 928, 812, 928, 812, 928, 812, 928, 804, 928, 812, 928, 2548, 928, 2548, 932, 812, 900, 2568, 932, 2548, 904, 836, 928, 812, 900, 836, 900, 2580, 896, 2576, 924, 812, 908, 2576, 896, 2576, 920, 816, 900, 840, 3508, 3448, 896, };
  long ARRAY_HIGHLOWS[] = { 67076, 3544, 3412, 876, 860, 940, 800, 880, 2604, 868, 2604, 876, 2604, 948, 2532, 876, 2600, 880, 2596, 876, 868, 936, 804, 924, 2556, 932, 2540, 940, 2544, 872, 2608, 872, 2600, 880, 2600, 960, 780, 872, 2608, 872, 868, 872, 2604, 936, 800, 880, 860, 880, 860, 880, 860, 876, 860, 880, 2604, 876, 860, 936, 2540, 936, 812, 928, 812, 868, 868, 924, 812, 3536, 3420, 932, 812, 872, 868, 920, 2556, 876, 2600, 880, 2596, 884, 2596, 928, 2552, 876, 2604, 920, 816, 944, 800, 936, 2540, 932, 2548, 920, 2556, 932, 2548, 936, 2540, 876, 2604, 932, 812, 872, 2604, 872, 864, 876, 2608, 916, 820, 952, 788, 872, 868, 872, 864, 880, 860, 940, 2540, 872, 864, 876, 2608, 932, 804, 936, 804, 876, 860, 880, 860, 3544, 3416, 932, 13852, 3528, 3428, 932, 804, 936, 804, 880, 2604, 920, 816, 920, 820, 876, 864, 936, 804, 920, 820, 868, 868, 880, 860, 924, 2556, 872, 864, 876, 860, 940, 808, 872, 860, 924, 816, 880, 860, 936, 2548, 928, 2548, 872, 864, 920, 2556, 884, 2596, 876, 868, 872, 864, 876, 864, 872, 2604, 936, 2548, 876, 860, 876, 2600, 880, 2600, 872, 868, 872, 868, 3544, 3416, 872, 864, 876, 860, 880, 2604, 940, 796, 940, 800, 924, 816, 872, 868, 872, 868, 872, 868, 872, 868, 868, 2608, 872, 868, 932, 804, 876, 868, 928, 804, 880, 860, 880, 864, 872, 2608, 872, 2608, 916, 816, 924, 2556, 924, 2556, 876, 860, 880, 868, 872, 860, 936, 2548, 868, 2604, 876, 868, 872, 2604, 876, 2604, 872, 864, 876, 860, 3548, 3412, 924, };
  long ARRAY_HIGHLOW28[] = { 315156, 3512, 3444, 900, 2572, 904, 836, 904, 2576, 904, 2568, 904, 2568, 912, 2568, 904, 2568, 912, 2568, 904, 2572, 908, 832, 904, 2568, 908, 2572, 904, 2568, 912, 2568, 904, 2576, 904, 2568, 904, 832, 908, 2568, 912, 832, 900, 2572, 908, 832, 908, 832, 900, 832, 908, 832, 908, 832, 908, 2572, 900, 832, 908, 2572, 904, 836, 900, 840, 900, 832, 908, 832, 3516, 3436, 904, 2572, 908, 832, 908, 2572, 900, 2572, 904, 2576, 904, 2568, 912, 2568, 904, 2568, 912, 2568, 904, 832, 908, 2568, 904, 2572, 908, 2572, 904, 2568, 908, 2572, 904, 2568, 912, 832, 900, 2568, 912, 832, 908, 2568, 904, 840, 900, 832, 908, 788, 944, 840, 908, 824, 908, 2568, 912, 832, 900, 2576, 904, 840, 900, 832, 900, 840, 900, 832, 3516, 3436, 912, 13864, 3516, 3440, 908, 828, 908, 832, 900, 2580, 904, 828, 908, 832, 908, 832, 904, 828, 908, 832, 908, 832, 908, 832, 900, 2576, 904, 832, 908, 832, 908, 832, 900, 836, 904, 832, 908, 832, 908, 2572, 904, 2576, 900, 836, 900, 2568, 912, 2568, 904, 832, 908, 832, 908, 832, 908, 2568, 912, 2568, 904, 832, 908, 2564, 908, 2572, 904, 828, 908, 832, 3516, 3444, 904, 832, 908, 832, 900, 2568, 912, 832, 908, 832, 900, 840, 900, 840, 900, 832, 908, 832, 908, 832, 900, 2568, 912, 832, 908, 832, 900, 840, 900, 832, 908, 832, 908, 832, 908, 680, 232, 1596, 964, 2568, 912, 832, 900, 2568, 912, 2536, 940, 832, 908, 832, 908, 832, 900, 2576, 904, 2440, 1036, 804, 932, 2576, 904, 2568, 904, 832, 908, 832, 3516, 3408, 940, 27780, 276, 14960, 276, };

  int iTimeLength;
  int iCountHighLow = sizeof( ARRAY_HIGHLOW ) / sizeof( ARRAY_HIGHLOW[0] );
  int iCountHighLows = sizeof( ARRAY_HIGHLOWS ) / sizeof( ARRAY_HIGHLOWS[0] );
  int temp,ans,tv;
  unsigned long ulTimeStart;
  byte num_command = sizeof(command) / sizeof(command[0]);                  //command[]の要素数

  f_done = false;
  post_msg = "[" + String(millis()) + "] " + "@" + auth_from + " command No.[ ";  //重複投稿で投稿失敗を防ぐ
  for(byte i = 0; i < num_command; i++){
    if( recieved_msg.indexOf( command[i], pos_splitter_2) != -1 ){          //コマンド一覧と受信メッセージの比較
      Serial.print("found command !  >> ");
      Serial.println( command[i] );
      switch(i){                                      //一致したコマンド(配列の添字)で分岐
        case 0:

  for( int iIndexHighLow = 0; iIndexHighLow < iCountHighLow; iIndexHighLow++ )
  {
    iTimeLength = ARRAY_HIGHLOW[iIndexHighLow];
    ulTimeStart = micros();
    do
    {
      digitalWrite( pin_led, 1 - (iIndexHighLow % 2) );  // iIndexHighLow : 0は、high。 1は、low。以下同様。
      delayMicroseconds(8);
      digitalWrite( pin_led, 0 );
      delayMicroseconds(7);
    } while( iTimeLength > micros() - ulTimeStart );

  }
    break;

    case 1:

      ans = analogRead(0) ;
      tv = map(ans,0,1023,0,5000) ;
      temp = map(tv,300,1600,-30,100) ;
      Serial.print("Current Room Temp is ") ;
      Serial.print(temp) ;
      Serial.println("Celsius");

    break;

    case 2:

  for( int iIndexHighLows = 0; iIndexHighLows < iCountHighLows; iIndexHighLows++ )
  {
    iTimeLength = ARRAY_HIGHLOW28[iIndexHighLows];
    ulTimeStart = micros();
    do
    {
      digitalWrite( pin_led, 1 - (iIndexHighLows % 2) );
      delayMicroseconds(8);
      digitalWrite( pin_led, 0 );
      delayMicroseconds(7);
    } while( iTimeLength > micros() - ulTimeStart );

  }

    break;

      case 3:
      ans = analogRead(0) ;
      tv = map(ans,0,1023,0,5000) ;
      temp = map(tv,300,1600,-30,100) ;

if(temp<26){
  for( int iIndexHighLows = 0; iIndexHighLows < iCountHighLows; iIndexHighLows++ )
  {
    iTimeLength = ARRAY_HIGHLOWS[iIndexHighLows];
    ulTimeStart = micros();
    do
    {
      digitalWrite( pin_led, 1 - (iIndexHighLows % 2) );
      delayMicroseconds(8);
      digitalWrite( pin_led, 0 );
      delayMicroseconds(7);
    } while( iTimeLength > micros() - ulTimeStart );

  }

}
      else break;


      }










      f_done = true;
      post_msg += String(i) + ", ";
    }
  }
  if(f_done){                                            //コマンドを実行したので報告を投稿
    post_msg += "] has been executed.";
    ans = analogRead(0) ;
      tv = map(ans,0,1023,0,5000) ;
      temp = map(tv,300,1600,-30,100) ;
    post_msg += "Current temp is";
    post_msg += " ";
    post_msg += temp;
    Serial.println(post_msg);
    f_post = false;
    byte cnt_post = 0;
    while(f_post == false){                              //投稿が成功するまでトライ
      cnt_post +=1;                                      //トライ回数をカウント
      if(cnt_post <= 10){                                //30秒ごとに10回までトライ
        twitter_post();
        if(f_post == false){
          delay(30000);
        }
      }
      else{                                              //10回を超えたら断念
        Serial.println("投稿10回トライしましたが失敗しました");
      }
    }
  }
  else{
    Serial.println("no command in message.");
  }
}

void twitter_post(){
  char post_msg_char[255];
  post_msg.toCharArray(post_msg_char,255);                 //String から char に変換
  if (twitter.post(post_msg_char)) {                       // twitterに接続して、メッセージを送信
    int status = twitter.wait();                           // 完了を待つ
    if (status == 200) {                                   // status が 200なら正常に完了
      f_post = true;
      Serial.println(twitter.response());                  // レスポンスを表示
    }
    else {
      f_post = false;
      Serial.print("failed : code ");
      Serial.println(status);
    }
  }
  else {
    f_post = false;
    Serial.println("connection failed.");
  }
}
