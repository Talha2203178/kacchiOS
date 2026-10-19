#include "process.h"

void userret(void)
{
    kill(currpid); //currpid is temporary
}