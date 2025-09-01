# NoderyOS

## TODOS

* File write above current cluster
* File creation

## Magic values
| Name               | Start     | End        | Size      |
|--------------------|-----------|------------|-----------|
| Boot disk          | 0x800     | 0x801      | 1 byte    |
| Upper memory       | 0x802     | 0x804      | 2 bytes   |
| Extended memory    | 0x804     | 0x806      | 2 bytes   |
| Kernel location    | 0x1000    | 0x????     | ?? bytes  |
| Master boot record | 0x7c00    | 0x7e00     | 512 bytes |
| Vesa structure     | 0x7e00    | 0x7f00     | 256 bytes |
| END                | OF        | BIOS       | MEMORY    |
| Stack              | 0x90000   | 0x????     | -?? bytes |
| Malloc start       | 0x1000000 | End of RAM |           |