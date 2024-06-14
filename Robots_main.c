#include <libpynq.h>
#include <stepper.h>
#include <iic.h>
#include "vl53l0x.h"
#include <edge_sensors.h>
#include "tcs3472.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

//FUNCTIONS FOR COMMUNCATION AND COORDINATES
void sleep_msec(int milliseconds)
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
//FUNCTIONS FOR COMMUNCATION AND COORDINATES

#define MESSAGE_BUFFER_SIZE 120  // Increase size if needed

void transmit_message_C(int x, int *y, char *direction,uint8_t base_msg) {
    char msg[MESSAGE_BUFFER_SIZE];
	
    // Format the message to include X, Y coordinates, and direction
    snprintf(msg, MESSAGE_BUFFER_SIZE, "%d,%d,%c,%s",*x, *y, *direction,base_msg);
    
    uint32_t length = strlen(msg);
    uint8_t bt;
    
    // Sending message length
    for (int i = 0; i < 4; i++) {
        bt = ((uint8_t*)&length)[i];
        uart_send(UART0, bt);
        sleep_msec(2);
    }
    sleep_msec(5);
    
    // Sending the message itself
    for (int i = 0; msg[i] != '\0'; i++) {
        uart_send(UART0, msg[i]);
        sleep_msec(2);
    }
}
void transmit_message(uint8_t* msg)
{
    uint32_t length = strlen((char*)msg);
    uint8_t bt;
    // Sending message length
    for (int i = 0; i < 4; i++) {
        bt = ((uint8_t*)&length)[i];
        uart_send(UART0, bt);
        sleep_msec(2);
    }
    sleep_msec(5);
    // Sending the message itself
    for (int i = 0; msg[i] != '\0'; i++) {
        uart_send(UART0, msg[i]);
        sleep_msec(2);
    }
}

//Distance sensor Adresses
uint8_t addrA = 0x69;
uint8_t addrB = 0x70;
uint8_t addrC = 0x68;

int getDistanceMountainSensor(){
int i;
	uint8_t addrA = 0x69;
	//Create a sensor struct
	vl53x sensorA;
	//Initialize the sensor
	i = tofInit(&sensorA, IIC0, addrA, 0); // set default range mode (up to 800mm)
	if (i != 0)
	{
		return -1; // problem - quit
	}
	uint32_t iDistance;
		iDistance = tofReadDistance(&sensorA);
    iDistance=iDistance/10;
  return iDistance;
}

int getBigBlockSensor(){
int i;
	//Create a sensor struct
	vl53x sensorB;
	//Initialize the sensor
	i = tofInit(&sensorB, IIC0, addrB, 0); // set default range mode (up to 800mm)
	if (i != 0)
	{
		return -1; // problem - quit
	}
	uint32_t iDistance;
		iDistance = tofReadDistance(&sensorB);
   		iDistance=iDistance/10;
  return iDistance;
}

