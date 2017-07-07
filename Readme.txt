Introduce:

    All modules in this part are aimed to be used as lib. This part is independent of
    your application. As usual you should not modify any code here to match your application
    requirement. You can do this correspondingly in 'arch' part.

Contents:

- hal(hardware abstact layer)
    a. drivers for on chip peripheral
    b. common functions for other hardware. eg. spi flash, eeprom, digital sensor...
    
- os(operate system kernel)
    
- service
    a. midlware. this part is hardware independence
    
- utilities
    a. all macros place here.