==========
DEPRECATED
==========
This tracing system is deprecated because:
  - Modern debuggers provide full visibility into exit paths and return codes.
  - The instrumentation is not used in the data path, and direct write syscalls are sufficient for initialization logging.

Successor solution will be added in future commits.
Summary
=======

Some functions have counters in the enter-happy exit path, since the
processes can exit without implemented returns (e.g., kpanic).
Within these traces, this possibility can be traced on hexdump.

Since increasing counters is an atomic operation, this implementation is
logical especially on init functions. In functions which will run on the
data path, it can be optional or never implemented.

Usage
=====

Even if the program usually doesn't write anything to the terminal, these
counters can be observed with the GNU debugging tool (gdb).
See more about GNU Debugger on the `official website <https://www.gnu.org/software/gdb/>`_.

This usage example acts as a hands-on guide with gdb, attempting a run
with an invalid parameter.

.. code-block:: console

    $ gdb ./binary

    (gdb) break unbind
    Breakpoint 1 at 0x400862: file pci.c, line 10.

    (gdb) run 0
    Starting program: /path/to/binary/driver 0
    # Note: Driver is binary name, and 0 is parameter.

    This GDB supports auto-downloading debuginfo from the following URLs:
    <ima:enforcing>
    <https://debuginfod.fedoraproject.org/>
    Enable debuginfod for this session? (y or [n]) y
    Debuginfod has been enabled.

    Breakpoint 1, unbind (pci=0xc00000 <error: Cannot access memory...>) at pci.c:10
    10    {

    (gdb) p/x debug_trace
    $1 = {unbind_counter = 0x0, alloc_hugepage_counter = 0x0, virt2phy_counter = 0x0}
    # With this command, we can trace the structure from memory itself.
    # display debug_trace can also be used to do it automatically after every 'next'.

    (gdb) next
    11        if (likely(trace)) {

    (gdb) next
    12            (*trace)++;

    (gdb) p/x debug_trace
    $2 = {unbind_counter = 0x0, alloc_hugepage_counter = 0x0, virt2phy_counter = 0x0}
    # 0x0 means it hasn't entered the function yet.
    # 0x1 means an error (unlikely path).
    # 0x2 is the happy path (exit success).

    (gdb) next
    16        snprintf(path, sizeof(path),

    (gdb) next
    19        fd = open(path, O_WRONLY);

    (gdb) next
    20        if (unlikely(fd < 0)) return -1;

    (gdb) next
    43    }

    (gdb) next
    main (argc=2, argv=0x7fffffffd8e8) at main.c:18
    18        if (unlikely(err != 0)){

    (gdb) next
    19            return -err;

    (gdb) next
    29    };

    (gdb) p/x debug_trace
    $4 = {unbind_counter = 0x1, alloc_hugepage_counter = 0x0, virt2phy_counter = 0x0}

    (gdb) next
    __libc_start_call_main (...) at ../sysdeps/nptl/libc_start_call_main.h:74
    74      exit (result);

    (gdb) p/x debug_trace
    $5 = {unbind_counter = 0x1, alloc_hugepage_counter = 0x0, virt2phy_counter = 0x0}

    (gdb) next
    [Inferior 1 (process 55552) exited with code 01]

    # After exiting the program, we can't see debug_trace since it will not be here.
    # Keep in mind that 'echo $?' will return the latest exit code.