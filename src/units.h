#ifndef UNITS_H
#define UNITS_H

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

const char KWP_units_RPM[]                  = "/min";
const char KWP_units_Percentage[]           = "%";
const char KWP_units_Angle[]                = "deg";
const char KWP_units_Temperature[]          = "degC";
const char KWP_units_Voltage[]              = "V";
const char KWP_units_Voltage_m[]            = "mV";
const char KWP_units_Speed[]                = "km/h";
const char KWP_units_Resistance[]           = "Ohm";
const char KWP_units_Resistance_k[]         = "kOhm";
const char KWP_units_Resistance_m[]         = "mOhm";
const char KWP_units_Distance[]             = "m";
const char KWP_units_Distance_k[]           = "km";
const char KWP_units_Distance_m[]           = "mm";
const char KWP_units_Distance_c[]           = "cm";
const char KWP_units_Pressure[]             = "bar";
const char KWP_units_Pressure_m[]           = "mbar";
const char KWP_units_Time[]                 = "s";
const char KWP_units_Time_m[]               = "ms";
const char KWP_units_Time_h[]               = "h";
const char KWP_units_Time_mo[]              = "months";
const char KWP_units_Volume[]               = "l";
const char KWP_units_Volume_m[]             = "ml";
const char KWP_units_Current[]              = "A";
const char KWP_units_Current_m[]            = "mA";
const char KWP_units_Capacity[]             = "Ah";
const char KWP_units_Power[]                = "W";
const char KWP_units_Power_k[]              = "kW";
const char KWP_units_Mass_Flow[]            = "g/s";
const char KWP_units_Mass_Flow_m[]          = "mg/s";
const char KWP_units_Mass_Flow_km[]         = "mg/km";
const char KWP_units_Correction[]           = "KW";
const char KWP_units_Segment_Correction[]   = "degKW";
const char KWP_units_Consumption_h[]        = "l/h";
const char KWP_units_Consumption_100km[]    = "l/100km";
const char KWP_units_Consumption_1000km[]   = "l/1000km";
const char KWP_units_Mass_Per_Stroke_m[]    = "mg/h";
const char KWP_units_Mass_Per_Stroke_k[]    = "kg/h";
const char KWP_units_Torque[]               = "Nm";
const char KWP_units_Misfires[]             = "/s";
const char KWP_units_Turn_Rate[]            = "deg/s";
const char KWP_units_Acceleration[]         = "m/s^2";
const char KWP_units_Mass[]                 = "g";
const char KWP_units_Mass_k[]               = "kg";
const char KWP_units_Impulses[]             = "/km";
const char KWP_units_Fuel_Level_Factor[]    = "l/mm";
const char KWP_units_Attenuation[]          = "dB";
const char KWP_units_Parts_Per_Million[]    = "ppm";

const char KWP_units_Warm[] = "Warm";
const char KWP_units_Cold[] = "Cold";

const char KWP_units_Ignition_ATDC[] = "degATDC";
const char KWP_units_Ignition_BTDC[] = "degBTDC";

const char KWP_units_Map1[] = "Map1";
const char KWP_units_Map2[] = "Map2";

const char KWP_units_Vss[] = "Vss";
const char KWP_units_Wm2[] = "W/m^2";
const char KWP_units_Wcm2[] = "W/cm^2";
const char KWP_units_Wh[] = "Wh";
const char KWP_units_Ws[] = "Ws";
const char KWP_units_ms[] = "m/s";
const char KWP_units_lkm[] = "l/km";
const char KWP_units_N[] = "N";
const char KWP_units_angdeg[] = "<deg";
const char KWP_units_degF[] = "degF";
const char KWP_units_Imp[] = "Imp";

#endif
