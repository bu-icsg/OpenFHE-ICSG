#include "pin.H"
#include <iostream>
#include <cstring>
#include <unordered_map>

const std::string SUBSTRING_TO_FIND = "_ZNK8lbcrypto17CryptoContextImplINS_12DCRTPolyImplIN9bigintdyn9mubintvecINS2_5ubintImEEEEEEE10EvalRotateESt10shared_ptrIKNS_14CiphertextImplIS7_EEEi";
std::string myFunc;
// Data structure to store memory instruction counts for each function
struct FuncInfo{
	UINT64 totalInstr;
	UINT64 memoryInstr;
	UINT64 controlInstr;
	UINT64 arithmeticInstr;
};
std::unordered_map<std::string, FuncInfo> functionInstructions;
//std::unordered_map<std::string, std::string> extraInstr;
//std::stack<std::string> funcStack;
bool inSubCall = false;
int totalInstructions = 0;
// Function to check if an instruction is an arithmetic instruction based on its XED_ICLASS code
bool IsALInstruction(INS ins) {
    switch (INS_Category(ins)) {
        case XED_CATEGORY_BINARY:// Most binary math ops
	case XED_CATEGORY_BITBYTE:// for BTS instruc
	case XED_CATEGORY_LOGICAL:
	case XED_CATEGORY_LOGICAL_FP:
	case XED_CATEGORY_SHIFT:
            return true;
        default:
            return false;
    }
}

// Function to check is instruction is SIMD SSE arithmetic or CMP instructions
bool IsSSEInstruction(INS ins){
	switch (INS_Opcode(ins)){
	    case XED_ICLASS_ADDSD:
	    case XED_ICLASS_ADDPD:
	    case XED_ICLASS_SUBSD:
	    case XED_ICLASS_SUBPD:
	    case XED_ICLASS_PSUBB:
	    case XED_ICLASS_CMPPD:
	    case XED_ICLASS_CMPSD:
	    case XED_ICLASS_MULSD:
	    case XED_ICLASS_MULPD:
	    case XED_ICLASS_DIVSD:
	    case XED_ICLASS_DIVPD:
		return true;
    	    default:
		return false;	    
	}
}

// This function is called for every function call instruction
VOID TraceFunction(const char* name, BOOL isArithmeticInstruction, BOOL isControlInstruction, BOOL isMemoryInstruction) {
    std::string functionName(name);
    totalInstructions++;
    // Check if the function name contains the desired substring
    if (functionName.find(SUBSTRING_TO_FIND) != std::string::npos) {
	functionInstructions[functionName].totalInstr++;
	if (isMemoryInstruction) {
	    functionInstructions[functionName].memoryInstr++;
        }
	if (isControlInstruction){
	    functionInstructions[functionName].controlInstr++;
	}
	if (isArithmeticInstruction) {
	    functionInstructions[functionName].arithmeticInstr++;
	}
    }
    if (inSubCall) {
    	functionInstructions[myFunc].totalInstr++;
        if (isMemoryInstruction) {
            functionInstructions[myFunc].memoryInstr++;
        }
        if (isControlInstruction){
            functionInstructions[myFunc].controlInstr++;
        }
        if (isArithmeticInstruction) {
            functionInstructions[myFunc].arithmeticInstr++;
        }
    }
}

VOID BeforeSetFunction(){
	inSubCall = true;
	//std::cout << "Settrue\n" << std::endl;
}

VOID AfterSetFunction(){
        inSubCall = false;
	//std::cout << "Setfalse\n" << std::endl;
}

VOID Routine(RTN rtn, VOID* v){
    if(RTN_Valid(rtn)){
	std::string funcName(RTN_Name(rtn).c_str());
	//std::cout << "in " << funcName << "\n" << std::endl;
	if(funcName.find(SUBSTRING_TO_FIND) != std::string::npos){
		myFunc = funcName;
		RTN_Open(rtn);
		RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)BeforeSetFunction, IARG_END);
		RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)AfterSetFunction, IARG_END);
		RTN_Close(rtn);
	}
    }
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID* v) {
    RTN rtn = INS_Rtn(ins);
    if (RTN_Valid(rtn)) {	 
        if (INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins)) {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)TraceFunction,
                           IARG_ADDRINT, RTN_Name(rtn).c_str(),
			   IARG_BOOL, FALSE,
			   IARG_BOOL, FALSE,
                           IARG_BOOL, TRUE, IARG_END);
        }else if (INS_IsControlFlow(ins)) {
	    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)TraceFunction,
                           IARG_ADDRINT, RTN_Name(rtn).c_str(),
			   IARG_BOOL, FALSE,
			   IARG_BOOL, TRUE,
                           IARG_BOOL, FALSE, IARG_END);
	}else if (IsALInstruction(ins) || IsSSEInstruction(ins)) {
	    //std::cout << CATEGORY_StringShort(INS_Category(ins)) << " contains " << INS_Mnemonic(ins) << "\n" << std::endl;
	    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)TraceFunction,
                           IARG_ADDRINT, RTN_Name(rtn).c_str(),
                           IARG_BOOL, TRUE,
                           IARG_BOOL, FALSE,
                           IARG_BOOL, FALSE, IARG_END);
	}else{	// mov's that did not mem read/write, 
	    //extraInstr[INS_Mnemonic(ins)] = CATEGORY_StringShort(INS_Category(ins));
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)TraceFunction,
                       IARG_ADDRINT, RTN_Name(rtn).c_str(),
		       IARG_BOOL, FALSE,
		       IARG_BOOL, FALSE,
                       IARG_BOOL, FALSE, IARG_END);
	}
    }
}

// Pin calls this function when the application exits
VOID ApplicationFini(INT32 code, VOID* v) {
    for (const auto& [entry,info] : functionInstructions) {
        std::cout << "Function: " << entry << " | Total Instructions: " << info.totalInstr 
	<< " | Memory Instructions: " << info.memoryInstr 
	<< " | Control Instructions: " << info.controlInstr
	<< " | Arithmetic Instructions: " << info.arithmeticInstr
	<< " | Out if a total ISA count: " << totalInstructions
	<< std::endl;
    }
    /*for (const auto& [entry,info] : extraInstr) {
        std::cout << entry << " - " << info << "\n" << std::endl;
    }*/
}

// Pin calls this function when the application starts
VOID ApplicationStart(VOID* v) {
    std::cout << "Starting program execution..." << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize Pin
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    
    // Register function to be called to instrument memory instructions
    //INS_AddInstrumentFunction(InstrumentMemoryInstruction, nullptr);

    // Register function to be called to instrument instructions
    RTN_AddInstrumentFunction(Routine, nullptr);
    INS_AddInstrumentFunction(Instruction, nullptr);
    // Register function to be called when application starts and ends
    PIN_AddApplicationStartFunction(ApplicationStart, nullptr);
    PIN_AddFiniFunction(ApplicationFini, nullptr);
    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}

