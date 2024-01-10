unsigned int number;

bool tombDoorRS1 = false;
bool tombDoorRS2 = true;

bool pin = false;

String dmStatus = "a";

String lastCommand;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(300);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(5, INPUT_PULLUP);
  
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop() {
  number++;

  pin = digitalRead(5) == 0;

  executeCommands();

  mega123State();

  // audioState();

  delay(100);
}

void audioState() {
  Serial.print("{i.b.n:AUDIO}");

  Serial.print("{s.d.c:" + String(lastCommand) + "}");
  Serial.print("{s.d.n:" + String(number) + "}");
  Serial.print("{s.d.t:" + String(millis()) + "}");

  Serial.print("{s.p.s:1001/0/0/3005}");
}

void mega123State() {
  digitalWrite(2, tombDoorRS1 ? HIGH : LOW);
  digitalWrite(3, tombDoorRS2 ? HIGH : LOW);

  Serial.print("{i.b.n:MEGA_123}");

  Serial.print("{s.d.c:" + String(lastCommand) + "}\n");
  Serial.print("{s.d.n:" + String(number) + "}\n");
  Serial.print("{s.d.t:" + String(millis()) + "}\n");

  //Serial.print("{s.b.o:" + String(random(0, 2)) + "}");
  //Serial.print("{s.b.s:" + String(random(0, 5)) + "}");
  //Serial.print("{s.b.v:" + String(random(250, 420) / 100.0) + "}");
  //Serial.print("{s.btn.s:" + String(random(0, 2)) + "}");
  //Serial.print("{s.fd.s:" + String(random(0, 2)) + "}");
  //Serial.print("{s.gd.s:" + String(random(0, 2)) + String(random(0, 2)) + "}");
  //Serial.print("{s.h.m:" + String(random(0, 2)) + "}");
  //Serial.print("{s.r.o:" + String(random(0, 2)) + "}");
  Serial.print("{s.t.s:" + String(!digitalRead(7)) + String(!digitalRead(8)) + String(!digitalRead(9)) + String(!digitalRead(10)) + String(!digitalRead(11)) + String(!digitalRead(12)) + "}\n");
  Serial.print("{s.td.s:" + String(tombDoorRS1) + String(tombDoorRS2) + "}\n");

  dmStatus = pin ? "s" : "a";

  Serial.print("{s.dm.s:" + dmStatus + "}");
}

void executeCommands() {
  while (1) {
    // Отдельная команда
    String command = Serial.readStringUntil(']');
    // Значение, необязательно
    String commandValue = "";

    // Если команды не поступило, прерываем цикл
    if (command.length() == 0) {
      break;
    }

    // Если команда начинается не с [, пропускаем
    if (command.substring(0, 1) != "[") {
      continue;
    }

    // Команда без начально [
    command = command.substring(1);

    lastCommand = command;

    int delimeterIndex = command.lastIndexOf("=");

    if (delimeterIndex >= 0) {
      commandValue = command.substring(delimeterIndex + 1);
      command = command.substring(0, delimeterIndex);
    }

    if (lastCommand == "led=1") {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
    } else if (lastCommand == "led=0") {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
    } else if (command == "md_2rm") {
      tombDoorRS1 = commandValue == "1";
      tombDoorRS2 = commandValue == "0";
    } else {
      Serial.print("{r.c." + lastCommand + ":not_exist}\n");
      continue;
    }

    Serial.print("{r.c." + lastCommand + ":completed}\n");
  }
}
