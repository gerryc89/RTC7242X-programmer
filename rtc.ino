#define a0 2
#define a1 3
#define a2 4
#define a3 5
#define d0 9
#define d1 10
#define d2 11
#define d3 12
#define ncs0 7
#define cs1 8
#define nrd 6
#define nwr 13
int seconds = 0;
int minutes = 0;
int hours = 0;
int ampm = 0;
int days = 0;
int months = 0;
int years = 0;
int dayweek = 0;
int thirtyadj = 0;
int irqflag = 0;
int busy = 0;
int hold = 0;
int t1 = 0;
int t0 = 0;
int irqpinmode = 0;
int mask = 0;
int clockmode = 0;
int stoprtc = 0;
int reset = 0;

void setup() {
  //address pin configuration
  pinMode(a0, OUTPUT);
  digitalWrite(a0, LOW);
  pinMode(a1, OUTPUT);
  digitalWrite(a1, LOW);
  pinMode(a2, OUTPUT);
  digitalWrite(a2, LOW);
  pinMode(a3, OUTPUT);
  digitalWrite(a3, LOW);

  //data pin initial input state
  pinMode(d0, INPUT);
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  pinMode(d3, INPUT);

  //selection pin configuration
  pinMode(ncs0, OUTPUT);
  digitalWrite(ncs0, HIGH);
  pinMode(cs1, OUTPUT);
  digitalWrite(cs1, LOW);
  pinMode(nrd, OUTPUT);
  digitalWrite(nrd, HIGH);
  pinMode(nwr, OUTPUT);
  digitalWrite(nwr, HIGH);

  //serial
  Serial.begin(9600);
  readreg();
  menuintro();
}

void loop() {
  int menusel = 0;
  if (Serial.available() > 0) {
    menusel = Serial.read() - 48;
    switch (menusel) {
      case 1: disptime();
        break;
      case 2: edittime();
        break;
      case 3: dispreg();
        break;
      case 4: editreg();
        break;
      case 5: initrtc();
        break;
      case 6: restart();
        break;
      case 7: standby();
        break;
      case 8: stopcount();
        break;
    }

  }
}
void menuintro() {
  Serial.setTimeout(100000);
  Serial.println("RTC7242X programmer");
  Serial.println("select a funcion:");
  Serial.println("1: for read actual time");
  Serial.println("2: for edit time");
  Serial.println("3: for read actual conf registers status");
  Serial.println("4: for edit conf registers");
  Serial.println("5: for init rtc");
  Serial.println("6: for restart rtc");
  Serial.println("7: for standby rtc");
  Serial.println("8: for stop rtc count");

}

void pause() {
  serclear();
  Serial.println("Press a key to continue");
  while (Serial.available() == 0) {};
  serclear();
}
void serclear() {
  while (Serial.available() != 0) {
    Serial.read();
  }
}

void initrtc() {
  readreg();
  reset = 1;
  writereg();
  Serial.println("Init ok");
  pause();
  menuintro();
}

void stopcount() {
  readreg();
  stoprtc = 1;
  reset = 1;
  writereg();
  Serial.println("stop count ok");
  pause();
  menuintro();
}
void disptime() {
  readtime();
  Serial.println("the actual time set is:");
  Serial.print("Year : ");
  Serial.println(years, DEC);
  Serial.print("Months : ");
  Serial.println(months, DEC);
  Serial.print("Days : ");
  Serial.println(days, DEC);
  Serial.print("Week day number : ");
  Serial.println(dayweek, DEC);
  Serial.print("Hours : ");
  Serial.println(hours, DEC);
  if (clockmode == 0) {
    Serial.print("0 for AM, 1 for PM : ");
    Serial.println(ampm, DEC);
  }
  Serial.print("Minutes : ");
  Serial.println(minutes, DEC);
  Serial.print("Seconds : ");
  Serial.println(seconds, DEC);
  pause();
  menuintro();
}

