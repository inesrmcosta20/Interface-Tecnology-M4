
const int trigPin   = 9;
const int echoPin   = 10;
const int pinoBotao = 12;
const int buzzer    = 11;


const float DIST_MIN   = 10.0;   // perto
const float DIST_MAX   = 350.0;  //  longe
const int   FREQ_GRAVE = 200;    
const int   FREQ_AGUDO = 2800;  
const int   DURACAO_BIP = 30;     

int  ritmoAtual       = 1000;
bool pessoasDetetadas = false;
bool emBip            = false;
unsigned long ultimoBip  = 0;
unsigned long inicioBip  = 0;


String serialBuffer = "";

unsigned long ultimaMedicao = 0;
float distanciaAtual = DIST_MAX;


void setup() {
  Serial.begin(9600);
  pinMode(trigPin,   OUTPUT);
  pinMode(echoPin,   INPUT);
  pinMode(pinoBotao, INPUT_PULLUP);
  pinMode(buzzer,    OUTPUT);
  noTone(buzzer);
}


void loop() {
  unsigned long agora = millis();

  
  lerSerial();


  if (agora - ultimaMedicao >= 60) {
    ultimaMedicao = agora;
    distanciaAtual = lerDistancia();

    Serial.print((int)distanciaAtual);
    Serial.print(",");
    Serial.println(digitalRead(pinoBotao));
  }

  bool ativo = pessoasDetetadas || (distanciaAtual < DIST_MAX);

  if (!ativo) {
 
    if (emBip) {
      noTone(buzzer);
      emBip = false;
    }
    ultimoBip = agora; 
    return;
  }

  if (emBip) {
    
    if (agora - inicioBip >= DURACAO_BIP) {
      noTone(buzzer);   
      emBip = false;
    }
  } else if (agora - ultimoBip >= (unsigned long)ritmoAtual)  {
    
      int freq = (int) mapFloat(distanciaAtual, DIST_MIN, DIST_MAX, FREQ_GRAVE, FREQ_AGUDO);
      freq = constrain(freq, FREQ_GRAVE, FREQ_AGUDO);

      tone(buzzer, freq); 
      emBip     = true;
      inicioBip = agora;
      ultimoBip = agora;
    }
  }
}

void lerSerial() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      serialBuffer.trim();
      if (serialBuffer.length() > 0) {
        int v = serialBuffer.indexOf(',');
        if (v > 0) {
          int r  = serialBuffer.substring(0, v).toInt();
          int c2 = serialBuffer.substring(v + 1).toInt();
          if (r >= 100 && r <= 2000) ritmoAtual = r;
          pessoasDetetadas = (c2 == 1);
        }
      }
      serialBuffer = "";
    } else {
      serialBuffer += c;
    }
  }
}

float lerDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 25000); // timeout 25ms
  if (duracao == 0) return DIST_MAX;
  return (duracao * 0.0343) / 2.0;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  x = constrain(x, in_min, in_max);
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
