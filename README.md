<a name="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/benzammour/cretro">
    <img src=".github/images/logo.svg" alt="Logo" width="128" height="128">
  </a>

<h3 align="center">cretro</h3>

  <p align="center">
    Yet another Chip8-Emulator!
    <br />
    <br />
    <a href="https://github.com/Benzammour/cretro/blob/main/.github/images/snake.gif">View Demo</a>
    ·
    <a href="https://github.com/benzammour/cretro/issues">Report Bug</a>
    ·
    <a href="https://github.com/benzammour/cretro/issues">Request Feature</a>
  </p>
</div>


## About The Project

In my attempt to learn more C and get my feet wet with Emulator Development I started writing a Gameboy Emulator...which ended in this project.
Realizing that the Gameboy was too ambitious initially I started **cretro**, a fully featured chip 8 emulator which also includes keybindings and sound.

[Here is the Emulator running snake!](https://github.com/Benzammour/cretro/blob/main/.github/images/snake.gif)

## Getting Started

As this project uses Linux/POSIX APIs, namely `pthread.h` and `sys/time.h` the project will only work on Linux right now.
However, if you are using Windows, using [WSL2](https://learn.microsoft.com/en-us/windows/wsl/install) works fine.

### Prerequisites

This project uses gcc, make and SDL2. Go check them out if you don't have them locally installed.

On Debian-based systems you can run the following to install the dependencies
```bash
$ sudo apt update
$ sudo apt install -y build-essential libsdl2-dev make
```

### Installation

1. Clone the Repository
   ```bash
   git clone https://github.com/benzammour/cretro.git
   ```
2. Build the Project
   ```bash
   mkdir build
   cd build
   cmake -G Ninja ..
   ninja
   ```
3. Optionally, you can install the resulting binary using
   ```
   ninja install-bin
   ```
   It can be removed using `ninja uninstall-bin`.

## Usage

To run a ROM, pass the path into `cretro`, right after your configured clock-delay (in milliseconds):

```bash
   ./cretro [-d <0..4>] [-f <FREQUENCY>] <ROM>
```

An example would be 
```bash
   ./cretro roms/bat.ch8
```
To initially test the emulator, I created the [bat.ch8](https://github.com/Benzammour/cretro/blob/main/roms/bat.ch8) ROM for you:
[![Bat Demo][bat-demo]](https://github.com/Benzammour/cretro/blob/main/.github/images/bat.png)


<p align="right">(<a href="#readme-top">back to top</a>)</p>


## Contributing

If you have a suggestion that would make this better, please fork the repo and create a pull request.
You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/cool-feature`)
3. Implement your desired Feature!
3. Commit your Changes (`git commit -am 'Add some cool-feature'`)
4. Push to the Branch (`git push origin feature/cool-feature`)
5. Create a new [Pull Request](https://github.com/benzammour/cretro/pulls)

## License

Distributed under the MIT License. See `LICENSE` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

[bat-demo]: .github/images/bat.png
[snake-demo]: .github/images/snake.gif
