/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    tasks.c

  Summary:
    Task functions to implement a task scheduler.

  Description:
    Function to implement a simple task scheduler at "loose" real time. Loose
    real time means that there is no guarantee of real time operation. I.e.,
    in case some "task" blocks the task loop, other tasks will not get executed.
    It is a task of the programmer to ensure that no task will block.
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


#include <xc.h>
#include <stddef.h>
#include "tasks.h"

#define MAX_TASKS 10

typedef struct {
    uint16_t delay;
    uint16_t reload;
    uint8_t trigger;
    void (*taskHandler)(void);
} Task;

Task tasks[MAX_TASKS];

void Task_synch(void)
{
    for(uint8_t i = 0; i<MAX_TASKS; i++)
    {
        if(tasks[i].taskHandler)
        {
            if(tasks[i].delay > 0) tasks[i].delay--;
            if(tasks[i].delay == 0)
            {
                tasks[i].delay = tasks[i].reload;
                tasks[i].trigger = 1;
            }            
        }
    }
}

uint8_t Task_register(uint16_t delay, uint16_t reload, void (* task)(void))
{
    for(uint8_t i = 0; i<MAX_TASKS; i++)
    {
        if(tasks[i].taskHandler == NULL)
        {
            tasks[i].delay = delay;
            tasks[i].reload = reload;
            tasks[i].trigger = 0;
            tasks[i].taskHandler = task;
            return i;
        }
    }
    return TASK_FULL;
}

void Task_delete(uint8_t task_idx)
{
    if(task_idx < MAX_TASKS)
    {
        tasks[task_idx].taskHandler = NULL;
        tasks[task_idx].trigger = 0;        
    }        
}

void Task_modify(uint8_t task_idx, uint16_t reload)
{
    if(task_idx < MAX_TASKS)
    {
        tasks[task_idx].reload = reload;
    }        
}

void Task_execute(void)
{
    for(uint8_t i = 0; i<MAX_TASKS; i++)
    {
        // check for valid function pointer and trigger active
        if(tasks[i].taskHandler && tasks[i].trigger)
        {
            // reset trigger
            tasks[i].trigger = 0;
            // execute task;
            tasks[i].taskHandler();
            // if it is a single-run task, erase the function pointer
            if(tasks[i].reload == 0)
            {
                tasks[i].taskHandler = NULL;
            }
        }
    }    
}