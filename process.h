#include "kacchi.h"

#ifndef NPROC
#define NPROC		8
#endif


/* Process state constants */

#define PR_FREE		0	
#define PR_CURR		1	
#define PR_READY	2
#define PR_SUSP		3	
#define PR_WAIT		4	

/* Miscellaneous process definitions */

#define PNMLEN		16	/* Length of process "name"		*/
#define NULLPROC	0	/* ID of the null process		*/	

/* Process initialization constants */

#define INITSTK		65536	
#define INITPRIO	20	
#define MINSTK      128
#define INITRET		userret	

/* Inline code to check process ID (assumes interrupts are disabled)	*/
#define isbadpid(x)	( ((pid32_t)(x) < 0) || \
			  ((pid32_t)(x) >= NPROC) || \
			  (proctab[(x)].prstate == PR_FREE))

/* Number of device descriptors a process can have open */

#define NDESC		5	/* must be odd to make procent 4N bytes	*/

#define DEV_NONE    ((int16_t)-1)
#define DEV_CONSOLE ((int16_t)0)

typedef void (*procfunc_t)(void);

struct pidqueue {
	pid32_t pids[NPROC];
	int32_t count;
};

struct queuetable {
	struct pidqueue ready;
	struct pidqueue wait;
	struct pidqueue sem[NSEM];
};

/* Definition of the process table (multiple of 32 bits) */

struct procent {		
	uint16_t	prstate;	
	pri16_t	    prprio;		
	char	   *prstkptr;	
	char	   *prstkbase;	
	uint32_t	prstklen;	
	char	    prname[PNMLEN];	
	sid32_t	    prsem;		
	pid32_t	    prparent;	
	umsg32_t	prmsg;		
	bool8_t	    prhasmsg;	
	int16_t	    prdesc[NDESC];	

	procfunc_t prentry; //temporary
};

/* Marker for the top of a process stack (used to help detect overflow) */
#define STACKMAGIC	0x1BADB002

extern	struct	procent proctab[];
extern	int32_t	prcount;	
extern	pid32_t	currpid;	

extern char	*sched_sp;
extern char	*proc_sp;

pid32_t create(void (*funcaddr), uint32_t ssize, pri16_t priority, char *name, uint32_t nargs, ...);
pri16_t resume(pid32_t pid);
syscall suspend(pid32_t pid);
syscall kill(pid32_t pid);
void xdone(void);
syscall getprio(pid32_t pid);
pid32_t getpid(void);
pri16_t chprio(pid32_t pid, pri16_t newprio);

void proc_init(void);
int32_t run_one(pid32_t pid);

syscall resched(void);
int ready(pid32_t pid);
void ctxsw(char **old_sp, char *new_sp);

void userret(void);