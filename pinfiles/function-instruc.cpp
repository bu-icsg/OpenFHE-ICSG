#include "pin.H"
#include <iostream>
#include <cstring>
#include <unordered_map>

const std::string SUBSTRING_TO_FIND = "EvalAdd";
// Data structure to store memory instruction counts for each function
struct FuncInfo{
	UINT64 totalInstr;
	UINT64 memoryInstr;
	UINT64 controlInstr;
	UINT64 arithmeticInstr;
};
std::unordered_map<std::string, FuncInfo> functionInstructions;

// This function is called for every function call instruction
VOID TraceFunction(const char* name, BOOL isArithmeticInstruction, BOOL isControlInstruction, BOOL isMemoryInstruction) {
    std::string functionName(name);
    // Check if the function name contains the desired substring
    if (functionName.find(SUBSTRING_TO_FIND) != std::string::npos) {
	functionInstructions[functionName].totalInstr++;
        // Increment memory instruction count for the current function
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
	}else if (INS_Category(ins) == XED_CATEGORY_ARITHMETIC) {
	    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)TraceFunction,
                           IARG_ADDRINT, RTN_Name(rtn).c_str(),
                           IARG_BOOL, TRUE,
                           IARG_BOOL, FALSE,
                           IARG_BOOL, FALSE, IARG_END);
	}else{
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
	<< std::endl;
    }
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
    INS_AddInstrumentFunction(Instruction, nullptr);

    // Register function to be called when application starts
    PIN_AddApplicationStartFunction(ApplicationStart, nullptr);
    PIN_AddFiniFunction(ApplicationFini, nullptr);
    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}

