#include "kernel.h"

/*
  1. Check if a free process slot exists and if the there's enough free space (check allocated_pages).
  2. Alloc space for page_table (the size of it depends on how many pages you need) and update allocated_pages.
  3. The mapping to kernel-managed memory is not built up, all the PFN should be set to -1 and present byte to 0.
  4. Return a pid (the index in MMStruct array) which is >= 0 when success, -1 when failure in any above step.
*/
int proc_create_vm(struct Kernel *kernel, int size)
{
    // Check if a free process slot exists and if the there's enough free space (check allocated_pages).
    int pid = -1;
    for (int i = 0; i < MAX_PROCESS_NUM; i++)
    {
        if (kernel->running[i] == 0)
        {
            pid = i;
            break;
        }
    }

    // If there is no free process slot, return -1.
    if (pid == -1)
    {
        return -1;
    }

    // If there is not enough free space, return -1.
    if (kernel->allocated_pages * PAGE_SIZE + size > KERNEL_SPACE_SIZE)
    {
        return -1;
    }

    if (size > VIRTUAL_SPACE_SIZE)
    {
        return -1;
    }

    // Alloc space for page_table (the size of it depends on how many pages you need) and update allocated_pages.
    int pages = size / PAGE_SIZE + (size % PAGE_SIZE == 0 ? 0 : 1);

    kernel->mm[pid].size = size;
    kernel->mm[pid].page_table = (struct PageTable *)malloc(sizeof(struct PageTable));
    kernel->mm[pid].page_table->ptes = (struct PTE *)malloc(sizeof(struct PTE) * pages);
    for (int i = 0; i < pages; i++)
    {
        kernel->mm[pid].page_table->ptes[i].PFN = -1;
        kernel->mm[pid].page_table->ptes[i].present = 0;
    }
    kernel->allocated_pages += pages;
    kernel->running[pid] = 1;

    // Return a pid (the index in MMStruct array) which is >= 0 when success, -1 when failure in any above step.
    return pid;
}

/*
  This function will read the range [addr, addr+size) from user space of a specific process to the buf (buf should be >=
  size).
  1. Check if the reading range is out-of-bounds.
  2. If the pages in the range [addr, addr+size) of the user space of that process are not present,
     you should firstly map them to the free kernel-managed memory pages (first fit policy).
  Return 0 when success, -1 when failure (out of bounds).
*/
int vm_read(struct Kernel *kernel, int pid, char *addr, int size, char *buf)
{
    // Check if the pid is valid.
    if (pid < 0 || pid >= MAX_PROCESS_NUM)
    {
        return -1;
    }

    // Check if the process is running.
    if (kernel->running[pid] == 0)
    {
        return -1;
    }

    // Check if the reading range is out-of-bounds.
    if ((int)addr < 0 || (int)addr + size > kernel->mm[pid].size)
    {
        return -1;
    }

    int start_page = (int)addr / PAGE_SIZE;
    int end_page = ((int)addr + size - 1) / PAGE_SIZE;
    int size_remain = size;

    // If the pages in the range [addr, addr+size) of the user space of that process are not present, you should firstly
    // map them to the free kernel-managed memory pages (first fit policy).
    for (int i = start_page; i <= end_page; i++)
    {
        if (kernel->mm[pid].page_table->ptes[i].present == 0)
        {
            // Find the first free page.
            int pfn = -1;
            for (int j = 0; j < KERNEL_SPACE_SIZE / PAGE_SIZE; j++)
            {
                if (kernel->occupied_pages[j] == 0)
                {
                    pfn = j;
                    break;
                }
            }

            // If there is no free page, return -1.
            if (pfn == -1)
            {
                return -1;
            }

            // Map the page.
            kernel->mm[pid].page_table->ptes[i].PFN = pfn;
            kernel->mm[pid].page_table->ptes[i].present = 1;
            kernel->occupied_pages[pfn] = 1;

            // Initialize the page.
            for (int j = 0; j < PAGE_SIZE; j++)
            {
                kernel->space[pfn * PAGE_SIZE + j] = 0;
            }
        }
        // Read the page.
        for (int j = 0; j < PAGE_SIZE || size_remain > 0; j++)
        {
            buf[size - size_remain] = kernel->space[kernel->mm[pid].page_table->ptes[i].PFN * PAGE_SIZE + j];
            size_remain--;
        }
    }

    // Return 0 when success, -1 when failure (out of bounds).
    return 0;
}

