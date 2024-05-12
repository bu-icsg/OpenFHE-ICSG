#include <iostream>
#include "pin.H"

// Name of the function to track
#define TARGET_FUNCTION_NAME "_Z10targetFuncv" // Modify this to match your target function name

// Global counter for instructions
UINT64 insCount = 0;

// Address range of the target function
ADDRINT funcStartAddr = 0;
ADDRINT funcEndAddr = 0;

// Pin tool function to be executed for each instruction
VOID Instruction(INS ins, VOID *v) {
    // Check if the instruction falls within the address range of the target function
    ADDRINT insAddr = INS_Address(ins);
    //std::cout << "At instruction : " << insAddr << std::endl;
    if (insAddr >= funcStartAddr && insAddr < funcEndAddr) {
        // Increment instruction count
        insCount++;
    }
}

// Pin tool function to be called when the application exits
VOID Fini(INT32 code, VOID *v) {
    std::cout << "Start Add: " << funcStartAddr << " and End Add: " << funcEndAddr << std::endl;
    // Print out the total instruction count
    std::cout << "Total Instructions in " << TARGET_FUNCTION_NAME << ": " << insCount << std::endl;
}

// Callback to get the address range of the target function
VOID ImageLoad(IMG img, VOID *v) {
    // Iterate through all the functions in the image
    std::cout << "In Image " << std::endl;
    RTN testRtn = RTN_FindByName(img, TARGET_FUNCTION_NAME);
    if(RTN_Valid(testRtn)){
	std::cout << "Found " << std::endl;
	funcStartAddr = RTN_Address(testRtn);
	funcEndAddr = funcStartAddr+RTN_Size(testRtn);
    }    
    /**
    for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym)) {
        // Check if the current symbol matches the target function name
	std::cout << "Here 1 " << std::endl;
        if (SYM_Name(sym) == TARGET_FUNCTION_NAME) {
	    std::cout << "Here 2 " << std::endl;
            funcStartAddr = SYM_Value(sym);
            std::cout << "Here 3 " << std::endl;
	    RTN rtn = RTN_FindByAddress(funcStartAddr);
            std::cout << "Here 4 " << std::endl;
	    funcEndAddr = funcStartAddr + RTN_Size(rtn);
	    std::cout << "Here 5 " << std::endl;
            break;
	}
    }**/
}

INT32 Usage() {
    std::cerr << "This tool tracks the number of instructions executed within a specific function." << std::endl;
    std::cerr << std::endl << KNOB_BASE::StringKnobSummary() << std::endl;
    return -1;
}

// Main function
int main(int argc, char *argv[]) {
    // Initialize Pin
    PIN_InitSymbols();
    if (PIN_Init(argc, argv)) {
        return Usage();
    }
    // Register ImageLoad to be called when an image is loaded
    IMG_AddInstrumentFunction(ImageLoad, 0);
    // Register Instruction to be called for every executed instruction
    INS_AddInstrumentFunction(Instruction, 0);
    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    // Start the program, never returns
    PIN_StartProgram();
    return 0;
}
