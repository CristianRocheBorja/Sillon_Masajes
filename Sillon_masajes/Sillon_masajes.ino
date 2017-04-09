
// ############# Librerias ##############
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Modulo I2C para LCD
#include <virtuabotixRTC.h>     // Reloj Tiempo Real


// ########### LCD ###########
LiquidCrystal_I2C lcd(0x3F,16,2);  // Indica la direccion en la que escucha el I2C y el tamaño
// Construccion de simbolos pixel a pixel
byte p1[8] = {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10}; // 1 - Linea vertical
byte p2[8] = {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}; // 2 - Linea vertical
byte p3[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C}; // 3 - Linea vertical
byte p4[8] = {0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E}; // 4 - Linea vertical
byte p5[8] = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}; // 5 - Linea vertical (Cuadro completo)
byte p6[8] = {0x00,0x00,0x00,0x00,0x04,0x0E,0x1F,0x00}; // Flecha arriba
byte p7[8] = {0x00,0x1F,0x0E,0x04,0x00,0x00,0x00,0x00}; // Flecha abajo
byte p8[8] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}; // Doble barra ||

long ultimoRefresco;  // Para refrescar el reloj de la pantalla inicial cada minuto

// Motores (Pines PWM)
#define MotorA 9
#define MotorB 10
#define MotorC 11
#define MotorD 12

// ######## Botonera #######
#define botonOk       35
#define botonArriba   36
#define botonAbajo    37
#define botonMas      38
#define botonMenos    39

// ######## MENUS #########
int menuNum    = 2;  // 0 = MenuPrincipal; 1 = SubmenuModoManual;
int indexMenu  = 0;  // Posicion en el menu

// ####### MOTORES #########
int potenciaMotores[]={0,0,0,0};
#define incremento 15

// ####### RELOJ #########
// CLK->6, DATA->7, RESET->8
virtuabotixRTC myRTC(6, 7, 8);


// ####### INTERRUPCIONES #######
boolean backPulsado   = false;
boolean loginPulsado  = false;
unsigned long ultimaInterrupcion;

// ######## MASAJE #######
boolean masajeEnCurso = false;

// ####### LOGIN #########
boolean logueado = false;


// ##########################################################
// ####################### FUNCIONES ########################
// ##########################################################

// ######### ACTUALIZAR MOTORES #######
void actualizarMotores(){
  analogWrite(MotorA,potenciaMotores[0]);
  analogWrite(MotorB, potenciaMotores[1]);
  analogWrite(MotorC,potenciaMotores[2]);
  analogWrite(MotorD,potenciaMotores[3]);
}

// ######### SET MOTORES #########
void setMotoresAll(int in){
  for(int i=0;i<4;i++) potenciaMotores[i]=in;
}

void imprimirMotores(){
  Serial.print("MotorA: ");
  Serial.print(potenciaMotores[0]);
  Serial.print(" MotorB: ");
  Serial.print(potenciaMotores[1]);
  Serial.print(" MotorC: ");
  Serial.print(potenciaMotores[2]);
  Serial.print(" MotorD: ");
  Serial.println(potenciaMotores[3]);
}

// ######## INCREMENTAR POTENCIA ########
void incrementarPotencia(int inMotor){
  switch(potenciaMotores[inMotor]){
    case 0:
      potenciaMotores[inMotor]=100;
      break;
    case 100:
      potenciaMotores[inMotor]=111;
      break;
    case 111:
      potenciaMotores[inMotor]=122;
      break;
    case 122:
      potenciaMotores[inMotor]=133;
      break;
    case 133:
      potenciaMotores[inMotor]=144;
      break;
    case 144:
      potenciaMotores[inMotor]=155;
      break;
    case 155:
      potenciaMotores[inMotor]=166;
      break;
    case 166:
      potenciaMotores[inMotor]=177;
      break;
    case 177:
      potenciaMotores[inMotor]=188;
      break;
    case 188:
      potenciaMotores[inMotor]=199;
      break;
    case 199:
      potenciaMotores[inMotor]=210;
      break;
    case 210:
      potenciaMotores[inMotor]=221;
      break;
    case 221:
      potenciaMotores[inMotor]=232;
      break;
    case 232:
      potenciaMotores[inMotor]=243;
      break;
    case 243:
      potenciaMotores[inMotor]=255;
      break;
  }
}

