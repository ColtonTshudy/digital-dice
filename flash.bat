@REM cmake --build C:/Users/Tshud/Documents/GitHub/lcd-stm32f4/build/Debug --

@REM openocd -f interface/stlink.cfg -f target/stm32f4x.cfg ^
@REM     -c init ^
@REM     -c "reset init" ^
@REM     -c "program build/Debug/lcd-stm32f4.elf" ^
@REM     -c reset ^
@REM     -c shutdown

probe-rs run --chip STM32F401CCU8 .\build\Debug\lcd-stm32f4.elf