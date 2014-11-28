#include <Wire.h>

#define BIELA_CIARA 940
#define UTOCNY_UHOL 90

byte val = 0;
byte mode = 0;
//int uhly [16] ={44, 66, 88, 22, 110, 352, 132, 330, 220, 286, 264, 308, 242, 176, 198, 154};
//stare:
//88, 110, 132, 66, 154, 44, 176, 22, 264, 330, 308, 354, 286, 220, 242, 198};  
/*
LINE SENZORY:
            vzadu : 3 , 7
            nalavo : 6 , 5
            napravo : 1 , X(2) 
            vpredu : 0 , 4
*/
//nove:
int uhly [16] ={88, 110, 132, 66, 154, 44, 176, 22, 264, 330, 308, 352, 286, 220, 242, 198};
float vysledokR;
boolean lopta = true;
int multi[16];
int vysledky[8];
int line[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int a, b, c, male, num, r = 120, sp, dir, dir1, vysledok, kom, teraz, kompas_smer, citaj, z, najmline, utok, poc = 0, vzdialenost;


void setup()
{
Serial2.begin(9600);
  pinMode(20, INPUT);
  pinMode(21, INPUT);
  digitalWrite(20, HIGH);
  digitalWrite(21, HIGH);
  Wire.begin(); 
  delay(500);
  
  pinMode(38, INPUT);
  
  pinMode(14, INPUT);
  pinMode(15, INPUT);
  
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  
  pinMode(31, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(35, OUTPUT);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT); 
  
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT); 
  
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT); 
  
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT); 
  

  delay(500);
Serial2.println("Kompici sokeres, press any key to debug");
  delay(100);
  kom = compass();
  for (int i = 0; i < 200; i++)
  {
    if (Serial2.available())
      break;
    delay(10);
  }
  if (Serial2.available()) menu();
Serial2.print("Starting a match kom=!"); 
Serial2.println(kom);
}
void ihrisko()
{
  boolean nacit = false;
  int s, plus = 0;
  int mini[8];
  int maxi[8];
 // boolean ci_true[8] = {false, false, false, false, false, false, false, false};
  for(int i = 0;i < 8;i++)
  {
    digitalWrite(31, i & 1);
    digitalWrite(33, (i >> 1) & 1);
    digitalWrite(35, (i >> 2) & 1); 
    delay(20); 
    mini[i] = maxi[i] = analogRead(A7);
  }
  stop_motor(2);
  stop_motor(4);
  motors(13, 0, 100);
  for(int j = 0;j < 10000;j++)
  {
    for(int i = 0;i < 8;i++)
    {
      digitalWrite(31, i & 1);
      digitalWrite(33, (i >> 1) & 1);
      digitalWrite(35, (i >> 2) & 1);  
      s = analogRead(A7);
      if(maxi[i] < s)
      {
        maxi[i] = s;
      }
      if(mini[i] > s)
      {
        mini[i] = s;
      }
    }
  }
  
  stop_motor(1);
  stop_motor(2);  
  stop_motor(3);
  stop_motor(4);
  for(int i = 0;i < 8;i++)
  {
    vysledky[i] = (maxi[i] - mini[i])/2 + mini[i];
    if(vysledky[i] > 960)
      vysledky[i] = 100;
  }
}  

boolean ciary(int d, int f) // d - je cislo senzora, f - je nacitana hodnota 
{
  if(vysledky[d] > f) return true;
  
  return false;
}

void configure_compass()
{
  Wire.beginTransmission(0x21);                              
  Wire.write(0x77);
  Wire.write(0x08);
  Wire.write(64 + 32 + 2);
  Wire.endTransmission();
}

int compass()
{
  byte lsb = 0, msb = 0;
  int value;
  Wire.beginTransmission(0x21);                              
  Wire.requestFrom(0x21, 2);
  int wait = 100;
  while ((!Wire.available()) && (wait--)) delay(1);
  if (Wire.available())    
  {
    msb = Wire.read();
    if (Wire.available())
      lsb = Wire.read();
  }  
  else msb = lsb = 0;
  Wire.endTransmission();
  value = (msb << 8) | lsb;
  return value;
}


void test_motors()
{
  for (int i = 1; i <= 4; i++)
  {
    for (int dir = 0; dir < 2; dir++)
    {
    Serial2.print("M");
    Serial2.print(i, DEC);
    Serial2.print(" ");
    Serial2.println(dir, DEC);
      
      for (int sp = 0; sp < 255; sp++)
      {
        mot(i, dir, sp);
        delay(10);
        if (sp % 25 == 0) Serial2.print(".");
      }
      for (int sp = 255; sp > 0; sp--)      
      {
        mot(i, dir, sp);
        delay(10);
        if (sp % 25 == 0) Serial2.print(".");
      }        
    }
  }
}

