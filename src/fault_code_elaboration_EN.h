#ifndef FAULT_CODE_ELABORATION_H
#define FAULT_CODE_ELABORATION_H

const char KWP_ELAB_00[] = "-";
const char KWP_ELAB_01[] = "Signal Shorted to Plus";
const char KWP_ELAB_02[] = "Signal Shorted to Ground";
const char KWP_ELAB_03[] = "No Signal";
const char KWP_ELAB_04[] = "Mechanical Malfunction";
const char KWP_ELAB_05[] = "Input Open";
const char KWP_ELAB_06[] = "Signal too High";
const char KWP_ELAB_07[] = "Signal too Low";
const char KWP_ELAB_08[] = "Control Limit Surpassed";
const char KWP_ELAB_09[] = "Adaptation Limit Surpassed";
const char KWP_ELAB_0A[] = "Adaptation Limit Not Reached";
const char KWP_ELAB_0B[] = "Control Limit Not Reached";
const char KWP_ELAB_0C[] = "Adaptation Limit (Mul) Exceeded";
const char KWP_ELAB_0D[] = "Adaptation Limit (Mul) Not Reached";
const char KWP_ELAB_0E[] = "Adaptation Limit (Add) Exceeded";
const char KWP_ELAB_0F[] = "Adaptation Limit (Add) Not Reached";
const char KWP_ELAB_10[] = "Signal Outside Specifications";
const char KWP_ELAB_11[] = "Control Difference";
const char KWP_ELAB_12[] = "Upper Limit";
const char KWP_ELAB_13[] = "Lower Limit";
const char KWP_ELAB_14[] = "Malfunction in Basic Setting";
const char KWP_ELAB_15[] = "Front Pressure Build-up Time too Long";
const char KWP_ELAB_16[] = "Front Pressure Reducing Time too Long";
const char KWP_ELAB_17[] = "Rear Pressure Build-up Time too Long";
const char KWP_ELAB_18[] = "Rear Pressure Reducing Time too Long";
const char KWP_ELAB_19[] = "Unknown Switch Condition";
const char KWP_ELAB_1A[] = "Output Open";
const char KWP_ELAB_1B[] = "Implausible Signal";
const char KWP_ELAB_1C[] = "Short to Plus";
const char KWP_ELAB_1D[] = "Short to Ground";
const char KWP_ELAB_1E[] = "Open or Short to Plus";
const char KWP_ELAB_1F[] = "Open or Short to Ground";
const char KWP_ELAB_20[] = "Resistance Too High";
const char KWP_ELAB_21[] = "Resistance Too Low";
const char KWP_ELAB_22[] = "No Elaboration Available";
const char KWP_ELAB_23[] = "-";
const char KWP_ELAB_24[] = "Open Circuit";
const char KWP_ELAB_25[] = "Faulty";
const char KWP_ELAB_26[] = "Output won't Switch or Short to Plus";
const char KWP_ELAB_27[] = "Output won't Switch or Short to Ground";
const char KWP_ELAB_28[] = "Short to Another Output";
const char KWP_ELAB_29[] = "Blocked or No Voltage";
const char KWP_ELAB_2A[] = "Speed Deviation too High";
const char KWP_ELAB_2B[] = "Closed";
const char KWP_ELAB_2C[] = "Short Circuit";
const char KWP_ELAB_2D[] = "Connector";
const char KWP_ELAB_2E[] = "Leaking";
const char KWP_ELAB_2F[] = "No Communications or Incorrectly Connected";
const char KWP_ELAB_30[] = "Supply voltage";
const char KWP_ELAB_31[] = "No Communications";
const char KWP_ELAB_32[] = "Setting (Early) Not Reached";
const char KWP_ELAB_33[] = "Setting (Late) Not Reached";
const char KWP_ELAB_34[] = "Supply Voltage Too High";
const char KWP_ELAB_35[] = "Supply Voltage Too Low";
const char KWP_ELAB_36[] = "Incorrectly Equipped";
const char KWP_ELAB_37[] = "Adaptation Not Successful";
const char KWP_ELAB_38[] = "In Limp-Home Mode";
const char KWP_ELAB_39[] = "Electric Circuit Failure";
const char KWP_ELAB_3A[] = "Can't Lock";
const char KWP_ELAB_3B[] = "Can't Unlock";
const char KWP_ELAB_3C[] = "Won't Safe";
const char KWP_ELAB_3D[] = "Won't De-Safe";
const char KWP_ELAB_3E[] = "No or Incorrect Adjustment";
const char KWP_ELAB_3F[] = "Temperature Shut-Down";
const char KWP_ELAB_40[] = "Not Currently Testable";
const char KWP_ELAB_41[] = "Unauthorized";
const char KWP_ELAB_42[] = "Not Matched";
const char KWP_ELAB_43[] = "Set-Point Not Reached";
const char KWP_ELAB_44[] = "Cylinder 1";
const char KWP_ELAB_45[] = "Cylinder 2";
const char KWP_ELAB_46[] = "Cylinder 3";
const char KWP_ELAB_47[] = "Cylinder 4";
const char KWP_ELAB_48[] = "Cylinder 5";
const char KWP_ELAB_49[] = "Cylinder 6";
const char KWP_ELAB_4A[] = "Cylinder 7";
const char KWP_ELAB_4B[] = "Cylinder 8";
const char KWP_ELAB_4C[] = "Terminal 30 missing";
const char KWP_ELAB_4D[] = "Internal Supply Voltage";
const char KWP_ELAB_4E[] = "Missing Messages";
const char KWP_ELAB_4F[] = "Please Check Fault Codes";
const char KWP_ELAB_50[] = "Single-Wire Operation";
const char KWP_ELAB_51[] = "Open";
const char KWP_ELAB_52[] = "Activated";

