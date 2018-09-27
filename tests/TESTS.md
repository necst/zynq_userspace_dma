## ZU_DMA tests

ZU_DMA features several tests to try out data transmission patterns to/from FPGA and explore how the software (host) side and the hardware side should be designed. The following folders contain the relevant data:
* [pynq_test_bitstreams](./pynq_test_bitstreams) contains pre-built bitstreams of the available designs (for PYNQ); these bitstreams can be flashed to the FPGA (e.g. via the `/dev/xdevcfg` device ) and tested
* [host_src](./host_src) contains the host-side code with one test per bitstream; tests send data to the FPGA logic, retrieve the result and check it to be correct
* [designs](./designs) contains the High Level Synthesis and Block Diagram design of each project, as well as scripts to synthesize it for PYNQ

### Flash the bitstreams

To flash the bistream of a test, the suggested procedure uses the `/dev/xdevcfg` device; therefore, you should flash it with sudo permission
```bash
sudo su
cat bitstreams/<test name>.bit > /dev/xdevcfg
exit
```

### Compile the test

To compile the test, a Makefile is provided, with pre-defined targets; to build a specific test, run
```bash
make test_<test name>
````
and make will compile and link together:

* the ZU_DMA library
* the local utils library, with debugging utilities foo developers' convenience
* the test_<test name>.c file

and finally run it
```bash
./test_<test name>
```
To compile all tests, run
```bash
make
```
To clean intermediate products (but not binaries), run
```bash
make clean
```
and
```bash
make distclean
````
to clean binaries as well.

### Build the bitstreams yourself

The [design](./design) folder contains the designs with script to build them. Each test folder contains

* the file block_design.png, showing the system architecture
* the hls folder if the test depends on HLS kernels
* the vivado folder with the script to build and synthesize the bitstream

To build the bitstream yourself, the script synth.sh can be used, which invokes the Vivado tools and the TCL script in each directory.
__NOTE__: the TCL script used were built and tested under __Vivado (HLS) 2017.2__; they check the version they run in, so they might fail in case of mismatch. It should be easy to modify them to work with another version of Vivado.
To build the test *`<test name>`*, you need to source the Vivado tools and invoke the [synth.sh](./designs/synth.sh) script like
```bash
source <path/to/Vivado/installation>
source <path/to/Vivado HLS/installation>

./synth.sh build <test name>
```
which will start the building process (HLS+Vivado) and leave the bitstream `<test name>`.bit in the design folder.
To clean after a build
```bash
./synth.sh clean <test name>
```
For a description of each test, check the file DESIGN.md inside each test directory.