void edittime() {
  char yearschar[2] = {0, 0};
  char monthschar[2] = {0, 0};
  char dayschar[2] = {0, 0};
  char weekchar[1] = {0};
  char ampmchar[1] = {0};
  char hourschar[2] = {0, 0};
  char minuteschar[2] = {0, 0};
  char secondschar[2] = {0, 0};

  Serial.println("Enter new values for time set:");
  Serial.print("Years : ");
  Serial.readBytes(yearschar, 2);
  years = min(((yearschar[0] - 48) * 10) + (yearschar[1] - 48), 99);
  Serial.println(years, DEC);
  Serial.print("Months : ");
  Serial.readBytes(monthschar, 2);
  months = min(((monthschar[0] - 48) * 10) + (monthschar[1] - 48), 12);
  Serial.println(months, DEC);
  Serial.print("Days : ");
  Serial.readBytes(dayschar, 2);
  days = min(((dayschar[0] - 48) * 10) + (dayschar[1] - 48), 31);
  Serial.println(days, DEC);
  Serial.print("Week day number : ");
  Serial.readBytes(weekchar, 1);
  dayweek = min((weekchar[0] - 48), 6);
  Serial.println(dayweek, DEC);
  if (clockmode == 0) {
    Serial.print("0 for AM, 1 for PM : ");
    Serial.readBytes(ampmchar, 1);
    ampm = min(((ampmchar[0] - 48) * 10), 1);
    Serial.println(ampm, DEC);
    Serial.print("Hours : ");
    Serial.readBytes(hourschar, 2);
    hours = min(((hourschar[0] - 48) * 10) + (hourschar[1] - 48), 12);
  }
  else {
    Serial.print("Hours : ");
    Serial.readBytes(hourschar, 2);
    hours = min(((hourschar[0] - 48) * 10) + (hourschar[1] - 48), 23);
  }
  Serial.println(hours, DEC);
  Serial.print("Minutes : ");
  Serial.readBytes(minuteschar, 2);
  minutes = min(((minuteschar[0] - 48) * 10) + (minuteschar[1] - 48), 59);
  Serial.println(minutes, DEC);
  Serial.print("Seconds : ");
  Serial.readBytes(secondschar, 2);
  seconds = min(((secondschar[0] - 48) * 10) + (secondschar[1] - 48), 59);
  Serial.println(seconds, DEC);
  Serial.println("Confirm?");
  int validopt = 0;
  while (validopt == 0) {
    int opt = Serial.read();
    if (opt == 121 || opt == 89) {
      writetime();
      validopt = 1;
      Serial.println("New date writed");
    }
    if (opt == 110 || opt == 78) {
      validopt = 1;
      Serial.println("Not confirmed");
    }
  }
  validopt = 0;
  pause();
  menuintro();
}

void dispreg() {
  readreg();
  Serial.println("The actual conf reg state");
  Serial.print("T1 : ");
  Serial.println(t1, DEC);
  Serial.print("T0 : ");
  Serial.println(t0, DEC);
  Serial.print("ITRPT STND : ");
  Serial.println(irqpinmode, DEC);
  Serial.print("MASK : ");
  Serial.println(mask, DEC);
  Serial.print("24 12 MODE : ");
  Serial.println(clockmode, DEC);
  Serial.print("STOP : ");
  Serial.println(stoprtc, DEC);
  pause();
  menuintro();
}

void editreg() {
  char t1char[1] = {0};
  char t0char[1] = {0};
  char irqpinmodechar[1] = {0};
  char maskchar[1] = {0};
  char clockmodechar[1] = {0};
  char stoprtcchar[1] = {0};
  Serial.println("Enter new values for reg config:");
  Serial.print("T1 : ");
  Serial.readBytes(t1char, 1);
  t1 = min((t1char[0] - 48), 1);
  Serial.println(t1, DEC);
  Serial.print("T0 : ");
  Serial.readBytes(t0char, 1);
  t0 = min((t0char[0] - 48), 1);
  Serial.println(t0, DEC);
  Serial.print("ITRPT STND : ");
  Serial.readBytes(irqpinmodechar, 1);
  irqpinmode = min((irqpinmodechar[0] - 48), 1);
  Serial.println(irqpinmode, DEC);
  Serial.print("MASK : ");
  Serial.readBytes(maskchar, 1);
  mask = min((maskchar[0] - 48), 1);
  Serial.println(mask, DEC);
  Serial.print("24 12 MODE : ");
  Serial.readBytes(clockmodechar, 1);
  clockmode = min((clockmodechar[0] - 48), 1);
  Serial.println(clockmode, DEC);
  Serial.print("STOP : ");
  Serial.readBytes(stoprtcchar, 1);
  stoprtc = min((stoprtcchar[0] - 48), 1);
  Serial.println(stoprtc, DEC);
  Serial.println("Confirm?");
  int validopt = 0;
  while (validopt == 0) {
    int opt = Serial.read();
    if (opt == 121 || opt == 89) {
      writereg();
      validopt = 1;
      Serial.println("New reg conf writed");
    }
    if (opt == 110 || opt == 78) {
      validopt = 1;
      Serial.println("Not confirmed");
    }
  }
  validopt = 0;
  pause();
  menuintro();
}

