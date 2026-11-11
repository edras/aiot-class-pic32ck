/*******************************************************************************
 *  Task scheduler module for loose real time control of tasks
 *
 *  Company:
 *    Microchip Technology Inc.
 *
 *  File Name:
 *    tasks.h
 *
 *  Summary:
 *    Task functions to implement a task scheduler.
 *
 *  Description:
 *    Function to implement a simple task scheduler at "loose" real time. Loose
 *    real time means that there is no guarantee of real time operation. I.e.,
 *    in case some "task" blocks the task loop, other tasks will not get executed.
 *    It is a task of the programmer to ensure that no task will block.
 * 
 *  How to use this module:
 *    1. Call Task_synch each 1 ms
 *    2. Register the task at program beginning or when needed.
 *    3. Call Task_execute inside the main loop
 *    To register a task:
 *      a. define a delay to start the task. delay = 0 start task immediately.
 *      b. define a reload time to repeat the task. reload = 0 execute the task
 *         only once and the resources for the task are released.
 *      c. pass a function pointer with no parameter and now return as a task.
 *         i.e.: void doSomething(void)
 *      Example: execute doSomething immediately and repeat it each 50ms:
 *      - Task_register(0, 50, doSomething);
 *      Example: execute doSomething after 100 ms and repeat it each 200ms:
 *      - Task_register(100, 200, doSomething);
 *      Example: execute doSomething only once after 200 ms:
 *      - Task_register(200, 0, doSomething);
 * 
 *******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef TASKS_H
#define	TASKS_H

#define TASK_FULL 0xFF

#include <xc.h> // include processor files - each processor file is guarded.  

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

void Task_execute(void);
void Task_delete(uint8_t task_idx);
void Task_modify(uint8_t task_idx, uint16_t reload);
void Task_synch(void);
uint8_t Task_register(uint16_t delay, uint16_t reload, void (* task)(void));

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* TASKS_H */