void test_ball_sensors()
{
  while (!Serial2.available())
  {
    hodnoty();
  Serial2.print("  -  ");   
    for (int i = 0; i < 16; i++)
    {
     int x = multi[i];

    Serial2.print(x,DEC);
    Serial2.print("  ");
    }
  Serial2.println();
    delay(200);
  }  
}



void test_vysledok()
{
  while (!Serial2.available())
  {
    hodnoty();
    najmensie();
  Serial2.print(vysledok, DEC);
  Serial2.print(" - ");
  Serial2.print(vysledokR, DEC);
  Serial2.println();
    delay(200);
  }
}

void test_line_sensors()
{
  while (!Serial2.available())
  {
    line_hodnoty();
    for(int i = 0;i < 8;i++)
    {
    Serial2.print(line[i], DEC);
    Serial2.print(" ");
    }
  Serial2.println();
    delay(200);
  }
}

void test_ciary()
{
  ihrisko();
  for(int j = 0;j < 8;j++)
  {
  Serial2.print(vysledky[j]);
  Serial2.print(" ");
  }
}
void test_compass()
{
  while (!Serial2.available())
  {
  Serial2.println(compass(), DEC);
    while(1)
    {
    Serial2.println(compass(), DEC);
      while(compass() < (3600-100) && compass() > 100)
      {
      Serial2.println(compass(), DEC);
        if(compass() > 1800)
        {
          mot(2, 0, 100);
          mot(4, 1, 100);
          mot(1, 1, 100);
          mot(3, 0, 100);
        } else {
          mot(2, 1, 100);
          mot(4, 0, 100);
          mot(1, 0, 100);
          mot(3, 1, 100);
        }
      }
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      break;  
  }
    delay(500);
  }
 
}

void calibrate_compass()
{
}

void test_narovnaj()
{
  kom = compass();
  while (!Serial2.available())
  {
  Serial2.println(kom, DEC);
    narovnaj();
  }
}


void menu()
{
  char key;
  do {
  Serial2.println("Debugging menu");
  Serial2.println("  1. test motors");
  Serial2.println("  2. test ball sensors");
  Serial2.println("  3. test line sensors");
  Serial2.println("  4. test compass");
  Serial2.println("  5. calibrate compass");
  Serial2.println("  6. start robot");
  Serial2.println("  7. test vysledok");
  Serial2.println("  8. test narovnaj");  
  Serial2.println("  9. configure compass"); 
  Serial2.println("  0. test ciary");  
  Serial2.print("> ");
    while (Serial2.available()) Serial2.read();
    
    do { key = Serial2.read(); } while (key < 0);
    switch (key)
    {
       case '1': test_motors(); break;
       case '2': test_ball_sensors(); break;
       case '3': test_line_sensors(); break;
       case '4': test_compass(); break;
       case '5': calibrate_compass(); break;
       case '7': test_vysledok(); break;
       case '8': test_narovnaj(); break;
       case '9': configure_compass(); break;
       case '0': test_ciary(); break;
    }
  } while (key != '6');
}

