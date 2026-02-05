#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <Adafruit_INA228.h>

// --- I2C addresses (typical) ---
static constexpr uint8_t OLED_ADDR = 0x3C;     // sometimes 0x3D
static constexpr uint8_t INA_ADDR    = 0x40;

// --- SH1106 128x64 over I2C ---
Adafruit_SH1106G oled(128, 64, &Wire, -1);

// --- INA228 ---
Adafruit_INA228 ina228;

// Pick a display refresh that is <= INA228 "new-data" cadence
static constexpr uint32_t DISPLAY_UPDATE_MS = 500;

static bool oled_begin_try(uint8_t addr) {
    return oled.begin(addr, true); // addr, reset=true
}

void setup() {
    Serial.begin(115200);
    delay(100);

    Wire.begin();
    // Optional: faster I2C transfers (doesn't change measurement accuracy)
    // Wire.setClock(400000);

    // OLED init (try 0x3C then 0x3D)
    if (!(oled_begin_try(0x3C) || oled_begin_try(0x3D))) {
        Serial.println("SH1106 init failed");
        while (true) delay(1000);
    }

    oled.clearDisplay();
    oled.setTextColor(SH110X_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.println("OLED OK");
    oled.display();

    // INA228 init
    if (!ina228.begin(INA_ADDR, &Wire)) {
        Serial.println("INA228 init failed");
        oled.clearDisplay();
        oled.setCursor(0, 0);
        oled.println("INA228 init failed");
        oled.display();
        while (true) delay(1000);
    }

    // IMPORTANT: set the *real* shunt value you are using.
    // Adafruit INA228 breakout uses 0.015 ohm shunt.
    ina228.setShunt(0.015f, 10.0f);

    // "Accurate-looking": internal averaging + longer conversion times = less noise, slower updates
    ina228.setAveragingCount(INA228_COUNT_256);
    ina228.setVoltageConversionTime(INA228_TIME_1052_us);
    ina228.setCurrentConversionTime(INA228_TIME_1052_us);
    ina228.setTemperatureConversionTime(INA2XX_TIME_150_us);
}

void loop() {
    static uint32_t last = 0;
    uint32_t now = millis();
    if (now - last < DISPLAY_UPDATE_MS)
        return;
    last = now;

    // Read values
    const float i_mA    = ina228.getCurrent_mA();
    const float v_bus = ina228.getBusVoltage_V();
    const float t_die = ina228.readDieTemp();

    // OLED output
    oled.clearDisplay();
    oled.setCursor(0, 0);

    oled.setTextSize(1);
    oled.print("INA228  Tdie ");
    oled.print(t_die, 2);
    oled.print(" C\n\n");

    oled.setTextSize(2);
    oled.print("I ");
    oled.print(i_mA, 1);
    oled.print("mA\n");

    oled.setTextSize(1);
    oled.print("\n");
    oled.setTextSize(2);

    oled.print("U ");
    oled.print(v_bus, 4);
    oled.print(" V\n");

    oled.display();

    // Optional serial debug
    Serial.printf("I=%.3f mA  Vbus=%.4f V  Tdie=%.2f C\n", i_mA, v_bus, t_die);
}