int getSmallBlockSensor(){
int i;
	//Create a sensor struct
	vl53x sensorC;
	//Initialize the sensor
	i = tofInit(&sensorC, IIC1, addrC, 0); // set default range mode (up to 800mm)
	if (i != 0)
	{
		return -1; // problem - quit
	}
	uint32_t iDistance;
		iDistance = tofReadDistance(&sensorC);
    	iDistance=iDistance/10;
  return iDistance;
}

 int main(void) {
  pynq_init();
  gpio_init();

  
  //Commmunication switchbox
  //We reset Sensor A
	gpio_set_direction(IO_AR6, GPIO_DIR_OUTPUT);
	gpio_set_level(IO_AR6, GPIO_LEVEL_HIGH);
	sleep_msec(200);
	//We reset Sensor B
	gpio_set_direction(IO_AR7, GPIO_DIR_OUTPUT);
	gpio_set_level(IO_AR7, GPIO_LEVEL_HIGH);
	sleep_msec(200);

	//We reset Sensor C
	gpio_set_direction(IO_AR8, GPIO_DIR_OUTPUT);
	gpio_set_level(IO_AR8, GPIO_LEVEL_HIGH);
	sleep_msec(200);

	//We reset COLOR SENSOR
	gpio_set_direction(IO_AR9, GPIO_DIR_OUTPUT);
	gpio_set_level(IO_AR9, GPIO_LEVEL_HIGH);
	sleep_msec(200);


 	switchbox_set_pin(IO_AR0, SWB_UART0_RX);
    switchbox_set_pin(IO_AR1, SWB_UART0_TX);
	// Initialize UART 0
	uart_init(UART0);
	uart_reset_fifos(UART0);
	// Flush FIFOs of UART 0


    switchbox_set_pin(IO_AR_SCL, SWB_IIC0_SCL);
	switchbox_set_pin(IO_AR_SDA, SWB_IIC0_SDA);
	iic_init(IIC0);
	switchbox_set_pin(IO_AR13, SWB_IIC1_SCL);
	switchbox_set_pin(IO_AR12, SWB_IIC1_SDA);
	iic_init(IIC1);

 	uint8_t model, revision;
	int i = 0;
	//Setup Sensor A
	printf("Initialising Sensor A:\n");
 
  	gpio_set_level(IO_AR6, GPIO_LEVEL_LOW);
  sleep_msec(1000);

	//Change the Address of the VL53L0X
	 i = tofSetAddress(IIC0, 0x29, addrA);
	printf("---Address Change: ");
	if(i != 0)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");
	
	i = tofPing(IIC0, addrA);
	printf("---Sensor Ping: ");
	if(i != 0)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");

	//Create a sensor struct
	vl53x sensorA;
	//Initialize the sensor
	i = tofInit(&sensorA, IIC0, addrA, 0);
	
	if (i != 0)
	{
		printf("---Init: Fail\n");
		return 1;
	}

	tofGetModel(&sensorA, &model, &revision);
	printf("---Model ID - %d\n", model);
	printf("---Revision ID - %d\n", revision);
	printf("---Init: Succes\n");
	fflush(NULL);

// 	//printf("\n\nNow Power Sensor B!!\nPress \"Enter\" to continue...\n");
// 	//getchar();

	//Setup Sensor B
	printf("Initialising Sensor B:\n");

//MOSFET POWERING SENSOR B
 
  gpio_set_level(IO_AR7, GPIO_LEVEL_LOW);
  sleep_msec(1000);
	//Change the Address of the VL53L0X
	i = tofSetAddress(IIC0, 0x29, addrB);
	printf("---Address Change: ");
	if(i != 0)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");

	i = tofPing(IIC0, addrB);
	printf("---Sensor Ping: ");
	if(i != 0)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");

	//Create a sensor struct
	vl53x sensorB;

	//Initialize the sensor

	i = tofInit(&sensorB, IIC0, addrB, 0);
	if (i != 0)
	{
		printf("---Init: Fail\n");
		return 1;
	}

	tofGetModel(&sensorB, &model, &revision);
	printf("---Model ID - %d\n", model);
	printf("---Revision ID - %d\n", revision);
	printf("---Init: Succes\n");
	fflush(NULL); //Get some output even is distance readings hang
	printf("\n");

	
	printf("\n\nNow Power Sensor C on IIC1 !!\nPress \"Enter\" to continue...\n");
	//getchar();
	//We reset Sensor A
	
	gpio_set_level(IO_AR8, GPIO_LEVEL_LOW);
	sleep_msec(200);

	//Setup Sensor C
	printf("Initialising Sensor C on IIC1:\n");

	 i = tofSetAddress(IIC0, 0x29, addrC);
	printf("---Address Change: ");
	if(i != 0)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");

	//Change the Address of the VL53L0X
	i = tofPing(IIC0, addrC);
	printf("---Sensor Ping: ");
	if(i != 0)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");

	//Create a sensor struct
	vl53x sensorC;

	//Initialize the sensor
	i = tofInit(&sensorC, IIC0, addrC, 0);
	if (i != 0)
	{
		printf("---Init: Fail\n");
		return 1;
	}

	tofGetModel(&sensorC, &model, &revision);
	printf("---Model ID - %d\n", model);
	printf("---Revision ID - %d\n", revision);
	printf("---Init: Succes\n");
	fflush(NULL);
	

// 	 printf("\n\nNow Power Colour Sensor on IIC1 !!");
// 	// getchar();
gpio_set_level(IO_AR9, GPIO_LEVEL_LOW);
	sleep_msec(200);
	// /******** Simple connection test *******/
	uint8_t id;
	int j = tcs_ping(IIC1, &id);
	printf("---Detection: ");
	if( j != TCS3472_SUCCES)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");
	printf("-- ID: %#X\n", id);
	

	//Due to 50hz powergrind and thus 50hz flicker in indoor lighting.
	//Always use a 20ms increment in the integration time.
	//Period of 50hz is 20ms
	  int integration_time_ms = 100; 

	// /********** Preconfigure sensor ********/
	tcs3472 sensor = TCS3472_EMPTY;
	tcs_set_integration(&sensor, tcs3472_integration_from_ms(integration_time_ms));
	tcs_set_gain(&sensor, x4);

	//enable sensor -> loads preconfiguration
	i = tcs_init(IIC1, &sensor);
	printf("---Sensor Init: ");
	if(i != TCS3472_SUCCES)
	{
		printf("Fail\n");
		return 1;
	}
	printf("Succes\n");
	fflush(NULL);

	sleep_msec(integration_time_ms);

	tcsReading rgb;
	// printf("        \n        \n        \n"); //Buffer some space
	

 while(1){

	sleep_msec(integration_time_ms);
 	i = tcs_get_reading(&sensor, &rgb);


// // if (rgb.blue > 450 && rgb.green > 450 && rgb.green>450) {
// //           printf("White is dominant\n");
// // 			}
// //  	else if(rgb.blue <240  && rgb.red<240) {
// //                  printf("Black is dominant\n");
// //              }
// //              else if ((rgb.green > rgb.red && rgb.green > rgb.blue)&&(rgb.green>500)) {
// //                  printf("Green is dominant\n");
// //              } else if (rgb.blue > rgb.red && rgb.blue > rgb.red) {
// //                  printf("Blue is dominant\n");
// //  			}else if(rgb.red > rgb.green && rgb.red > rgb.blue) {
// //                  printf("Red is dominant\n");
// //              }
		printf("Red: %hu\n",rgb.red);
		printf("Green: %hu\n",rgb.green);
		printf("Blue: %hu\n",rgb.blue);
		printf("\n");
//  		sleep_msec(300);
// 	printf("\n");
// //  		//getDistanceMountainSensor();
// //  	// 	getBigBlockSensor();
// // 	// getSmallBlockSensor();
	}
 // Initialize the stepper driver.
stepper_init();
//Apply power to the stepper motors.
stepper_enable();
//Set speed for stepper motors
stepper_set_speed(20000,20000);
//Initialise Edge sensor
 edge_sensors_init();

//stepper_steps(625,-625); //left turn
// stepper_steps(-625,625); //right turn


// }
int turn = 0;
int check = 0;
int boundarycheck=0;

int *y = malloc(sizeof(int));
int *x = malloc(sizeof(int));
*x=0;
*y= 0;

//initialise direction N
char *direction = malloc(sizeof(char));
*direction = 'N';

void change_C(char *direction, int *y, int *x, int i)
{
	if(*direction == 'N') *y = *y+i;
	else if(*direction == 'S') *y = *y-i;
	else if(*direction == 'W') *x = *x+i;
	else if(*direction == 'E') *x = *x-i;
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
}


//use function on 90 degrees right turn

void turn_right(char *direction)
{
	if(*direction == 'N') {*direction = 'E';
	printf("Check\n");}
	
	else if(*direction == 'S') *direction = 'W';
	else if(*direction == 'W') *direction = 'N';
	else if(*direction == 'E') *direction = 'S';
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
}



void turn_left(char *direction)
{
	if(*direction == 'N') *direction = 'W';
	else if(*direction == 'S') *direction = 'E';
	else if(*direction == 'W') *direction = 'S';
	else if(*direction == 'E') *direction = 'N';
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
}

void U_turn(char *direction)
{
	if(*direction == 'N') *direction = 'S';
	else if(*direction == 'S') *direction = 'N';
	else if(*direction == 'W') *direction = 'E';
	else if(*direction == 'E') *direction = 'W';
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
}
//Values need to be changed
 void get_Small_Block_Colour(uint16_t red, uint16_t green, uint16_t blue)
 {
	rgb.red=red;
	rgb.green=green;
	rgb.blue=blue;
 	
	if (rgb.blue > 450 && rgb.green > 450 && rgb.green>450) {
          			   strcpy((char *)base_msg, "S-White");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Wgite is dominant\n");
			}
 	else if(rgb.blue <240  && rgb.red<240) {
					   strcpy((char *)base_msg, "S-Black");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Black is dominant\n");
             }
             else if ((rgb.green > rgb.red && rgb.green > rgb.blue)&&(rgb.green>500)) {
					   strcpy((char *)base_msg, "S-Green");
					   transmit_message_C(x, y, direction, base_msg);
                 	   printf("Green is dominant\n");
              } else if (rgb.blue > rgb.red && rgb.blue > rgb.red) {
					   strcpy((char *)base_msg, "S-Blue");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Blue is dominant\n");
  			}else if(rgb.red > rgb.green && rgb.red > rgb.blue) {
					   strcpy((char *)base_msg, "S-Red");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Red is dominant\n");
            }


 }

 void get_Big_Block_Colour(uint16_t red, uint16_t green, uint16_t blue)
 {
	rgb.red=red;
	rgb.green=green;
	rgb.blue=blue;
 	
	if (rgb.blue > 450 && rgb.green > 450 && rgb.green>450) {
          			   strcpy((char *)base_msg, "B-White");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Wgite is dominant\n");
			}
 	else if(rgb.blue <240  && rgb.red<240) {
					   strcpy((char *)base_msg, "B-Black");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Black is dominant\n");
             }
             else if ((rgb.green > rgb.red && rgb.green > rgb.blue)&&(rgb.green>500)) {
					   strcpy((char *)base_msg, "B-Green");
					   transmit_message_C(x, y, direction, base_msg);
                 	   printf("Green is dominant\n");
              } else if (rgb.blue > rgb.red && rgb.blue > rgb.red) {
					   strcpy((char *)base_msg, "B-Blue");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Blue is dominant\n");
  			}else if(rgb.red > rgb.green && rgb.red > rgb.blue) {
					   strcpy((char *)base_msg, "B-Red");
					   transmit_message_C(x, y, direction, base_msg);
                       printf("Red is dominant\n");
            }


 }
// while(1){

// 	if(get_edge(LEFT) == true){
//     printf("Boundary\n");

// 	}else{
// 		printf("NOTHING\n");
// 	}
// }

//MAIN PROGRAM LOOP
 while(1){
	
//int iterations=0;
 stepper_steps(165,165);
 //We assume we go +y direction in the beggining
change_C( direction, y, x, 1);
 
printf("x: %d, y:%d,%s\n",*x,*y,direction);
uint8_t base_msg[] = "Nothing.";
transmit_message_C(x, y, direction, base_msg);
 
 //BOUDNARY CHECK
if (get_edge(LEFT) == true){
	check++;
	if(check == 1){
	sleep_msec(2000);
	stepper_steps(-625,625);//RIGHT TURN
	turn_right(direction);
	//printf("%s\n",direction);
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(2000);
	stepper_steps(1000,1000);
	//1000/165=6
	change_C( direction, y, x, 6);
	sleep_msec(2000);
	stepper_steps(625,-625);//LEFT TURN
	turn_left(direction);
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(2500);
	boundarycheck++;
	}
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
	//This is for side changing
	if(check == 2){
	sleep_msec(2000);
	stepper_steps(625,-625);//LEFT TURN
	turn_left(direction);
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(2000);
	stepper_steps(1000,1000);
	//1000/165=6
	change_C( direction, y, x, 6);
	sleep_msec(2000);
	stepper_steps(-625,625);//LEFT TURN
	turn_left(direction);
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(2500);
	boundarycheck++;
	}
	if(check == 2){
		check = 0;
	}

	if(get_edge(LEFT) == true){
    boundarycheck++;
	}
	if(boundarycheck==2){
		printf("Boundary\n");
		uint8_t base_msg[] = "Boundary";
		transmit_message_C(x, y, direction, base_msg);
		if(turn == 2){
			turn = 0;
		}
		turn++;
	}
	else if(boundarycheck==1){
		check = 0;
		printf("Its a cliff\n");
		uint8_t base_msg[] = "Cliff";
		transmit_message_C(x, y, direction, base_msg);
	}

if(boundarycheck == 2){
	boundarycheck=0;
	sleep_msec(2000);
	stepper_steps(1270,-1270);
	U_turn(direction);
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(2000);
	}
 	}

 	printf("x: %d, y:%d,%s\n",*x,*y,direction);
	//MOUNTAIN SITUATION
  	Mountain_check:
  	int mountain_distance=getDistanceMountainSensor();
  //The beggining of mountain avoidance
	if (mountain_distance <25 ){
	strcpy((char *)base_msg, "Mountain"); 
	transmit_message_C(x, y, direction, base_msg);
	printf("Mountain\n");//Just for the sake of terminal
    stepper_steps(-625,625);//RIGHT TURN
	turn_right(direction);
    sleep_msec(1500);
    stepper_steps(900,900);
	//1000/165=5.45=>5
	change_C( direction, y, x, 5);
    sleep_msec(1500);
    stepper_steps(625,-625); //LEFT TURN
	turn_left(direction);
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
	strcpy((char *)base_msg, "Nothing"); 
	transmit_message_C(x, y, direction, base_msg);
    sleep_msec(1500);
    goto Mountain_check;
}

	int smallBlock = getSmallBlockSensor();
	printf("x: %d, y: %d\n",*x,*y);
	strcpy((char *)base_msg, "Nothing"); 
	transmit_message_C(x, y, direction, base_msg);

	if(smallBlock < 4){
	
	sleep_msec(5000);
	int bigBlock = getBigBlockSensor();
	if(bigBlock < 30){
		sleep_msec(3000);
		get_Big_Block_Colour(rgb.red,rgb.green,rgb.blue);

	//printf("Big Block\n");
	//uint8_t base_msg[] = "Big-block";
	//transmit_message_C(x, y, direction, base_msg);
	}
	else{
		sleep_msec(3000);
		get_Small_Block_Colour(rgb.red,rgb.green,rgb.blue);
		// printf("Small block\n");
		// uint8_t base_msg[] = "Small-block";
		// transmit_message_C(x, y, direction, base_msg);

	}

	sleep_msec(3000);
	stepper_steps(-400,-400);
	//-400/165=-2.42=>-2
	change_C( direction, y, x, -2);
	strcpy((char *)base_msg, "Nothing");
	printf("x: %d, y:%d,%s\n",*x,*y,direction);
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(1000);
	stepper_steps(-625,625);//RIGHT TURN
	turn_right(direction);
	strcpy((char *)base_msg, "Nothing");
	printf("x: %d, y:%d,%s\n",*x,*y,direction); 
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(1000);
	stepper_steps(1000,1000);
		//1000/165=5.45=>5
	change_C( direction, y, x, 5);
	strcpy((char *)base_msg, "Nothing"); 
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(1000);
	stepper_steps(625,-625);//LEFT TURN
	turn_left(direction);
	strcpy((char *)base_msg, "Nothing"); 
	transmit_message_C(x, y, direction, base_msg);
	sleep_msec(1000);
}

	strcpy((char *)base_msg, "Nothing"); 

	transmit_message_C(x, y, direction, base_msg);
}


free(direction);
free(y);
free(x);
return EXIT_SUCCESS;
}