// ######## DECREMENTAR POTENCIA ########
void decrementarPotencia(int inMotor){
  switch(potenciaMotores[inMotor]){
    case 100:
      potenciaMotores[inMotor]=0;
      break;
    case 111:
      potenciaMotores[inMotor]=100;
      break;
    case 122:
      potenciaMotores[inMotor]=111;
      break;
    case 133:
      potenciaMotores[inMotor]=122;
      break;
    case 144:
      potenciaMotores[inMotor]=133;
      break;
    case 155:
      potenciaMotores[inMotor]=144;
      break;
    case 166:
      potenciaMotores[inMotor]=155;
      break;
    case 177:
      potenciaMotores[inMotor]=166;
      break;
    case 188:
      potenciaMotores[inMotor]=177;
      break;
    case 199:
      potenciaMotores[inMotor]=188;
      break;
    case 210:
      potenciaMotores[inMotor]=199;
      break;
    case 221:
      potenciaMotores[inMotor]=210;
      break;
    case 232:
      potenciaMotores[inMotor]=221;
      break;
    case 243:
      potenciaMotores[inMotor]=232;
      break;
    case 255:
      potenciaMotores[inMotor]=243;
      break;
  }
}

// ######### MASAJE 1   ###############
void masajeRelajante(){
  unsigned long tiempoInicio=millis();
  potenciaMotores[0]=100;
  while(!backPulsado && !loginPulsado){
    if ((millis()-tiempoInicio)<20000){
      potenciaMotores[1]=0;
    }
    else{
      if ((millis()-tiempoInicio)<40000){
        potenciaMotores[1]=100;
        potenciaMotores[2]=0;
      }
      else{
        if ((millis()-tiempoInicio)<60000){
          potenciaMotores[2]=100;
          potenciaMotores[3]=0;
        }
        else{
          if ((millis()-tiempoInicio)<70000){
            potenciaMotores[3]=100;
          }
          else
            tiempoInicio=millis();
        }
      }
    }
    actualizarMotores();
    imprimirMotores();
  }
}

// ######### MASAJE 2   ###############
void masajePulsante(){
  unsigned long tiempoInicio=millis();
  while(!backPulsado && !loginPulsado){
    if((millis()-tiempoInicio)<2000) setMotoresAll(255);      // Maxima potencia inicial durante 2 segundos
    else if(potenciaMotores[0]<60) tiempoInicio=millis();  // Si la potencia ha bajado del minimo se reinicia el ciclo
    else setMotoresAll(255-((millis()-tiempoInicio)/100));  // Cada segundo se reduce la potencia de los motores en 10
    actualizarMotores();
    imprimirMotores();

  }
}

// ######### MASAJE 3   ###############
void masajeVariado(){
  unsigned long tiempoInicio=millis();
  int retardo=5000;
  int potencia=160;
  setMotoresAll(0);
  potenciaMotores[0]=potencia;
  potenciaMotores[2]=potencia;
  actualizarMotores();
  boolean cambiar=false;
  boolean fin = true;
  while(!backPulsado && !loginPulsado){
    if((millis()-tiempoInicio)>retardo){
      if(potencia<180){
        if(retardo>550)retardo-=255-potenciaMotores[0];
        if(cambiar) potencia+=5;
        cambiar=!cambiar;
      }
      else if(potencia<255){
        if(retardo>410)retardo-=70;
        if(cambiar) potencia+=1;
        cambiar=!cambiar;
      }
      else if(fin) {
        fin=!fin;
        setMotoresAll(255);
        retardo=10000;
      }
      else{
        retardo=5000;
        potencia=100;
        fin=true;
      }
      if(potenciaMotores[0]!=0){
        potenciaMotores[0]=0;
        potenciaMotores[1]=potencia;
        potenciaMotores[2]=0;
        potenciaMotores[3]=potencia;
      }
      else{
        potenciaMotores[0]=potencia;
        potenciaMotores[1]=0;
        potenciaMotores[2]=potencia;
        potenciaMotores[3]=0;
      }
      actualizarMotores();
      imprimirMotores();
      Serial.print("retardo: ");
      Serial.print(retardo);
      tiempoInicio=millis();
    }
  }
}

