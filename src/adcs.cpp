#include <adcs.h>
#include <string>
bool adcsreading=true; // define whether the adcs reading is received or not
// if pin number for the adcs system= High, bool adcsreadfing= true.
std::string readADCS(bool adcsreading, std::string reading){
    if (adcsreading==true){return reading;} //if a adcs reading is received, then the reading is return from the function.
}