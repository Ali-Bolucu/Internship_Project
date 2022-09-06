#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdatomic.h>



_Atomic char* TCP_reply;

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

DWORD WINAPI ThreadFunc() {
    // Do stuff.  This will be the first function called on the new thread.
    // When this function returns, the thread goes away.  See MSDN for more details.




    return 0;
}


int main() {
    HANDLE thread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
    if (thread) {
        // Optionally do stuff, such as wait on the thread.
    }
    while (1) {
        printf("main func ");
        delay(1);
    }





    return 0;
}
