# VendingMachine
University project used to design a vending machine utilizing freeRTOS. For simplicity sake I am only uploading the freertos.c file.

## How was the System Structured
I structured the code so there were three tasks that are used by the OS as threads.
* Coin Task
** Would ask the user to select either the $3 coin or the $5 coin.
* Select Task
** Would ask the user to select either snack 1, snack 2, or snack 3.
** Each snack would cost $2 and once the snack was selected it would dispense the remaining change.
* Dispense Task
** Would dispense the snack for the user.

After creating the three tasks we need to go from coin task to select task and vice versa.
So I created a Mutex Semaphore we'll call gatekeeper. Generally only one task can access 
the gatekeeper at a time only if they had the key. At first coin task will have the key
until the user has selected the coin they would like to use ($3 or $5). Once this coin is 
selected the coin task will give the key to the select task. We also enqueue the value of
the coin to the coin queue to be used later.

Once the select task has the key the select task can now access the gatekeeper. The user 
has to select one of the snacks. Once the snack has been selected we save that variable 
and enqueue it to snack queue. After we dequeue the coin variable from earlier and 
depending on that value we dispense the change. At the same time dispense task is 
executing while and dispensing our snack.

So the dispense task dequeues the snack variable and dispenses the snack based on the 
value while concurrently dispensing the remaining change.

After select task is and dispense task complete the key is sent back to the coin task to
access the coin task again.

## What changes would I make
I believe the timing was not utilized to its full extent. I used vTaskDelay() which is not
accurate use of time delays and constraints in terms of milliseconds. If I used 
vTaskDelayUntil() I believe I would be able to make the simulation more accurate.

## Softwares Used
* STM CubeMX
* ST Link
* Keil uVision5
* FreeRTOS (Compiled by STM CubeMX)

## Hardware Used
* STM32F407 DISC-1
