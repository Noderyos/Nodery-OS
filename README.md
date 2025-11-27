# NoderyOS

## TODOS

* File write above current cluster
* File creation

## Physical memory
| Name               | Start      | End        | Size      |
|--------------------|------------|------------|-----------|
| Boot disk          | 0x800      | 0x801      | 1 byte    |
| Upper memory       | 0x802      | 0x804      | 2 bytes   |
| Extended memory    | 0x804      | 0x806      | 2 bytes   |
| Master boot record | 0x7c00     | 0x7e00     | 512 bytes |
| Vesa structure     | 0x7e00     | 0x7f00     | 256 bytes |
| Kernel location    | 0x10000    | 0x????     | ?? bytes  |
| Stack              | 0x90000    | 0x????     | -?? bytes |
| Alloc start        | 0x400000   | -          | ?? bytes  |

## Kernel virtual memory
| Name               | Start      | End        | Size      |
|--------------------|------------|------------|-----------|
| Identity mapping   | 0x0        | 0x400000   | 4 Mbytes  |
| Malloc memory      | 0xE0000000 | 0x10000000 | 256 Mbytes|
| Page Dir/Tbl       | 0xFFC00000 | -          | 4 Mbytes  |

## Program virtual memory
| Name               | Start      | End        | Size      |
|--------------------|------------|------------|-----------|
| Stack              | 0xD0000000 | 0xD0001000 | 4 kbytes  |
| ELF content        | 0x08048000 | 0x??       | ?? bytes  |
