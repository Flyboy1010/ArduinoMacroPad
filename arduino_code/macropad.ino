

// Pins used
const int btnPin_1 = 3;
const int btnPin_2 = 2;
const int ledPin_1 = 8;
const int ledPin_2 = 7;

// Pin state
int btnState_1 = 0; // Variable for the state of button 1
int btnState_2 = 0;

int valor;
int valor_0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // set baud rate to math the communication rate with the program
  pinMode(btnPin_1, INPUT);
  pinMode(btnPin_2, INPUT);
  pinMode(ledPin_1, OUTPUT);
  pinMode(ledPin_2, OUTPUT);
  valor_0 = analogRead(A0);

}

void loop() {
  // put your main code here, to run repeatedly  
  valor = analogRead(A0);

  if (valor > (valor_0 + 10)){
    valor_0 = analogRead(A0);
    Serial.println("Bajar");       
  
  }   else if (valor < (valor_0 - 10)) {
    valor_0 = analogRead(A0);
    Serial.println("Subir");

  }

  if (Serial.available()){
    // Read incoming data
    // char incomingData = Serial.read();
  }
  btnState_1 = digitalRead(btnPin_1);
  btnState_2 = digitalRead(btnPin_2);


  // Do something with data
  if (btnState_1 == HIGH) {
    Serial.print("Comando1\n");
    digitalWrite(ledPin_1, HIGH);     

  } else if (btnState_1 == LOW) {
    digitalWrite(ledPin_1, LOW);

  }
  
  if (btnState_2 == HIGH) {
    Serial.print("Comando2\n");
    digitalWrite(ledPin_2, HIGH);
    
  } else if (btnState_2 == LOW) {
    digitalWrite(ledPin_2, LOW);

  }

  delay(150);

}