void hodnoty()
{
  int temp[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int ct1,ct2;
  for(ct1=0;ct1<4;ct1++)
    for(ct2=0;ct2<8;ct2++) {
      digitalWrite(22, ct2 & 1);
      digitalWrite(24, (ct2 >> 1) & 1);
      digitalWrite(26, (ct2 >> 2) & 1); 
      delay(5);
    
      a = analogRead(A0);
      b = analogRead(A1);
      temp[ct2] += a;      
      temp[ct2+8] += b; 
      if (ct1 == 3) {
        temp[ct2] /= 4;
        temp[ct2+8] /= 4;
      }
    }

  for(ct1 = 0;ct1 < 16;ct1++) {
    multi[ct1] = temp[ct1];
  }
}


void line_hodnoty()
{
  
  for(int i = 0;i < 8;i++)
  {
    digitalWrite(31, i & 1);
    digitalWrite(33, (i >> 1) & 1);
    digitalWrite(35, (i >> 2) & 1); 
    delay(20); 
    c = analogRead(A7);
    line[i] = c;
  }
/*
LINE SENZORY:
            vzadu : 3 , 7
            nalavo : 6 , 5
            napravo : 1 , X(2) 
            vpredu : 0 , 4
*/  
  /*
  int ciara;
  boolean p = false;
  for(int i = 0;i < 8;i++)
  {
    if(ciary(i, line[i]))
    {
      ciara = i;
      p = true;
      break;
    }
  }
  
  switch (ciara)
  {
    case 0:
            stop_motor(1);
            stop_motor(2);
            stop_motor(3);
            stop_motor(4);
            delay(1000);
            dir1 = 0;
            dir = 1;
            motors(24, dir1, r);
            motors(13, dir, r); 
            break;
    case 1: 
            break;
    case 2: 
            break;
    case 3: 
            break;
    case 4: 
            stop_motor(1);
            stop_motor(2);
            stop_motor(3);
            stop_motor(4);
            delay(1000);
            dir1 = 0;
            dir = 1;
            motors(24, dir1, r);
            motors(13, dir, r); 
            break;
    case 5: 
            break;
    case 6: 
            break;
    case 7: 
            break;
  }
  */
  najmline = 1025;
  line[0] = 1025;
  line[3] = 1025;
  line[4] = 1025;
  line[7] = 1025;
  
  for(int i = 0; i < 8;i++)
  {
    if(najmline > line[i])
    {
      najmline = line[i];
    }
  }
  if(najmline < BIELA_CIARA)
  {
    stop_motor(1);
    stop_motor(2);
    stop_motor(3);
    stop_motor(4);
    if(dir == 0) dir = 1;
    else if(dir == 1) dir = 0;
    
    if(dir1 == 0) dir1 = 1;
    else if(dir1 == 1) dir1 = 0;
    
    motors(24, dir1, r);
    motors(13, dir, r);
    
    delay(800);
    stop_motor(1);
    stop_motor(2);
    stop_motor(3);
    stop_motor(4);
  }
}

int rozdiel_uhlov(int x, int y)
{
  int d = abs(x - y);
  if(d > 180)
  {
    d = 360 - d;
  }
  return d;
}

int rel_rozdiel_uhlov(int x, int y)
{
  int d = y - x;
  if (d > 0)
  {
    if (d > 180) d = -(360 - d);
  }
  else
  {
    if (d < -180) d = (360 + d);
  }
  return d;
}

// vrati uhol -180..180 pre lubovolny uhol u
int normalizuj_uhol(int u)
{
  while (u < 0) u += 360;
  u = u % 360;
  if (u > 180) u -= 360;
  return u;
}

void najmensie()
{

  int H1, H2, H3, P180, P270, P360, pomH;
  num = 0;
  male = 1024;
  for(int i = 0;i < 3;i++)
  {
    for(int j = 0;j < 16;j++)
    {
      if(male >= multi[j])
      {
        male = multi[j];
        num = j;  
      }
    }
    if(male > 450 && i == 0) vzdialenost = 1;
    else vzdialenost = 0;
    if(male > 950) lopta = false;
    switch(i)
    {
      case 0: H1 = uhly[num];
              break;
      case 1: H2 = uhly[num];
              break;
      case 2: H3 = uhly[num];
              break;
    }
    multi[num] = 1023;
  }
  
  P180 = 0;
  P270 = 0;
  P360 = 0;
  for(int i = 0;i < 3;i++)
  {
    switch(i)
    {
      case 0: pomH = H1;
              break;
      case 1: pomH = H2;
              break;
      case 2: pomH = H3;
              break;
    }
    if(pomH <= 180)
    {
      P180++;
    }
    if(pomH > 90 && pomH <= 270)
    {
      P270++;
    }
    if(pomH > 180 && pomH < 360)
    {
      P360++;
    }
  }
  
  if ((P180 == 3) || (P270 == 3) || (P360 == 3))
  {
    vysledok = (H1+H2+H3) / 3;
  } else {
    H1 = (H1 + 180) % 360;
    H2 = (H2 + 180) % 360;
    H3 = (H3 + 180) % 360;
    vysledok = (H1+H2+H3) / 3;
    vysledok = (vysledok + 180) % 360;
  } 
  
  int k = compass() / 10;
  int uhol = normalizuj_uhol(rel_rozdiel_uhlov(kom/10, k) + 360 - vysledok);
  
Serial2.println(vysledok);
  
  if(abs(uhol) < UTOCNY_UHOL)
  {
    utok = 1;
  } else {
    utok = 0;
  }
  
  vysledokR = float(vysledok) * 3.1415926536 / 180.0;  
}

void mot(int m, int dir, int sp)
{
  switch (m) {
    
    case 1: if(dir == 1)
            {
              digitalWrite(3, LOW);
              analogWrite(2, sp);
            } else if(dir == 0)
                    {
                      digitalWrite(2, LOW);
                      analogWrite(3, sp);
                    }
            break;
    case 3 : if(dir == 1)
            {
              digitalWrite(5, LOW);
              analogWrite(4, sp);
            } else if(dir == 0)
                    {
                      digitalWrite(4, LOW);
                      analogWrite(5, sp);
                    }
            break;
    case 2: if(dir == 1)
            {
              digitalWrite(6, LOW);
              analogWrite(7, sp);
            } else if(dir == 0)
                    {
                      digitalWrite(7, LOW);
                      analogWrite(6, sp);
                    }
            break;
            
    case 4: if(dir == 1)
            {
              digitalWrite(9, LOW);
              analogWrite(8, sp);
            } else if(dir == 0)
                    {
                      digitalWrite(8, LOW);                      
                      analogWrite(9, sp);
                    }
            break;
  }
}

void stop_motor(int number)
{
  switch (number)
  {
    case 1: digitalWrite(2, LOW);
            digitalWrite(3, LOW);
            break;
    case 2: digitalWrite(4, LOW);
            digitalWrite(5, LOW);
            break;
    case 3: digitalWrite(6, LOW);
            digitalWrite(7, LOW);
            break;
    case 4: digitalWrite(8, LOW);
            digitalWrite(9, LOW);
            break;
  }
}

void motors(int two, int dir, int sp)
{ 
  if(two == 13)
  {
    mot(1, dir, sp);
    mot(3, dir, sp);
  }
  if(two == 24)
  {
    mot(2, dir, sp);
    mot(4, dir, sp);
  }
}

boolean je_vlavo(int e, int f)
{
  int g = e - f;  
  if (g < 0) g += 3600;   
  if (g < 1800) return true;
  return false;  
}  

void narovnaj()
{
  stop_motor(1);
  stop_motor(2);
  stop_motor(3);
  stop_motor(4);
  int pom, k;
      citaj = compass();
      if(je_vlavo(kom, citaj))
      {
        while(je_vlavo(kom, citaj))
        {
          citaj = compass();
          mot(2, 0, 50);
          mot(4, 1, 50);
          mot(1, 1, 50);
          mot(3, 0, 50);
          delay(10);
        }
      } else {
          while(!je_vlavo(kom, citaj))
          {
            citaj = compass();
            mot(2, 1, 50);
            mot(4, 0, 50);
            mot(1, 0, 50);
            mot(3, 1, 50);
            delay(10);            
          }
      }
      stop_motor(1);
      stop_motor(2);
      stop_motor(3);
      stop_motor(4);
}

void smer()
{ 
  if(lopta == true)
  {
    if (utok == 0)
    {
      if((poc % 13) == 0)
      {
        stop_motor(1);
        stop_motor(2);
        stop_motor(3);
        stop_motor(4);
        narovnaj();
        poc++;
        motors(24, 0, 250);
        motors(13, 1, 250);
      }   
      else poc++;
    } else {
        if(rozdiel_uhlov(kom/10, compass()/10) > 15) narovnaj();
        
        poc = 0;      
        if(vysledok == 352 || vysledok == 22)
        {
          stop_motor(1);
          stop_motor(2);
          stop_motor(3);
          stop_motor(4);
          dir1 = 1;
          dir = 0;
          motors(24, dir1, r);
          motors(13, dir, r);
        } else if(vysledok == 44 || vysledok == 66)
          {
            stop_motor(1);
            stop_motor(2);
            stop_motor(3);
            stop_motor(4);
            dir = 1;
            dir1 = 1;
            motors(13, dir, r);
            if(vzdialenost == 1) motors(24, dir1, r);
          } else if(vysledok == 330 || vysledok == 308)
            {
              stop_motor(1);
              stop_motor(2);
              stop_motor(3);
              stop_motor(4);
              dir1 = 0;
              dir = 0;
              motors(24, dir1, r);
              if(vzdialenost == 1) motors(13, dir, r);
            } 
        else {
          if(vysledok > 44 && vysledok < 242)
          {
            dir = 1;
          } else 
          {
            dir = 0;
          }
        
          if(vysledok < 330 && vysledok > 154)
          {
            dir1 = 0;
          } else 
          {
            dir1 = 1;
          }
                  
          double tg = abs(tan(vysledokR));          
          
          if(tg >= 1.0)
          {
            motors(24, dir1, r);
            motors(13, dir, int(double(r)/tg));
          } else 
          {    
             motors(13, dir, r);
             motors(24, dir1, int(double(r)*tg));
          }
      
        }
    }
  } else {
      stop_motor(1);
      stop_motor(2);
      stop_motor(3);
      stop_motor(4);
      lopta = true;
  }
  
}

void loop()
{  
    line_hodnoty();
    hodnoty();
    najmensie(); 
    smer(); 
}
