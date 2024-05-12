#include <iostream>

int notargetfunc(){
	std::cout << "Not target func" << std::endl;
	int i = 0;
	i+=1;
	i+=1;
}
// Function to be tracked
void targetFunc() {
    std::cout << "Inside targetFunc "<< std::endl;
}

// Main function
int main() {
    std::cout << "Starting main" << std::endl;
    targetFunc();
    std::cout << "Ending main " <<typeid(&targetFunc).name()<< std::endl;
    return 0;
}
