Introduce:

    All modules in this part are aimed to be used as library. This part is independent of
    your application. As usual you should not modify any code here to match your application
    requirement. You might correspondingly do this in 'arch' directory.

Contents:

- driver
    a. contains low-level software drivers for MCU on-chip resources (eg cpu, usart, adc drivers).
    b. offers, for each supported hardware component, a software
       interface to interact with the component (eg memories like SDRAM, SD card, or display)
    
- os(operate system and kernel)
    
- service
    a. middleware. (eg. FAT, TCP/IP stack, os, JPEG decoder).
    
- utilities
    C utility files with general usage defines and macros.