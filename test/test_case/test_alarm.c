#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int global_count = 0; 

void sig_alarm_loop() 
{ 
    if (global_count >= 10) {
        exit(0);
        return;
    }
    global_count++;
    printf("wakup by alarm %d\n", global_count);
    // signal(SIGALRM, sig_alarm_loop); 
    alarm(1);
} 

int main(int argc, char *argv[]) 
{ 
    signal(SIGALRM, sig_alarm_loop); 
    sig_alarm_loop();
    // sleep(5); 
    while (1) {
        sleep(1);
    }
    // printf("Hello World!\n"); 
    return 0; 
}