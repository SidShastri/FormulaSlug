//This is a C++ program that reads the accelerator pedal sensors (aka analog voltages), 
//computes the desired Torque Demand value, and prints it to console, handling all safety rules and requirements.
#include "mbed.h"
#include "test.hpp"
#include <algorithm>
#include <iostream>
using namespace std;

AnalogIn APPS0{PA_4};
AnalogIn APPS1{PA_3};

Timer implausibility;
//Track if implausibility is currently active, will be used later for ten percent check
bool sensor_implausibility_active = false;

int main()
{
   while(true) {
      test(5);
      double Pedal1=APPS0.read()*3.3;
      double Pedal2=APPS1.read()*3.3;
      
      //Variables for checking if within ten percent of each other
      double DiffOfPedals=abs(Pedal1-Pedal2);
      double tenPercentCheck=Pedal1*.10;
      
      //If condition checking if within 10 percent of each other, if they aren't provide a detailed message, also helps with debugging
      if(DiffOfPedals >= tenPercentCheck){
         cout<<"The difference between Pedal1 with "<< Pedal1 << 
         " volts and Pedal2 with " <<  Pedal2 <<  
         " volts is "<<  DiffOfPedals << 
         " and ten percent of  Pedal 1 is " <<  tenPercentCheck <<  
         " and the difference is greater than or equal to "<<  tenPercentCheck <<  
         " so the 2 pedals' difference is greater than or equal to 10 percent of each other, which violates the rules";
         
      }
      //If within ten percent of each other, then check for the maxes and mins within the elapsed time
      else{
         bool is_out_of_range = (Pedal1 > 2.25 || Pedal2 > 2.7 || Pedal1 < 0.25 || Pedal2 < 0.3);
         if(is_out_of_range){

            //If first time we see implausibility, begin timer 
            if(!sensor_implausibility_active){
               implausibility.start();
               sensor_implausibility_active=true;
               
            }

            //If timer measures implausibility to be active for over 100ms, return 0
             else if(implausibility.elapsed_time().count() / 1000 >= 100){
               return 0;
            }

         } else {

            //If no implausibility, reset timer, return active boolean to false
            if (sensor_implausibility_active) {
               implausibility.stop();
               implausibility.reset();
               sensor_implausibility_active = false;
            }
         }

         // Pedal Percentage if no implausibility marker
          if(!sensor_implausibility_active){
            printf("Analog in 1: %f\n",Pedal1);
            printf("Analog in 2: %f\n", Pedal2);
            double percentage1 = ((Pedal1-0.25)/(2.0));
            double percentage2 = ((Pedal2-0.3)/(2.4));
            double finalPercentage = ((percentage1 + percentage2)/2) * 100;
            printf("Final Percentage %f\n", finalPercentage);  

         }
      }
   }
   return 0;
}
