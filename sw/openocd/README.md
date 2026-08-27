## ASTRARV32 OpenOCD Scripts

* `astrarv32.cfg` For the default **single-core** processor configuration
* `astrarv32.dual_core.cfg` For the **SMP dual-core** processor configuration

Example:

```bash
astrarv32/sw/openocd$ openocd -f astrarv32.cfg
```

Helper scripts in [`lib`](lib) are called by the main configuration files
in a specific order to setup the target:

1. `lib/interface.cfg` Physical (JTAG) adapter configuration.
2. `lib/target.cfg` CPU core(s) and GDB configuration.
3. `lib/authenticate.cfg` Authenticate debug access via the RISC-V DM authentication commands
using the _default_ ASTRARV32 authenticator.
4. `lib/start.cfg` Reset and halt target.

To adapt it to your own design, you can customize the included files or replace them entirely.
