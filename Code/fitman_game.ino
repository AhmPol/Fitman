#include <LiquidCrystal.h>

#define PIN_AUTOPLAY 1
#define PIN_CONTRAST 12

#define SPRITE_JUMP_UPPER 0
#define SPRITE_RUN1 1
#define SPRITE_RUN2 2
#define SPRITE_JUMP 3
#define SPRITE_JUMP_UPPER '.'     
#define SPRITE_JUMP_LOWER 4
#define SPRITE_TERRAIN_EMPTY ' '
#define SPRITE_TERRAIN_SOLID 5
#define SPRITE_TERRAIN_SOLID_RIGHT 6
#define SPRITE_TERRAIN_SOLID_LEFT 7

#define HERO_HORIZONTAL_POSITION 1

#define TERRAIN_WIDTH 16
#define TERRAIN_EMPTY 0
#define TERRAIN_LOWER_BLOCK 1
#define TERRAIN_UPPER_BLOCK 2

#define HERO_POSITION_OFF 0
#define HERO_POSITION_RUN_LOWER_1 1
#define HERO_POSITION_RUN_LOWER_2 2
#define HERO_POSITION_JUMP_1 3
#define HERO_POSITION_JUMP_2 4
#define HERO_POSITION_JUMP_3 5
#define HERO_POSITION_JUMP_4 6
#define HERO_POSITION_JUMP_5 7
#define HERO_POSITION_JUMP_6 8
#define HERO_POSITION_JUMP_7 9
#define HERO_POSITION_JUMP_8 10
#define HERO_POSITION_RUN_UPPER_1 11
#define HERO_POSITION_RUN_UPPER_2 12

int rs = 11;
int e = 10;
int d4 = 9;
int d5 = 8;
int d6 = 3;
int d7 = 4;
LiquidCrystal lcd(11, 10, 9, 8, 3, 4);  // Match wiring
String state = "";  // Global variable to track pose state
unsigned long previousMillis = 0;
const long interval = 50;

class SideScrollerGame {
  char terrainUpper[TERRAIN_WIDTH + 1];
  char terrainLower[TERRAIN_WIDTH + 1];

  byte heroPos;
  byte newTerrainType;
  byte newTerrainDuration;
  bool playing;
  bool blink;
  unsigned int distance;
  unsigned long lastInputTime = 0;
  const unsigned long inputTimeout = 1000; // 1 second timeout

public:
  SideScrollerGame() : heroPos(HERO_POSITION_RUN_LOWER_1), newTerrainType(TERRAIN_EMPTY),
    newTerrainDuration(1), playing(false), blink(false), distance(0) {}

  void initializeGraphics() {
    static byte graphics[] = {
      B01100, B01100, B00000, B01110, B11100, B01100, B11010, B10011, // Run 1
      B01100, B01100, B00000, B01100, B01100, B01100, B01100, B01110, // Run 2
      B01100, B01100, B00000, B11110, B01101, B11111, B10000, B00000, // Jump
      B11110, B01101, B11111, B10000, B00000, B00000, B00000, B00000, // Jump lower
      B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111, // Ground
      B00011, B00011, B00011, B00011, B00011, B00011, B00011, B00011, // Ground right
      B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000  // Ground left
    };

    for (int i = 0; i < 7; ++i) {
      lcd.createChar(i + 1, &graphics[i * 8]);
    }

    for (int i = 0; i < TERRAIN_WIDTH; ++i) {
      terrainUpper[i] = SPRITE_TERRAIN_EMPTY;
      terrainLower[i] = SPRITE_TERRAIN_EMPTY;
    }
  }

