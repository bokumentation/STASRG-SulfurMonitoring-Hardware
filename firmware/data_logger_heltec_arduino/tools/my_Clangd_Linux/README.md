## My Settings for my ClangD
If you wanna use it, just change the `loq` to your os username. In this example i using Linux (Debian).

### Example
```clangd
CompileFlags:
  ...

    - -isystem
    - "/home/<your_os_username>/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20241119/xtensa-esp-elf/xtensa-esp-elf/include"
    
    - -isystem
    - "/home/<your_os_username>/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20241119/xtensa-esp-elf/xtensa-esp-elf/include/c++/14.2.0"
    
    - -isystem
    - "/home/<your_os_username>/.espressif/tools/xtensa-esp-elf/esp-14.2.0_20241119/xtensa-esp-elf/xtensa-esp-elf/include/c++/14.2.0/xtensa-esp-elf"

    - -std=c++17
  
  ...

```