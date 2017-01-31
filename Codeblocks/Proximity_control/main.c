#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>

int main()
{
    int pwm;//   = (int)malloc(sizeof(int)); /* allocate buffer */
    int frequency;

    wiringPiSetup();        // Default WiringPi setup function

    printf("Duty cycle (percentage): ");
    int result = scanf("%d", &pwm);

    if (result == EOF){
        fprintf(stderr, "ERROR: EOF");
        return 0;
    }

    if (pwm>100 || pwm<0){
        fprintf(stderr, "ERROR: invalid input");
        return 0;
    }

    printf("Frequency (hertz): ");
    result = scanf("%d", &frequency);

    if (result == EOF){
        fprintf(stderr, "ERROR: EOF");
        return 0;
    }

    if (frequency>200 || frequency<1){
        fprintf(stderr, "ERROR: invalid input");
        return 0;
    }

    printf("You entered: %d\n", frequency);

    printf("Running PWM function...\n");

    softPwmCreate(5,0,100);

    double pulseTime = 1/frequency; //1 sec
    double timeHigh = pulseTime * pwm / 100; //0.6 sec
    double timeLow = pulseTime - timeHigh; //0.4 sec

    while(1){
        softPwmWrite(5,HIGH); //set high
        usleep(timeHigh*1e6); //sleep for high time
        softPwmWrite(5,LOW); //set low
        usleep(timeLow*1e6); //sleep for low time
    }

    return 0;
}
