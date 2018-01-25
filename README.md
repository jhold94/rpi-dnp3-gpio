# ts-dnp3

DNP3 outstation implementation mapped to TS-7680's GPIO and Analog Input pins

# prerequisites

This project and opendnp3 require cmake and GCC 4.8:

```
> sudo apt-get install g++-4.8 cmake
```

# dependencies

The library also uses the [inih](https://github.com/benhoyt/inih) library for reading configuration files. This is specified as a git submodule, so be sure to clone this repository recursively:

```
git clone --recursive https://github.com/jhold94/ts-dnp3.git
```

## opendnp3

Build and install the 2.2.0 release of [opendnp3](https://github.com/automatak/dnp3). Build instructions are [here](https://automatak.com/opendnp3/docs/guide/current/build/cmake/).

```
> CXX=g++-4.9 cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
> make
> sudo make install
```

# building

The build uses cmake:

```
> cd build
> cmake ..
> make
```

#usage 

The program takes a single argument, the path to the INI configuration file:

```
> ./ts-dnp3 ../default.ini
```