void restart() {
  readreg();
  reset = 1;
  stoprtc = 0;
  writereg();
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  digitalWrite(cs1, LOW);
  delayMicroseconds(100);
  readreg();
  Serial.println("restart ok");
  pause();
  menuintro();
}
void standby() {
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  digitalWrite(cs1, LOW);
  Serial.println("Standby rtc ok, can you disconnect");
  pause();
  menuintro();
}

//address conversion from decimal and chip slection line activation
void chipsel(int address) {
  int addpins[4] = {0, 0, 0, 0};
  if (address != 0) {
    int qoz = address;
    int i = 0;
    do {
      addpins[i] = qoz % 2;
      qoz = qoz / 2;
      i++;
    } while (qoz != 0);
  }
  digitalWrite(cs1, HIGH);
  delayMicroseconds(10);
  digitalWrite(ncs0, LOW);

  //debug
  //Serial.print("address = ");
  //Serial.print(address, DEC);
  //Serial.print("  bits = ");
  //Serial.print(addpins[3], DEC);
  //Serial.print(addpins[2], DEC);
  //Serial.print(addpins[1], DEC);
  //Serial.print(addpins[0], DEC);
  //Serial.print("  ");


  digitalWrite(a0, addpins[0]);
  digitalWrite(a1, addpins[1]);
  digitalWrite(a2, addpins[2]);
  digitalWrite(a3, addpins[3]);
  //delayMicroseconds(10);
}

void holdrtc() {
  hold = 1;
  writereg();
  do {
    readreg();
  } while (busy != 0);
}

//data read from address
int readdata(int address) {
  //holdrtc()
  chipsel(address);
  return dataout();
}

//data write to address
void writedata(int address, int data) {
  //holdrtc()
  chipsel(address);
  datain(data);
}

//read total time
void readtime() {
  seconds = readdata(0) + (readdata(1) * 10);
  minutes = readdata(2) + (readdata(3) * 10);
  hours = readdata(4);
  int hours10 = readdata(5);
  if (clockmode == 0 && hours10 >= 4) {
    ampm = 1;
    hours10 = hours10 - 4;
  }
  else {
    ampm = 0;
  }
  hours = hours + (hours10 * 10);
  days = readdata(6) + (readdata(7) * 10);
  months = readdata(8) + (readdata(9) * 10);
  years = readdata(10) + (readdata(11) * 10);
  dayweek = readdata(12);
}

//write new time
void writetime () {
  writedata(0, seconds % 10);
  writedata(1, seconds / 10);
  writedata(2, minutes % 10);
  writedata(3, seconds / 10);
  writedata(4, hours % 10);
  if (clockmode == 0 && ampm == 1) {
    writedata(5, ((hours / 10) + 4));
  }
  else {
    writedata(5, hours / 10);
  }
  writedata(6, days % 10);
  writedata(7, days / 10);
  writedata(8, months % 10);
  writedata(9, months / 10);
  writedata(10, years % 10);
  writedata(11, years / 10);
  writedata(12, dayweek);
}

