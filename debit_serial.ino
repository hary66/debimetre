//
//#define impulse_topic "sensor/impulse" //Topic débit
//#define volume_topic "sensor/volume" //Topic volume
//#define debit_topic "sensor/debit" //Topic débit

//  marron  masse
//  jaune   signal
//  bleu  +5V_DC
//  ############  Déclaration des variables ##################

const byte ledPin = 13;
bool etatLedPin = false;
const byte onduleurPin = 7;
const byte PcPompePin = 6;
const byte interruptPin = 2;
volatile byte interruptCounter = 0;
unsigned long numberOfInterrupts = 0;

unsigned long now = 0;
//unsigned long last = 0;
unsigned int timerMonitor = 1000;
unsigned int timerLed = 1000;

//  ############  Déclaration des fonctions ##################
void update_interruptCounter();
void update_monitor(unsigned int const &timer);
void handleInterrupt();
float volume_pompe ();   //  renvoie volume en litre / heure
float debit();  //  renvoie débit en litre / heure
float debit(int const &dt);
void update_led(unsigned int const &timer);

void setup() {
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(onduleurPin, OUTPUT);
  digitalWrite(onduleurPin, HIGH);
  pinMode(PcPompePin, OUTPUT);
  digitalWrite(PcPompePin, HIGH);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
}

void loop() {
  now = millis();
  //Envoi d'un message par minute
  //  unsigned long t = millis();
  //
  //  if (now - lastMsg > 1000 * 10) {
  //    lastMsg = now;
  //    client.publish(impulse_topic, String(numberOfInterrupts).c_str(), true); //Publie le nombre d'impulsions le topic impulse_topic
  //    client.publish(volume_topic, String(volume_pompe ()).c_str(), true); //Publie le volume total depuis démarrage
  //    client.publish(debit_topic, String(q).c_str(), true); //Publie le débit le topic débit_topic
  //    client.publish(temp_topic, String(tempDev[1]).c_str(), true); //Publie la température capteur N°1
  //    //client.publish(temp_topic, String(sensors.getTempCByIndex(0)).c_str(), true); //Publie la température le topic temp_topic
  //  }
  update_interruptCounter();
  update_monitor(timerMonitor);
  update_led(timerLed);
  //  if (debit() < 200) {
  //    pompeOff(true);
  //    timerLed = 100;
  //  }
}

//  ############  Définition des fonctions ##################
void handleInterrupt() {
  interruptCounter++;
}
void update_interruptCounter() {
  if (interruptCounter > 0) {
    interruptCounter--;
    numberOfInterrupts++;
    //Serial.print("An interrupt has occurred. Total: ");
    //Serial.print(numberOfInterrupts);
    //    Serial.print("    Volume total: ");
    //    Serial.print(volume_pompe ());
    //    Serial.print(" litres.    Débit : ");
    //    Serial.print(debit());
    //    Serial.print(" l/min");
  }
}

void update_monitor(unsigned int const &timer) {
  static unsigned long last = 0;
  if (now - last > timerMonitor) {
    Serial.print("<\t");
    Serial.print(now / 1000);
    Serial.print("\tnumberOfInterrupts : ");
    Serial.print(numberOfInterrupts);
    Serial.print("\tVolume : ");
    Serial.print(volume_pompe ());
    Serial.print(" l\t");
    Serial.print("Débit : ");
    Serial.print(debit());
    Serial.print(" l/min");
    Serial.print("\tDébit : ");
    Serial.print(debit(1));
    Serial.println(" l/min sur 10 minutes");
    last = now;
  }
}

float volume_pompe () {
  return numberOfInterrupts / 262.0; //volume total pompé en litres
}
// calcul du débit en litre /min
float debit() {
  //Serial.print("debit()");
  static unsigned long last_numberOfInterrupts = 0;
  static unsigned long last = 0;
  static float q = 0.0;
  int delta_t = now - last;
  if (delta_t > 15000) { //  toutes les 15 secondes
    q = ((numberOfInterrupts - last_numberOfInterrupts) / 262.0) / (delta_t / 60000.0);
    last_numberOfInterrupts = numberOfInterrupts;
    last = now;
  }
  return q;
}
// calcul du débit en litre /min
float debit(int const &dt) {
  static unsigned long last_numberOfInterrupts = 0;
  static unsigned long last = 0;
  static float q = 0.0;
  double delta_t = now - last;
  if (delta_t > (dt * 60000)) { //  toutes les 10min
    q = ((numberOfInterrupts - last_numberOfInterrupts) / 262.0) / (delta_t / 60000.0);
    last_numberOfInterrupts = numberOfInterrupts;
    last = now;
  }
  return q;
}
void update_led(unsigned int const & timer) {
  static unsigned long last = 0;
  if (now - last > timer) {
    digitalWrite (ledPin, !etatLedPin);
    last = now;
  }
}
//void pompeOff(bool &pompeOff) {
//  if (pompeOff) {
//    static unsigned long last = 0;
//    static bool procedure = false;
//    int timer = 500;
//    static bool INIT = true;
//    if (INIT) {
//        INIT = false;
//        last = now;
//      }
//    if (!procedure) {
//      PcPompeOff();
//      procedure = true;
//      else if ((procedure) && (now - last > timer)) {
//        onduleurOff();
//        procedure = false;
//        pompeOff = false;
//      }
//    }
//  }
//  void pompeOn(bool &pompeOn) {
//    if (pompeOn) {
//      static unsigned long last = 0;
//      static bool procedure = false;
//      int timer = 2000;
//      static bool INIT = true;
//      if (INIT) {
//        INIT = false;
//        last = now;
//      }
//      if (!procedure) {
//        onduleurOn();
//        procedure = true;
//      }
//      else if ((procedure) && (now - last > timer)) {
//        PcPompeOn();
//        procedure = false;
//        pompeOn = false;
//        INIT = true;
//      }
//    }
//  }
//
//  void PcPompeOff() {
//    digitalWrite (PcPompePin, LOW);
//  }
//  void PcPompeOn() {
//    digitalWrite (PcPompePin, HIGH);
//  }
//  void onduleurOff() {
//    digitalWrite (onduleurPin, LOW);
//  }
//  void onduleurOn() {
//    digitalWrite (onduleurPin, HIGH);
//  }
