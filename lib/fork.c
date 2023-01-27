// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	void *pg_addr;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	pte_t entry = uvpt[VPN(addr)];
	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.
	//   No need to explicitly delete the old page's mapping.

	// LAB 4: Your code here.
	pte_t entry = uvpt[VPN(addr)];
	if (!(uvpt[VPN(addr)] & PTE_COW)) {
        panic("pgfault: Not a COW page, %e", err);	
    }

    r = sys_page_alloc(0, (void*)PFTEMP, PTE_U | PTE_P | PTE_W);
    if (r < 0) {
        panic("pgfault: could not allocate a page, %e", err);	
    }

    pg_addr = ROUNDDOWN(addr, PGSIZE);
    memmove(PFTEMP, pg_addr, PGSIZE);

    r = sys_page_map(0, (void*)PFTEMP, 0, pg_addr, PTE_U|PTE_W|PTE_P);
    if (r < 0) {
        panic("pgfault: could not map page, %e", err);	
    }

    r = sys_page_unmap(0, PFTEMP);
    if (r < 0) {
        panic("pgfault: could not unmap page");
    }
	return;
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int err;
    int perm;
    pte_t pte;
    void *va;

    pte = uvpt[pn];
    va = (void *) ((uintptr_t) pn * PGSIZE);

    perm = pte & PTE_SYSCALL;

    if (perm & PTE_COW || perm & PTE_W) {
        perm &= ~PTE_W;
        perm |= PTE_COW;

        err = sys_page_map(0, va, envid, va, perm);
        if (err < 0) {
            panic("page_map: page map failed, %e", err);
        }

        err = sys_page_map(0, va, 0, va, perm);
        if (err < 0) {
            panic("page_map: page map failed, %e", err);
        }

        return 0;
    }

    err = sys_page_map(0, va, envid, va, perm);
    if (err < 0) {
        panic("page_map: page map failed, %e", err);
    }
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
 int err;
    envid_t envid;
    int pml4;
    int pdpe;
    int pde;
    int pte;
    int i;
    int j;
    int k;

	set_pgfault_handler(pgfault);
    envid = sys_exofork();
    if (envid < 0) {
        panic("fork: Couldn't fork %e", envid);
    }

    if (envid == 0) {
        thisenv = &envs[ENVX(sys_getenvid())];
        return 0;
    }

	err = sys_page_alloc(envid, (void*)(UXSTACKTOP-PGSIZE), PTE_P|PTE_W|PTE_U);
	if (err < 0) {
        panic("fork: Couldn't fork %e", err);
    }

    for(pml4 = 0; pml4 < VPML4E(UTOP); pml4++) {
		if(!(uvpml4e[pml4] & PTE_P)) {
            continue;
        }
        for (pdpe = 0; pdpe < NPDPENTRIES; pdpe++) {
            i = pml4 * NPDPENTRIES + pdpe;
            if(!(uvpde[i] & PTE_P)) {
                continue;
            }
            for (pde = 0; pde < NPDENTRIES; pde++) {
                j = i * NPDENTRIES + pde;
                if(!(uvpd[j] & PTE_P)) {
                    continue;
                }
                for (pte = 0; pte < NPTENTRIES; pte++) {
                    k = j * NPTENTRIES + pte;
                    if(!(uvpt[k] & PTE_P) || VPN(UXSTACKTOP-PGSIZE) == k) {
                        continue;
                    }
                    err = duppage(envid, (unsigned)k);
                    if (err < 0) {
                        panic("fork: Couldn't fork %e", err);
                    }
                }
            }
        }
    }
	extern void _pgfault_upcall(void);	
	err = sys_env_set_pgfault_upcall(envid, _pgfault_upcall);
	if (err < 0)
		panic("fork: couldn't set pgfault upcall, %e\n", err);

	err = sys_env_set_status(envid, ENV_RUNNABLE);
	if (err < 0)
		panic("fork: couldn't set env status, %e\n", err);
	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