// ######### MASAJE 4   ###############
void masajeIncremental(){
  unsigned long tiempoInicio=millis();
  while(!backPulsado && !loginPulsado){
    if((millis()-tiempoInicio)<8000) potenciaMotores[0]=95;
    else{
      if((millis()-tiempoInicio)<16000) {
        potenciaMotores[0]=135;
        potenciaMotores[1]=95;
      }
      else{
        if((millis()-tiempoInicio)<24000){
          potenciaMotores[0]=175;
          potenciaMotores[1]=135;
          potenciaMotores[2]=95;
        }
        else{
          if((millis()-tiempoInicio)<32000){
            potenciaMotores[0]=215;
            potenciaMotores[1]=175;
            potenciaMotores[2]=135;
            potenciaMotores[3]=95;
          }
          else{
            if((millis()-tiempoInicio)<40000){
              potenciaMotores[0]=255;
              potenciaMotores[1]=215;
              potenciaMotores[2]=175;
              potenciaMotores[3]=135;
            }
            else{
              tiempoInicio=millis();
              setMotoresAll(0);
            }
          }
        }
      }
    }
    actualizarMotores();
    imprimirMotores();
  }
}

// ######### MASAJE 5   ###############
void masajeFuerte(){
  unsigned long tiempoInicio=millis();
  int motor = 0;
  int index = 0;
  boolean meneo=false;
  unsigned long ultimoCambio=millis();
  boolean activar=true;
  while(!backPulsado && !loginPulsado){
     switch(index){
        case 0:
          setMotoresAll(0);
          while(!backPulsado && !loginPulsado && (millis()-tiempoInicio)<20000){
              if(activar){
                setMotoresAll(255);
                activar=false;
                actualizarMotores();
                imprimirMotores();
              }
          }
          activar=true;
          break;
        case 1:
          setMotoresAll(0);
          while(!backPulsado && !loginPulsado && (millis()-tiempoInicio)<20000){
              if((millis()-ultimoCambio)>1000){
                switch(motor){
                  case 0:
                    potenciaMotores[0]=255;
                    potenciaMotores[3]=0;
                    break;
                  case 1:
                    potenciaMotores[1]=255;
                    potenciaMotores[0]=0;
                    break;
                  case 2:
                    potenciaMotores[2]=255;
                    potenciaMotores[1]=0;
                    break;
                  case 3:
                    potenciaMotores[3]=255;
                    potenciaMotores[2]=0;
                    break;
                }
                motor = (motor+1)%4;
                actualizarMotores();
                imprimirMotores();
                ultimoCambio=millis();
              }
          }
          break;
        case 2:
          setMotoresAll(0);
          while(!backPulsado && !loginPulsado && (millis()-tiempoInicio)<20000){
              if((millis()-ultimoCambio)>200){
                switch(motor){
                  case 0:
                    potenciaMotores[0]=255;
                    potenciaMotores[3]=0;
                    break;
                  case 1:
                    potenciaMotores[1]=255;
                    potenciaMotores[0]=0;
                    break;
                  case 2:
                    potenciaMotores[2]=255;
                    potenciaMotores[1]=0;
                    break;
                  case 3:
                    potenciaMotores[3]=255;
                    potenciaMotores[2]=0;
                    break;
                }
                motor = (motor+1)%4;
                actualizarMotores();
                imprimirMotores();
                ultimoCambio=millis();
              }
          }
          break;
        case 3:
          setMotoresAll(0);
          while(!backPulsado && !loginPulsado && (millis()-tiempoInicio)<20000){
              if((millis()-ultimoCambio)>200){
                if(activar){
                  setMotoresAll(255);
                }
                else setMotoresAll(0);
                activar=!activar;
                actualizarMotores();
                imprimirMotores();
                ultimoCambio=millis();
              }
          }
          activar=true;
          
          break;
      }
      index= (index+1)%4;
      tiempoInicio=millis();
  }
}

// ######## DIBUJAR MENU INICIAL #####
void dibujarMenuInicial(){
  myRTC.updateTime();  // Carga la fecha y la hora
  lcd.write(7);
  lcd.setCursor(4,0);lcd.print(myRTC.dayofmonth);lcd.print("/");lcd.print(myRTC.month);lcd.print("/");lcd.print(myRTC.year);
  lcd.setCursor(15, 0);
  lcd.write(7);lcd.setCursor(0,1);lcd.write(7);
  lcd.setCursor(6,1);lcd.print(myRTC.hours);lcd.print(":");
  if(myRTC.minutes<10)lcd.print("0");
  lcd.print(myRTC.minutes);//lcd.print(":");lcd.print(myRTC.seconds);
  lcd.setCursor(15,1);lcd.write(7);
}

