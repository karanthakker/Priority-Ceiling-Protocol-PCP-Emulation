SET PATH=D:\Keil\ARM\ARMCC\bin\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\system32\wbem;c:\program files\common files\acronis\snapapi\;D:\Program Files\MATLAB\R2009a\bin;D:\Program Files\MATLAB\R2009a\bin\win32;C:\Program Files\ATI Technologies\ATI Control Panel;C:\WINDOWS\system32\WindowsPowerShell\v1.0
SET CPU_TYPE=TM4C123GH6PM
SET CPU_VENDOR=Texas Instruments
SET UV2_TARGET=freertos_project
SET CPU_CLOCK=0x04C4B400
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\FREERT~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\freertos_project.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\heap_2._ip"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\heap_2.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\list._ip"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\list.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\port._ip"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\port.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\queue._ip"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\queue.__i"
"D:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\rvmdk\startup_rvmdk._ia"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\tasks._ip"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\tasks.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\led_util._ip"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\led_util.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\LED_BL~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\led_blue_task.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\LED_GR~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\led_green_task.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\LED_RE~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\led_red_task.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\TASK_U~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\task_util.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\TEST1_~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\test1_task.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\TEST2_~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\test2_task.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\UART_U~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\uart_util.__i"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via "rvmdk\UARTST~1._IP"
"D:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\rvmdk\uartstdio.__i"
"D:\Keil\ARM\ARMCC\bin\ArmLink" --Via ".\rvmdk\freertos_project.lnp"
D:\Keil\ARM\ARMCC\bin\fromelf.exe ".\rvmdk\freertos_project.axf" --i32combined --output ".\rvmdk\freertos_project.hex"
fromelf --bin --output .\rvmdk\freertos_project.bin .\rvmdk\freertos_project.axf
