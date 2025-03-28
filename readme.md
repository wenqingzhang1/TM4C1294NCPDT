# Tiva TM4C1294 Launchpad FreeRTOS Demo #

This firmware package is intended to provide a starting point for creating a FreeRTOS 9.0 based project in Code Composer Studio for the Tiva TM4C1294 launchpad, which is currently not supplied by either FreeRTOS or Texas Instruments.

## Required Components ##
* Code Composer Studio (v6.0.1 used for this project)
* [TM4C1294 Connected Launchpad](http://www.ti.com/tool/ek-tm4c1294xl)
* [Artistic Style](http://astyle.sourceforge.net/astyle.html) (optional for formatted code)

## Expected Output ##
* Code will build with no errors or warnings
* Green LEDs D1 - D4 will blink in sequence 1 per second
* Serial port via Stellaris Virtual Serial Port will output "Hello, world from FreeRTOS 9.0!"

## Notes ##
This software is provided as-is!
## Task ##
* 1.Start with the CCS Project with configuredFreeRTOS utilizing Three Tasks. Given in lecture.(configuredFreeRTOS）
* 2.The behavior of the provided code does the following:
a.Press Button 1 and the LED will activate/deactivate in a sequence from Right to Left.（按下按钮一，LED从左到右依次激活或者禁用）
b.Press Button 2 and the LED will activate/deactivate in a sequence from Left to Right.（按下按妞二，LED从右到左依次激活或者禁用）
* 3.Create 4 task each of which controls 1 LED（创造四个任务分别控制一个LED)
* 4.Pressing each of the respective buttons the LEDs should start flashing in the same pattern as the test code.(按下各个按钮，LED 指示灯应开始以与测试代码相同的模式闪烁)
* 5.You will need to utilize Task scheduling and periodic execution to perform this desired task.(您需要利用任务调度和定期执行来完成这项预期任务)
Note: Task management is NOT a trivial task.