// ######## DIBUJAR FLECHAS DIRECCIONALES #####
void dibujarFlechasDireccionales(){
  lcd.setCursor(15,0);
  lcd.write(5);
  lcd.setCursor(15,1);
  lcd.write(6);
}

// ######### GET BLOQUES(POTENCIA) #########
int getBloques(int inMotor){
  switch(potenciaMotores[inMotor]){
      case 0:
        return 0;
        break;
      case 100:
        return 1;
        break;
      case 111:
        return 2;
        break;
      case 122:
        return 3;
        break;
      case 133:
        return 4;
        break;
      case 144:
        return 5;
        break;
      case 155:
        return 6;
        break;
      case 166:
        return 7;
        break;
      case 177:
        return 8;
        break;
      case 188:
        return 9;
        break;
      case 199:
        return 10;
        break;
      case 210:
        return 11;
        break;
      case 221:
        return 12;
        break;
      case 232:
        return 13;
        break;
      case 243:
        return 14;
        break;
      case 255:
        return 15;
        break;
  }
}

// ######### REFRESCAR LCD #########
void refrescarLCD(){
  lcd.clear();
  ultimoRefresco=millis();
  // ############## MENU PRINCIPAL #################
  if(menuNum==0){
    // Primera linea
    switch(indexMenu){
      case 0:
        lcd.print("1 Modo Manual");
        break;
      case 1:
        lcd.print("2 M.Relajante");
        break;
      case 2:
        lcd.print("3 M.Pulsante");
        break;
      case 3:
        lcd.print("4 M.Variado");
        break;
      case 4:
        lcd.print("5 M.Incremental");
        break;
      case 5:
        lcd.print("6 M.ExtraFuerte");
        break;
    }
    // Segunda linea
    lcd.setCursor(2,1);
    if(!masajeEnCurso) lcd.print("-pulse ok-");
    else lcd.print("-En Curso-");
    dibujarFlechasDireccionales();
  // ############## MENU MODO MANUAL #################
  }else if(menuNum==1){
    // Linea1 - Zona
    switch(indexMenu){
      case 0:
        lcd.print("PotenciaGeneral");
        break;
      case 1:
        lcd.print("Piernas");
        break;
      case 2:
        lcd.print("Lumbares");
        break;
      case 3:
        lcd.print("Dorsales");
        break;
      case 4:
        lcd.print("Cervicales");
        break;
    }
    // Linea2 - Imprimir la barra de potencia
    int bloquesCompletos=0;
    if(indexMenu!=0) bloquesCompletos = getBloques(indexMenu-1);
    else bloquesCompletos = getBloques(0);
    lcd.setCursor(0,1);
    for(int i=0;i<bloquesCompletos;i++) lcd.write(4);
    dibujarFlechasDireccionales();
  // ############## MENU RELOJ #################
  }else if(menuNum==2){
    dibujarMenuInicial();
  }

  delay(200);
}

// ######### INICIAR MASAJE #########
void iniciarMasaje(){
  setMotoresAll(0);
  actualizarMotores();
  switch (indexMenu) {
    case 1:
      masajeRelajante();
      break;
    case 2:
      masajePulsante();
      break;
    case 3:
      masajeVariado();
      break;
    case 4:
      masajeIncremental();
      break;
    case 5:
      masajeFuerte();
      break;
  }
  masajeEnCurso=false;
  setMotoresAll(0);
  actualizarMotores();
  refrescarLCD();
}