  void advanceTerrain(char* terrain, byte newTerrain) {
    for (int i = 0; i < TERRAIN_WIDTH; ++i) {
      char current = terrain[i];
      char next = (i == TERRAIN_WIDTH - 1) ? newTerrain : terrain[i + 1];

      switch (current) {
        case SPRITE_TERRAIN_EMPTY:
          terrain[i] = (next == SPRITE_TERRAIN_SOLID) ? SPRITE_TERRAIN_SOLID_RIGHT : SPRITE_TERRAIN_EMPTY;
          break;
        case SPRITE_TERRAIN_SOLID:
          terrain[i] = (next == SPRITE_TERRAIN_EMPTY) ? SPRITE_TERRAIN_SOLID_LEFT : SPRITE_TERRAIN_SOLID;
          break;
        case SPRITE_TERRAIN_SOLID_RIGHT:
          terrain[i] = SPRITE_TERRAIN_SOLID;
          break;
        case SPRITE_TERRAIN_SOLID_LEFT:
          terrain[i] = SPRITE_TERRAIN_EMPTY;
          break;
      }
    }
  }

  bool drawHero(byte position) {
    bool collide = false;
    char upperSave = terrainUpper[HERO_HORIZONTAL_POSITION];
    char lowerSave = terrainLower[HERO_HORIZONTAL_POSITION];
    byte upper, lower;

    switch (position) {
      case HERO_POSITION_OFF:
        upper = lower = SPRITE_TERRAIN_EMPTY;
        break;
      case HERO_POSITION_RUN_LOWER_1:
        upper = SPRITE_TERRAIN_EMPTY;
        lower = SPRITE_RUN1;
        break;
      case HERO_POSITION_RUN_LOWER_2:
        upper = SPRITE_TERRAIN_EMPTY;
        lower = SPRITE_RUN2;
        break;
      case HERO_POSITION_JUMP_1:
      case HERO_POSITION_JUMP_8:
        upper = SPRITE_TERRAIN_EMPTY;
        lower = SPRITE_JUMP;
        break;
      case HERO_POSITION_JUMP_2:
      case HERO_POSITION_JUMP_7:
        upper = SPRITE_JUMP_UPPER;
        lower = SPRITE_JUMP_LOWER;
        break;
      case HERO_POSITION_JUMP_3:
      case HERO_POSITION_JUMP_4:
      case HERO_POSITION_JUMP_5:
      case HERO_POSITION_JUMP_6:
        upper = SPRITE_JUMP;
        lower = SPRITE_TERRAIN_EMPTY;
        break;
      case HERO_POSITION_RUN_UPPER_1:
        upper = SPRITE_RUN1;
        lower = SPRITE_TERRAIN_EMPTY;
        break;
      case HERO_POSITION_RUN_UPPER_2:
        upper = SPRITE_RUN2;
        lower = SPRITE_TERRAIN_EMPTY;
        break;
    }

    if (upper != ' ') {
      terrainUpper[HERO_HORIZONTAL_POSITION] = upper;
      collide = (upperSave != SPRITE_TERRAIN_EMPTY);
    }
    if (lower != ' ') {
      terrainLower[HERO_HORIZONTAL_POSITION] = lower;
      collide |= (lowerSave != SPRITE_TERRAIN_EMPTY);
    }

    terrainUpper[TERRAIN_WIDTH] = '\0';
    terrainLower[TERRAIN_WIDTH] = '\0';

    byte digits = (distance > 9999) ? 5 : (distance > 999) ? 4 : (distance > 99) ? 3 : (distance > 9) ? 2 : 1;

    char temp = terrainUpper[16 - digits];
    terrainUpper[16 - digits] = '\0';

    lcd.setCursor(0, 0);
    lcd.print(terrainUpper);
    terrainUpper[16 - digits] = temp;

    lcd.setCursor(0, 1);
    lcd.print(terrainLower);

    lcd.setCursor(16 - digits, 0);
    lcd.print(distance >> 3);

    terrainUpper[HERO_HORIZONTAL_POSITION] = upperSave;
    terrainLower[HERO_HORIZONTAL_POSITION] = lowerSave;

    return collide;
  }

