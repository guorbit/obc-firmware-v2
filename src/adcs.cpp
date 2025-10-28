#include <adcs.h>
float readADCS(bool adcsreading, float reading){
    if (adcsreading==true){return reading;} //if a adcs reading is received, then the reading is return from the function.
    else {cout<<"Reading ADCS is not yet implemented"<<endl;} // the case when adcs reading is not received.
}