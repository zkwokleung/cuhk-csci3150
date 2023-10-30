#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#pragma region My Debug Functions
// Enums
#define DEBUG_LEVEL_NONE 100
#define DEBUG_LEVEL_ALL -1
#define DEBUG_LEVEL_LOG 0
#define DEBUG_LEVEL_WARN 1
#define DEBUG_LEVEL_ERROR 2

// * Change the log level here
#define DEBUG_LEVEL DEBUG_LEVEL_ALL

#define debug_log(...)                                                                                                 \
    if (DEBUG_LEVEL <= DEBUG_LEVEL_LOG)                                                                                \
        printf(__VA_ARGS__);

#define debug_warn(...)                                                                                                \
    if (DEBUG_LEVEL <= DEBUG_LEVEL_WARN)                                                                               \
        printf(__VA_ARGS__);

#define debug_error(...)                                                                                               \
    if (DEBUG_LEVEL <= DEBUG_LEVEL_ERROR)                                                                              \
        printf(__VA_ARGS__);
#pragma endregion

void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time);
void sort_process_by_id(Process *p, int num, int accending);

// Implement by students
void scheduler(Process *proc, LinkedQueue **ProcessQueue, int proc_num, int queue_num, int period)
{
    printf("Process number: %d\n", proc_num);
    for (int i = 0; i < proc_num; i++)
        printf("%d %d %d\n", proc[i].process_id, proc[i].arrival_time, proc[i].execution_time);

    printf("\nQueue number: %d\n", queue_num);
    printf("Period: %d\n", period);
    for (int i = 0; i < queue_num; i++)
    {
        printf("%d %d %d\n", i, ProcessQueue[i]->time_slice, ProcessQueue[i]->allotment_time);
    }

    int proc_remain = proc_num;
    int time = 0;
    int slice_time = 0;
    int slice_flag = 0;
    int queue_index = -1;

    // Sort the queue by process id
    sort_process_by_id(proc, proc_num, 1);

    while (proc_remain)
    {
        // check if any process arrives
        for (int i = 0; i < proc_num; i++)
        {
            if (proc[i].arrival_time == time)
            {
                // Initialize the process's uninitialized variables
                proc[i].waiting_time = 0;
                proc[i].turnaround_time = 0;
                proc[i].completion_time = 0;
                proc[i].service_time = 0;

                // enqueue to the last queue
                ProcessQueue[queue_num - 1] = EnQueue(ProcessQueue[queue_num - 1], proc[i]);
                debug_log("?? Process %d entered the queue %d for the first time at time %d\n", proc[i].process_id,
                          queue_num - 1, time);
            }
            else
            {
                proc[i].waiting_time++;
            }
        }

        // Check whether there is any process in the queue according to the priority
        if (queue_index == -1)
        {
            for (int i = queue_num - 1; i >= 0; i--)
            {
                if (!IsEmptyQueue(ProcessQueue[i]))
                {
                    queue_index = i;
                    break;
                }
            }
        }

        // If there is a process in the queue, execute it
        if (queue_index != -1)
        {
            if (!IsEmptyQueue(ProcessQueue[queue_index]))
            {
                Process proc = FrontQueue(ProcessQueue[queue_index]);

                // Execute the process
                proc.execution_time -= slice_time;
                proc.service_time += slice_time;

                // Check if the process is finished
                if (proc.execution_time <= 0)
                {
                    DeQueue(ProcessQueue[queue_index]);

                    proc.completion_time = time;
                    proc.turnaround_time = proc.completion_time - proc.arrival_time;
                    proc_remain--;

                    outprint(time - slice_time, time, proc.process_id, proc.arrival_time, proc.execution_time);
                    debug_log(">> Process %d finished at time %d\n", proc.process_id, time);

                    // Reset the slice time
                    slice_time = 0;

                    slice_flag = 1;
                }
                else if (slice_time == ProcessQueue[queue_index]->time_slice)
                {
                    DeQueue(ProcessQueue[queue_index]);

                    // Check whether the process has been running in the same queue for a period
                    if (queue_index > 0 && proc.service_time >= ProcessQueue[queue_index]->allotment_time)
                    {
                        // If yes, move the process to the next queue if it is not the last queue
                        proc.service_time = 0;
                        ProcessQueue[queue_index + 1] = EnQueue(ProcessQueue[queue_index - 1], proc);
                        debug_log("Process %d was downgraded to queue %d at time %d\n", proc.process_id,
                                  queue_index - 1, time);
                    }
                    else
                    {
                        // If no, enqueue the process back to the same queue
                        ProcessQueue[queue_index] = EnQueue(ProcessQueue[queue_index], proc);
                        debug_log("Enqueue process %d to queue %d at time %d\n", proc.process_id, queue_index, time);
                    }

                    outprint(time - slice_time, time, proc.process_id, proc.arrival_time, proc.execution_time);

                    // Reset the slice time
                    slice_time = 0;

                    slice_flag = 1;
                }
                else
                {
                    slice_time++;
                }
            }
        }

        // Check whether there is a slice happened
        if (slice_flag == 0)
        {
            // If not, proceed to the next time
            time++;

            // If the time reaches the period, move all the processes in the queue to the last queue (highest priority)
            if (time % period == 0)
            {
                // Create a temporary array to store the processes in the queue
                Process *temp = (Process *)malloc(sizeof(Process) * proc_num);
                int temp_index = 0;
                for (int i = 0; i < queue_num - 1; i++)
                {
                    while (!IsEmptyQueue(ProcessQueue[i]))
                    {
                        temp[temp_index++] = DeQueue(ProcessQueue[i]);
                    }
                }

                // Sort the temporary array by process id
                sort_process_by_id(temp, temp_index, 1);

                // Enqueue the processes in the temporary array to the last queue
                for (int i = 0; i < temp_index; i++)
                {
                    ProcessQueue[queue_num - 1] = EnQueue(ProcessQueue[queue_num - 1], temp[i]);
                    debug_log("Process %d was downgraded to queue %d at time %d\n", temp[i].process_id, queue_num - 1,
                              time);
                }

                // Free the temporary array
                free(temp);
            }
        }
        else
        {
            // If yes, reset the queue index so that the scheduler will check the queue again
            queue_index = -1;
        }

        slice_flag = 0;
    }
}

void sort_process_by_id(Process *p, int num, int accending)
{
    for (int i = 0; i < num; i++)
    {
        for (int j = i + 1; j < num; j++)
        {
            if ((accending && p[i].process_id > p[j].process_id) || (!accending && p[i].process_id < p[j].process_id))
            {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
}