const char* const fault_elaboration_table[] = {
	KWP_ELAB_00, KWP_ELAB_01, KWP_ELAB_02, KWP_ELAB_03, KWP_ELAB_04, KWP_ELAB_05, 
	KWP_ELAB_06, KWP_ELAB_07, KWP_ELAB_08, KWP_ELAB_09, KWP_ELAB_0A, KWP_ELAB_0B, 
	KWP_ELAB_0C, KWP_ELAB_0D, KWP_ELAB_0E, KWP_ELAB_0F, KWP_ELAB_10, KWP_ELAB_11, 
	KWP_ELAB_12, KWP_ELAB_13, KWP_ELAB_14, KWP_ELAB_15, KWP_ELAB_16, KWP_ELAB_17, 
	KWP_ELAB_18, KWP_ELAB_19, KWP_ELAB_1A, KWP_ELAB_1B, KWP_ELAB_1C, KWP_ELAB_1D, 
	KWP_ELAB_1E, KWP_ELAB_1F, KWP_ELAB_20, KWP_ELAB_21, KWP_ELAB_22, KWP_ELAB_23, 
	KWP_ELAB_24, KWP_ELAB_25, KWP_ELAB_26, KWP_ELAB_27, KWP_ELAB_28, KWP_ELAB_29, 
	KWP_ELAB_2A, KWP_ELAB_2B, KWP_ELAB_2C, KWP_ELAB_2D, KWP_ELAB_2E, KWP_ELAB_2F, 
	KWP_ELAB_30, KWP_ELAB_31, KWP_ELAB_32, KWP_ELAB_33, KWP_ELAB_34, KWP_ELAB_35, 
	KWP_ELAB_36, KWP_ELAB_37, KWP_ELAB_38, KWP_ELAB_39, KWP_ELAB_3A, KWP_ELAB_3B, 
	KWP_ELAB_3C, KWP_ELAB_3D, KWP_ELAB_3E, KWP_ELAB_3F, KWP_ELAB_40, KWP_ELAB_41, 
	KWP_ELAB_42, KWP_ELAB_43, KWP_ELAB_44, KWP_ELAB_45, KWP_ELAB_46, KWP_ELAB_47, 
	KWP_ELAB_48, KWP_ELAB_49, KWP_ELAB_4A, KWP_ELAB_4B, KWP_ELAB_4C, KWP_ELAB_4D, 
	KWP_ELAB_4E, KWP_ELAB_4F, KWP_ELAB_50, KWP_ELAB_51, KWP_ELAB_52, 
};

#endif
