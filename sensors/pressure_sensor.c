// sensor BMP280 - temperatura e pressão

#include "pressure_sensor.h"
#include <math.h>
#include "pico/stdlib.h"
#include "sensors/i2c_interface.h"

#define BMP280_PORT i2c0
#define BMP280_BAUDRATE 400 * 1000 // 400 kHz
#define BMP280_SERIAL_DATA 0
#define BMP280_SERIAL_CLOCK 1
#define BMP280_ADDRESS 0x76

#define CALIBRATE_PARAMETERS_SIZE 24
#define COMMAND_PARAMETERS 0x88
#define COMMAND_RESET_1 0xE0
#define COMMAND_RESET_2 0xB6
#define COMMAND_CONFIG_1 0xF5
#define COMMAND_CONFIG_2 0x94
#define COMMAND_MEASUREMENT_1 0xF4
#define COMMAND_MEASUREMENT_2 0x2F
#define COMMAND_GET_DATA 0xF7

#define SEA_LEVEL_PRESSURE 101325.0 // Pressão ao nível do mar em Pa

// pt - temperature parameter, pp - pressure parameter
typedef struct {
    unsigned short pt1;
    short pt2;
    short pt3;
    unsigned short pp1;
    short pp2;
    short pp3;
    short pp4;
    short pp5;
    short pp6;
    short pp7;
    short pp8;
    short pp9;
} internal_parameters;

static internal_parameters _parameters;

static void init_calibrate_parameters() {
    unsigned char parameters_command[] = {COMMAND_PARAMETERS};
    i2c_interface_write(BMP280_PORT, BMP280_ADDRESS, parameters_command, 1);
    unsigned char result[CALIBRATE_PARAMETERS_SIZE];
    i2c_interface_read(BMP280_PORT, BMP280_ADDRESS, result, CALIBRATE_PARAMETERS_SIZE);
    _parameters.pt1 = (unsigned short)(result[1] << 8) | result[0];
    _parameters.pt2 = (short)(result[3] << 8) | result[2];
    _parameters.pt3 = (short)(result[5] << 8) | result[4];
    _parameters.pp1 = (unsigned short)(result[7] << 8) | result[6];
    _parameters.pp2 = (short)(result[9] << 8) | result[8];
    _parameters.pp3 = (short)(result[11] << 8) | result[10];
    _parameters.pp4 = (short)(result[13] << 8) | result[12];
    _parameters.pp5 = (short)(result[15] << 8) | result[14];
    _parameters.pp6 = (short)(result[17] << 8) | result[16];
    _parameters.pp7 = (short)(result[19] << 8) | result[18];
    _parameters.pp8 = (short)(result[21] << 8) | result[20];
    _parameters.pp9 = (short)(result[23] << 8) | result[22];
}

static void init() {
    unsigned char config_command[] = {COMMAND_CONFIG_1, COMMAND_CONFIG_2};
    i2c_interface_write(BMP280_PORT, BMP280_ADDRESS, config_command, 2);
    unsigned char measurement_control_command[] = {COMMAND_MEASUREMENT_1, COMMAND_MEASUREMENT_2};
    i2c_interface_write(BMP280_PORT, BMP280_ADDRESS, measurement_control_command, 2);
}

void pressure_sensor_init() {
    i2c_interface_init(BMP280_PORT, BMP280_BAUDRATE, BMP280_SERIAL_CLOCK, BMP280_SERIAL_DATA);
    init();
    init_calibrate_parameters();
}

static unsigned char* raw_read() {
    unsigned char get_data_command[] = {COMMAND_GET_DATA};
    i2c_interface_write(BMP280_PORT, BMP280_ADDRESS, get_data_command, 1);
    static unsigned char raw_data[6];
    i2c_interface_read(BMP280_PORT, BMP280_ADDRESS, raw_data, 6);
    return raw_data;
}

static long calculate_fine_temperature(long temperature) {
    long bias1 =((((temperature >> 3) - ((long)_parameters.pt1 << 1))) * ((long)_parameters.pt2)) >> 11;
    long bias2 = (((((temperature >> 4) - ((long)_parameters.pt1)) * ((temperature >> 4) - ((long)_parameters.pt1))) >> 12) * ((long)_parameters.pt3)) >> 14;
    return bias1 + bias2;
}

float pressure_sensor_temperature_read() {
    unsigned char* raw_data = raw_read();
    long raw_temperature = (raw_data[3] << 12) | (raw_data[4] << 4) | (raw_data[5] >> 4);
    long fine_temperature = calculate_fine_temperature(raw_temperature);
    return ((fine_temperature * 5 + 128) >> 8)/100.0;
}

long pressure_sensor_pressure_read() {
    unsigned char* raw_data = raw_read();
    long raw_temperature = (raw_data[3] << 12) | (raw_data[4] << 4) | (raw_data[5] >> 4);
    long raw_pressure = (raw_data[0] << 12) | (raw_data[1] << 4) | (raw_data[2] >> 4);
    long fine_temperature = calculate_fine_temperature(raw_temperature);
    long var1, var2;
    var1 = (((long)fine_temperature) >> 1) - (long)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((long)_parameters.pp6);
    var2 += ((var1 * ((long)_parameters.pp5)) << 1);
    var2 = (var2 >> 2) + (((long)_parameters.pp4) << 16);
    var1 = (((_parameters.pp3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((long)_parameters.pp2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((long)_parameters.pp1)) >> 15);
    if (var1 == 0) return 0;
    unsigned long converted = (((unsigned long)(((long)1048576) - raw_pressure) - (var2 >> 12))) * 3125;
    converted = (converted < 0x80000000) ? (converted << 1) / ((unsigned long)var1) : (converted / (unsigned long)var1) * 2;
    var1 = (((long)_parameters.pp9) * ((long)(((converted >> 3) * (converted >> 3)) >> 13))) >> 12;
    var2 = (((long)(converted >> 2)) * ((long)_parameters.pp8)) >> 13;
    return ((long)converted + ((var1 + var2 + _parameters.pp7) >> 4));
}

float pressure_sensor_altitude_read() {
    return 44330.0 * (1.0 - pow(pressure_sensor_pressure_read() / SEA_LEVEL_PRESSURE, 0.1903));
}