  void update() {
    static String inputBuffer = "";
    static bool gotNewInput = false; // Declare gotNewInput as static so it keeps value between calls
    unsigned long currentMillis = millis(); // Define currentMillis using millis()

    gotNewInput = false; // Reset at start of each loop
    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') {
        inputBuffer.trim();
        float inputFloat = inputBuffer.toFloat();
        Serial.print("Input received: ");
        Serial.println(inputFloat);
        gotNewInput = true;

        if (inputFloat > 0.8) {
          state = "RUN";
          lastInputTime = currentMillis;
        } else if (inputFloat < 0.2) {
          state = "JUMP";
          lastInputTime = currentMillis;
        }

        inputBuffer = "";  // Clear buffer
      } else {
        inputBuffer += c;  // Accumulate characters
      }
    }


    // Optional: reset to default if too long since input
    if (!gotNewInput && currentMillis - lastInputTime > inputTimeout) {
      state = "RUN";  // fallback default
    }

        
    if (!playing) {
      drawHero((blink) ? HERO_POSITION_OFF : heroPos);
      if (blink) {
        lcd.setCursor(0, 0);
        lcd.print("Pose to play  ");
      }
      delay(250);
      blink = !blink;

      if (state == "RUN") {  // Start game only if pose is "1"
        initializeGraphics();
        heroPos = HERO_POSITION_RUN_LOWER_1;
        playing = true;
        distance = 0;
      }
      return;
    }

    // When playing:

    advanceTerrain(terrainLower, newTerrainType == TERRAIN_LOWER_BLOCK ? SPRITE_TERRAIN_SOLID : SPRITE_TERRAIN_EMPTY);
    advanceTerrain(terrainUpper, newTerrainType == TERRAIN_UPPER_BLOCK ? SPRITE_TERRAIN_SOLID : SPRITE_TERRAIN_EMPTY);

    if (--newTerrainDuration == 0) {
      if (newTerrainType == TERRAIN_EMPTY) {
        newTerrainType = (random(3) == 0) ? TERRAIN_UPPER_BLOCK : TERRAIN_LOWER_BLOCK;
        newTerrainDuration = 2 + random(10);
      } else {
        newTerrainType = TERRAIN_EMPTY;
        newTerrainDuration = 10 + random(10);
      }
    }

    if (state == "JUMP") {
      if (heroPos <= HERO_POSITION_RUN_LOWER_2) {
        heroPos = HERO_POSITION_JUMP_1;
      }
    } else if (state == "RUN") {
      if (heroPos >= HERO_POSITION_JUMP_1 && heroPos <= HERO_POSITION_JUMP_8) {
        heroPos = HERO_POSITION_RUN_LOWER_1;
      }
    }

    if (drawHero(heroPos)) {
      playing = false;
    } else {
      if (heroPos == HERO_POSITION_RUN_LOWER_2 || heroPos == HERO_POSITION_JUMP_8) {
        heroPos = HERO_POSITION_RUN_LOWER_1;
      } else if ((heroPos >= HERO_POSITION_JUMP_3 && heroPos <= HERO_POSITION_JUMP_5) &&
                terrainLower[HERO_HORIZONTAL_POSITION] != SPRITE_TERRAIN_EMPTY) {
        heroPos = HERO_POSITION_RUN_UPPER_1;
      } else if (heroPos >= HERO_POSITION_RUN_UPPER_1 &&
                terrainLower[HERO_HORIZONTAL_POSITION] == SPRITE_TERRAIN_EMPTY) {
        heroPos = HERO_POSITION_JUMP_5;
      } else if (heroPos == HERO_POSITION_RUN_UPPER_2) {
        heroPos = HERO_POSITION_RUN_UPPER_1;
      } else {
        ++heroPos;
      }

      ++distance;
      digitalWrite(PIN_AUTOPLAY, terrainLower[HERO_HORIZONTAL_POSITION + 2] == SPRITE_TERRAIN_EMPTY ? HIGH : LOW);
    }

    delay(50);
  }


};

SideScrollerGame game;

void setup() {
  pinMode(PIN_CONTRAST, OUTPUT);
  digitalWrite(PIN_CONTRAST, LOW);

  pinMode(PIN_AUTOPLAY, OUTPUT);
  digitalWrite(PIN_AUTOPLAY, HIGH);

  lcd.begin(16, 2);
  game.initializeGraphics();

  Serial.begin(9600);
}


void loop() {
    game.update();
}
