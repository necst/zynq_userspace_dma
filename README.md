# ZYNQ Userspace DMA: ZU_DMA

ZYNQ Userspace DMA (__ZU_DMA__ for brevity) is a userspace DMA implementation to manage AXI DMA and transmit data to/from custom designs on ZYNQ FPGAs.

## Getting Started

To get started and use __ZU_DMA__, you should first clone the udmabuf repository as a submodule to build the kernel driver

```bash
git submodule update --init --recursive
```
then move into the udmabuf directory and build udmabuf

```bash
cd udmabuf
make
cd ..
```
Finally, move into the lib_dmabuf directory to generate the static library (libdmabuf.a)

```bash
cd lib_dmabuf
make
```
or the dynamic library (libdmabuf.so)

```bash
make dynamic
```

or the tests

```bash
make tests
```

### Prerequisites and assumptions

We developed and tested ZU_DMA in the following environment:

* [PYNQ](http://www.pynq.io) platform v2.1, with Ubuntu 16.04 LTS
* [gcc](https://gcc.gnu.org) 5.3.1, available within PYNQ platform v2.1
* [make](https://www.gnu.org/software/make/), also available within PYNQ platform v2.1
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/) for generating the documentation

ZU_DMA depends on [udmabuf](https://github.com/ikwzm/udmabuf) for getting userspace memory-coherent buffers for DMA transmission.

_**NOTE**: ZU_DMA makes little assumptions on the underlying hardware and software (basically: Linux+Xilinx SoC+Xilinx DMA IP), therefore we are confident it can work on other platforms like [Zedboard](http://zedboard.org/product/zedboard). **Testers are welcome!**_

### Running the tests

ZU_DMA comes with pre-defined tests for the PYNQ platform to check that things work out correctly. Two examples are shown in the following.
More details over tests are available in [tests details](tests/TESTS.md).

##### DMA Passthrough
The DMA Passthrough test consists in a simple AXI Data FIFO connected to a single DMA, which just returns the data it receives. To run the passthrough test, first build it

```bash
cd tests/host_src
make test_passthrough
cd ..
```

then flash the bistream (the provided bitstreams are for PYNQ platform only!); here, we use the `/dev/xdevcfg` device to flash, which needs root permission

```bash
sudo su
cat pynq_test_bitstreams/pynq_test_passthrough.bit > /dev/xdevcfg
```

and finally run the test (as sudo, since it requires inserting the udmabuf module)

```bash
cd host_src
./test_passthrough
```

##### 2D Vector Sum
The 2D Vector Sum test consists in a custom HLS kernel that takes in input:
* 2 integer vectors `V1` and `V2` (as DMA streams) of equal length
* the number of elements inside a vector
* three constants `a`, `b` and `c`

and returns the vector sum `V3 = a * V1 + b + V2 + c` as output DMA stream.

This kernel has a custom AXI control interface to control its running status and to pass inputs, which is also controlled via host software. The steps to run it are similar to the previous test: first build it

```bash
cd tests/host_src
make test_2d_vec_sum
cd ..
```

then flash the bistream

```bash
sudo su
cat pynq_test_bitstreams/pynq_test_2d_vec_sum.bit > /dev/xdevcfg
```

and finally run it

```bash
cd host_src
./test_2d_vec_sum
```

[comment]: # (## Detailed Description of ZU_DMA)

[comment]: # (Add additional notes about how to deploy this on a live system)

[comment]: # (## Contributing)

[comment]: # (Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.)

## Authors

* **Alberto Scolari** - *Initial work*

More authors are welcome!
[comment]: # (See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.)

## License

This project is licensed under the Apache V2 License - see the [LICENSE.md](LICENSE) file for details

[comment]: # (## Acknowledgments)

[comment]: # (* Hat tip to anyone whose code was used)
[comment]: # (* Inspiration)
[comment]: # (* etc)
