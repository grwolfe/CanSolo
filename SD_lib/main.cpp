 #include "mbed.h"
 #include "string"
 #include "SDHCFileSystem.h"
 
 SDFileSystem sd(p5, p6, p7, p8, "sd"); // mosi, miso, sclk, cs
   
 int main() {
       
       FILE *fp = fopen("/sd/fdata.csv", "w");
       fprintf(fp, "1234, 5678, 91011, 121314, 151617, 181920\n\r");
       fclose(fp);
 }