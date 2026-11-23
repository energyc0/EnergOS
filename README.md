# EnergOS

A simple RTOS for STM32 NUCLEO F411RE boards.

The project is built with a Cortex-M4 cross compiler. The kernel and user tasks
are compiled together with STM32 HAL into `.elf` and `.bin` firmware images.

The project can be configured for a different platform; all platform-dependent components have been moved into separate modules.

## Requirements

- CMake 3.31 or newer;
- ARM GCC: `arm-none-eabi-gcc`;
- ARM Binutils: `arm-none-eabi-objcopy`;
- OpenOCD for flashing and debugging;
- Minicom for serial communication;
- `gdb-multiarch` for debugging;
- `qemu-system-arm` for tests.

On Ubuntu or Debian, install the required packages with:

```sh
sudo apt-get update
sudo apt-get install cmake gcc-arm-none-eabi binutils-arm-none-eabi \
  libnewlib-arm-none-eabi openocd minicom gdb-multiarch qemu-system-arm
```

After cloning the repository, initialize its submodules:

```sh
git submodule update --init --recursive
```

## Building the firmware

Use [toolchain/cortex-m4.cmake](toolchain/cortex-m4.cmake) for the built-in STM32F411RE profile:

```sh
cmake -B build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_TOOLCHAIN_FILE=toolchain/cortex-m4.cmake
cmake --build build
```

Build artifacts are written to [build/kernel/](build/kernel/):

- `energos.elf` — firmware for debugging and flashing;
- `energos.bin` — binary firmware image;
- `energos.map` — memory and section map.

To build with debug symbols, specify the build type:

```sh
cmake -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_TOOLCHAIN_FILE=toolchain/cortex-m4.cmake
cmake --build build
```

The build directory does not need to be reconfigured for a normal rebuild.

## Flashing and working with the MCU

The toolchain uses these defaults:

- serial device: `/dev/ttyACM0`;
- UART baud rate: `115200`;
- ST-Link interface: `interface/stlink.cfg`;
- OpenOCD target: `target/stm32f4x.cfg`.

Connect the board and start the OpenOCD debug server:

```sh
cmake --build build --target debug_server
```

Flash the firmware to the MCU with:

```sh
cmake --build build --target flash
```

For debugging, start `debug_server`, then run this in another terminal:

```sh
cmake --build build --target gdb
```

To view UART output, open the serial port with Minicom:

```sh
cmake --build build --target minicom
```

If the board is connected through a different device, change `MCU_DEVICE_FILE`
in the toolchain file before configuring the project.

## Tests

The tests are compiled for Cortex-M4 and run with QEMU. The task and
synchronization tests compile production kernel sources and redirect platform
functions to the QEMU test port. The port test runs the real Cortex-M4 port
implementation.

Configure a separate build with tests enabled:

```sh
cmake -B build \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_TOOLCHAIN_FILE=toolchain/cortex-m4.cmake \
  -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build/tests --output-on-failure
```

QEMU uses the `netduinoplus2` machine, which emulates a Cortex-M4 board.
Running the tests requires `arm-none-eabi-gcc` and `qemu-system-arm`.

Check the build and source formatting with:

```sh
./scripts/build_and_lint.sh
```

The script builds the firmware, runs `run-clang-tidy`, and checks source
formatting with `clang-format-22`.

## Project structure

- `kernel/` — OS core, task scheduler, synchronization, and system calls;
- `portable/` — platform support code and task context switching;
- `portable/stm32f411re/` — STM32F411RE port implementation;
- `user/` — user tasks, console, and system services;
- `util/` — shared utility functions;
- `tests/` — tests and startup code for QEMU;
- `toolchain/` — CMake toolchain and linker scripts;
- `third_party/` — STM32 HAL and third-party components.

# Contacts

- **Telegram**: @energyc0
- **Email**: maksim.gavrilov.0@mail.ru