/*
  This function will write the content of buf to user space [addr, addr+size) (buf should be >= size).
  1. Check if the writing range is out-of-bounds.
  2. If the pages in the range [addr, addr+size) of the user space of that process are not present,
     you should firstly map them to the free kernel-managed memory pages (first fit policy).
  Return 0 when success, -1 when failure (out of bounds).
*/
int vm_write(struct Kernel *kernel, int pid, char *addr, int size, char *buf)
{
    // Check if the pid is valid.
    if (pid < 0 || pid >= MAX_PROCESS_NUM)
    {
        return -1;
    }

    // Check if the process is running.
    if (kernel->running[pid] == 0)
    {
        return -1;
    }

    // Check if the writing range is out-of-bounds.
    if ((int)addr < 0 || (int)addr + size > kernel->mm[pid].size)
    {
        return -1;
    }

    // If the pages in the range [addr, addr+size) of the user space of that process are not present, you should
    // firstly map them to the free kernel-managed memory pages (first fit policy).
    int start_page = (int)addr / PAGE_SIZE;
    int end_page = ((int)addr + size - 1) / PAGE_SIZE;
    int size_remain = size;

    for (int i = start_page; i <= end_page; i++)
    {
        if (kernel->mm[pid].page_table->ptes[i].present == 0)
        {
            // Find the first free page.
            int pfn = -1;
            for (int j = 0; j < KERNEL_SPACE_SIZE / PAGE_SIZE; j++)
            {
                if (kernel->occupied_pages[j] == 0)
                {
                    pfn = j;
                    break;
                }
            }

            // If there is no free page, return -1.
            if (pfn == -1)
            {
                return -1;
            }

            // Map the page.
            kernel->mm[pid].page_table->ptes[i].PFN = pfn;
            kernel->mm[pid].page_table->ptes[i].present = 1;
            kernel->occupied_pages[pfn] = 1;
        }

        // Write the page.
        for (int j = 0; j < PAGE_SIZE || size_remain > 0; j++)
        {
            kernel->space[kernel->mm[pid].page_table->ptes[i].PFN * PAGE_SIZE + j] = buf[size - size_remain];
            size_remain--;
        }
    }

    // Return 0 when success, -1 when failure (out of bounds).
    return 0;
}

/*
  This function will free the space of a process.
  1. Reset the corresponding pages in occupied_pages to 0.
  2. Release the page_table in the corresponding MMStruct and set to NULL.
  Return 0 when success, -1 when failure.
*/
int proc_exit_vm(struct Kernel *kernel, int pid)
{
    // Check if the process is running.
    if (kernel->running[pid] == 0)
    {
        return -1;
    }

    // Reset the corresponding pages in occupied_pages to 0.
    for (int i = 0; i < (kernel->mm[pid].size + PAGE_SIZE - 1) / PAGE_SIZE; i++)
    {
        if (kernel->mm[pid].page_table->ptes[i].present == 1)
        {
            int page = kernel->mm[pid].page_table->ptes[i].PFN;
            kernel->occupied_pages[page] = 0;
        }
    }

    // Reset the corresponding pages in running to 0.
    kernel->running[pid] = 0;

    // Release the page_table in the corresponding MMStruct and set to NULL.
    free(kernel->mm[pid].page_table->ptes);
    free(kernel->mm[pid].page_table);
    kernel->mm[pid].page_table = NULL;

    // Return 0 when success, -1 when failure.
    return 0;
}
