# cpputils

my cpp library, and some demos.

## build

### dependencies

[gif-h](https://github.com/charlietangora/gif-h) for gif r/w. install from git:

```shell
git clone git@github.com:charlietangora/gif-h.git
sudo cp gif-h/gif.h /usr/local/include/
```

`libpng`,`libjpeg`,`xcb`,`gtk2`,`opengl`,`glew(windows)` just install from package manager like `apt` or `yay`.

```shell
# archlinux
yay -S mesa glu freeglut libpng libjpeg-turbo libxcb gtk2
# ubuntu
apt install libgtk2.0-dev freeglut3-dev libpng-dev libjpeg-turbo8-dev libxcb1-dev
```

### unnecessary dependencies
used in test or some demo.

- [conio.h](https://github.com/zoelabbb/conio.h)
  ```shell
  git clone https://github.com/zoelabbb/conio.h.git
  sudo cp conio.h/conio.h /usr/local/include
  ```
- boost-test
  `apt install libboost-test-dev`