// ######## ESCUCHAR BOTONERA ########
void escucharBotonera(){
  boolean pulsado=false;
  do{
    if(backPulsado) {
      pulsado=true;
      backPulsado=false;
    } else if(loginPulsado){
      pulsado=true;
      loginPulsado=false;
    } else if(digitalRead(botonOk)){
      if(menuNum==0 && indexMenu==0) menuNum=1;
      else if(menuNum==0 && indexMenu!=0) masajeEnCurso=true;
      else if(menuNum==2) menuNum=0;
      pulsado=true;
    }
    else if(digitalRead(botonArriba)){
      indexMenu--;
      pulsado=true;
    }
    else if(digitalRead(botonAbajo)){
      indexMenu++;
      pulsado=true;
    }
    else if(digitalRead(botonMas)){
      if(menuNum==1){
        switch(indexMenu){
          case 0:
            incrementarPotencia(0);
            setMotoresAll(potenciaMotores[0]);
            break;
          case 1:
            incrementarPotencia(0);
            break;
          case 2:
            incrementarPotencia(1);
            break;
          case 3:
            incrementarPotencia(2);
            break;
          case 4:
            incrementarPotencia(3);
            break;
        }
        actualizarMotores();
      }
      pulsado=true;
    }
    else if(digitalRead(botonMenos)){
      if(menuNum==1){
        switch(indexMenu){
         case 0:
            decrementarPotencia(0);
            setMotoresAll(potenciaMotores[0]);
            break;
          case 1:
            decrementarPotencia(0);
            break;
          case 2:
            decrementarPotencia(1);
            break;
          case 3:
            decrementarPotencia(2);
            break;
          case 4:
            decrementarPotencia(3);
            break;
        }
        actualizarMotores();
      }
      pulsado=true;
    }
  }while(!pulsado);

  // Evitamos salirnos del menu
  if(menuNum==0){
    if(indexMenu>5) indexMenu=0;
    else if(indexMenu<0) indexMenu=5;
  }else if(menuNum==1){
    if(indexMenu>4) indexMenu=0;
    else if(indexMenu<0) indexMenu=4;
  }
  refrescarLCD();
  if(masajeEnCurso) iniciarMasaje();
}

// ################### IGNORAR INTERRUPCIONES #################
// Obliga a hacer un delay entre interrupciones
bool ignorarInterrupciones(){
  if((millis()-ultimaInterrupcion)>800){
    ultimaInterrupcion=millis();
    return false;  
  }
  else return true;
}

// ################### BACK INTERRUPCION ###################

void backInterrupcion(){
  if(!ignorarInterrupciones()){
    if(menuNum==1) menuNum=0;
    else if(menuNum==0) menuNum=2;
    backPulsado=true;
  }
}

// ################### LOGIN INTERRUPCION ###################
void loginInterrupcion(){
  if(!ignorarInterrupciones()){
    setMotoresAll(0);
    actualizarMotores();
    if(!logueado) menuNum=0;
    else menuNum=2;
    logueado=!logueado;
    loginPulsado=true;
  }
}

// ##########################################################
// ######################### SETUP ##########################
// ##########################################################

void setup() {
  Serial.begin(9600);
  delay(200);
  //### LCD ####
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(200);
  lcd.createChar(0, p1);
  lcd.createChar(1, p2);
  lcd.createChar(2, p3);
  lcd.createChar(3, p4);
  lcd.createChar(4, p5);
  lcd.createChar(5, p6);
  lcd.createChar(6, p7);
  lcd.createChar(7, p8);

  // #### RELOJ ####
  // Linea para volver a establecer la hora 
  //myRTC.setDS1302Time(00, 07, 22, 7, 11, 12, 2016); // (segundos, minutos, hora, dia da semana, dia del mes, mes, año)


  //### PINES ###
  pinMode(MotorA,OUTPUT);
  pinMode(MotorB,OUTPUT);
  pinMode(MotorC,OUTPUT);
  pinMode(MotorD,OUTPUT);

  pinMode(botonOk, INPUT);
  pinMode(botonArriba,INPUT);
  pinMode(botonAbajo,INPUT);
  pinMode(botonMas,INPUT);
  pinMode(botonMenos,INPUT);

  // #### Interrupciones ####
  // # Interrupcion         0    1     2    3    4    5
  // # MEGA(PIN)            2    3    21   20   19   18
  pinMode(2, INPUT);
  attachInterrupt(0, backInterrupcion, RISING);
  pinMode(3, INPUT);
  attachInterrupt(1, loginInterrupcion, RISING);
  ultimaInterrupcion=millis();

  refrescarLCD();
}


// ##########################################################
// ########################## LOOP ##########################
// ##########################################################
void loop() {
  if(logueado) escucharBotonera();
  if(menuNum==2 && ((millis()-ultimoRefresco)>60000)) refrescarLCD();  // Refresca el LCD para actualizar los minutos
}