//read registers
void readreg () {
  chipsel(13);
  digitalWrite(nrd, LOW);
  delayMicroseconds(10);
  hold = digitalRead(d0);
  busy = digitalRead(d1);
  irqflag = digitalRead(d2);
  thirtyadj = digitalRead(d3);
  digitalWrite(nrd, HIGH);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  chipsel(14);
  digitalWrite(nrd, LOW);
  delayMicroseconds(10);
  mask = digitalRead(d0);
  irqpinmode = digitalRead(d1);
  t0 = digitalRead(d2);
  t1 = digitalRead(d3);
  digitalWrite(nrd, HIGH);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  chipsel(15);
  digitalWrite(nrd, LOW);
  delayMicroseconds(10);
  reset = digitalRead(d0);
  stoprtc = digitalRead(d1);
  clockmode = digitalRead(d2);
  digitalWrite(nrd, HIGH);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
}

//write registers
void writereg () {
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  digitalWrite(d0, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  chipsel(13);
  digitalWrite(d0, hold);
  digitalWrite(d1, busy);
  digitalWrite(d2, irqflag);
  digitalWrite(d3, thirtyadj);
  delayMicroseconds(10);
  digitalWrite(nwr, LOW);
  delayMicroseconds(10);
  digitalWrite(nwr, HIGH);
  delayMicroseconds(10);
  digitalWrite(d0, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  chipsel(14);
  digitalWrite(d0, mask);
  digitalWrite(d1, irqpinmode);
  digitalWrite(d2, t0);
  digitalWrite(d3, t1);
  delayMicroseconds(10);
  digitalWrite(nwr, LOW);
  delayMicroseconds(10);
  digitalWrite(nwr, HIGH);
  delayMicroseconds(10);
  digitalWrite(d0, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  chipsel(15);
  digitalWrite(d0, reset);
  digitalWrite(d1, stoprtc);
  digitalWrite(d2, clockmode);
  digitalWrite(d3, LOW);
  delayMicroseconds(10);
  digitalWrite(nwr, LOW);
  delayMicroseconds(10);
  digitalWrite(nwr, HIGH);
  delayMicroseconds(10);
  digitalWrite(d0, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  pinMode(d0, INPUT);
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  pinMode(d3, INPUT);
  delayMicroseconds(10);
}

//data output conversion
int dataout () {
  digitalWrite(nrd, LOW);
  delayMicroseconds(1);
  int dat0 = digitalRead(d0);
  int dat1 = digitalRead(d1);
  int dat2 = digitalRead(d2);
  int dat3 = digitalRead(d3);
  digitalWrite(nrd, HIGH);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);
  int dec8 = dat3 * 8;
  int dec4 = dat2 * 4;
  int dec2 = dat1 * 2;
  int data = dat0 + dec2 + dec4 + dec8;

  //debug
  //Serial.print("data = ");
  //Serial.print(dat3, DEC);
  //Serial.print(dat2, DEC);
  //Serial.print(dat1, DEC);
  //Serial.print(dat0, DEC);
  //Serial.print(" = ");
  //Serial.println(data, DEC);

  return data;
}

//data input conversion
void datain(int data) {
  pinMode(d0, OUTPUT);
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  digitalWrite(d0, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);

  int datapins[4] = {0, 0, 0, 0};
  if (data != 0) {
    int qoz = data;
    int i = 0;
    do {
      datapins[i] = qoz % 2;
      qoz = qoz / 2;
      i++;
    } while (qoz != 0);
  }
  digitalWrite(d0, datapins[0]);
  digitalWrite(d1, datapins[1]);
  digitalWrite(d2, datapins[2]);
  digitalWrite(d3, datapins[3]);
  delayMicroseconds(10);
  digitalWrite(nwr, LOW);
  delayMicroseconds(10);
  digitalWrite(nwr, HIGH);
  delayMicroseconds(10);
  digitalWrite(d0, LOW);
  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  pinMode(d0, INPUT);
  pinMode(d1, INPUT);
  pinMode(d2, INPUT);
  pinMode(d3, INPUT);
  delayMicroseconds(10);
  digitalWrite(ncs0, HIGH);
  delayMicroseconds(10);

  //debug
  //Serial.print("data = ");
  //Serial.print(data, DEC);
  //Serial.print(" = ");
  //Serial.print(datapins[3], DEC);
  //Serial.print(datapins[2], DEC);
  //Serial.print(datapins[1], DEC);
  //Serial.println(datapins[0], DEC);
}

