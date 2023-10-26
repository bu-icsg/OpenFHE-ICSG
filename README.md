OpenFHE - Open-Source Fully Homomorphic Encryption Library
Copy for ICSG testing purposes
=====================================

## To build project in SCC using Singularity Container

Follow the following steps:
    * Go to [SCC](https://scc-ondemand1.bu.edu/pun/sys/dashboard) and go to a login node
    * ssh into scc-i01 using ```ssh scc-i01```
    * Go to a folder of your choosing and clone repository into it
    * Create a singularity .def file (say openfhe-sandbox.def) and copy into it the following
    ```
    Bootstrap: docker
    from: ubuntu:latest
    %files
            <path to source repo code> <name to give code in container>
    %post
            apt-get -y  update
            apt-get -y install wget gcc g++ make cmake git vim
            apt-get -y clean
            mkdir -p usr4
    %runscript
            /bin/bash
    ```
    * Run ```singularity build --fakeroot --sandbox openfhe-sandbox-container/ openfhe-sandbox.def ``` to build container
    * Run ```singularity shell --writable openfhe-sandbox-container/``` to open container shell

## To build project and run code

Follow the [docs](https://openfhe-development.readthedocs.io/en/latest/sphinx_rsts/intro/installation/installation.html)


## Misc. Docs



