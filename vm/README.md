# Virtual Machine Project
This project is based on the tutorial found [here](https://www.jmeiners.com/lc3-vm/)

This VM is built to simulate the LC-3 architecture. The documentation for the architecture can be found [here](https://www.jmeiners.com/lc3-vm/supplies/lc3-isa.pdf).

### Running the project
This project just requires that you a C compiler (for example `gcc`).

Once installed you can run `make` from this directory to build the project
* this will create a new directory called `build`

There are 2 example object files to simulate running the VM, found in the `examples` folder

Once the project has been built, run one of the example programs with
* `./build/lc3-vm examples/<example_file>`
* where `<example_file>` is the name of one of the files found in the `examples` folder
