#include <iostream>

#include "sk_os.h"
#include "smmsg.h"

int main() {

    sk_sem_id_t semid;
    sk_sem_create(&semid,"test",2);
    sk_sem_lock(semid);
    sk_sem_lock(semid);
    sk_mem_malloc(32);
    run_smallwin();
    printf("this is test....");
    return 0;
}