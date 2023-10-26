OpenFHE - Open-Source Fully Homomorphic Encryption Library\
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

# EvalAdd in [simple-real-numbers-ICSG-test.cpp](https://github.com/bu-icsg/OpenFHE-ICSG/blob/eval-add-pke-tests/src/pke/examples/simple-real-numbers-ICSG-test.cpp)


Stack trace goes to [EvalAddCoreInPlace](https://github.com/bu-icsg/OpenFHE-ICSG/blob/cc2297656da1b14621e42b39a647d88f39273869/src/pke/lib/schemebase/base-leveledshe.cpp#L603)

Can print out most of stack trace by running ```/<name of repo in container>/build/bin/examples/pke/simple-real-numbers-ICSG-test``` in container

```
void LeveledSHEBase<Element>::EvalAddCoreInPlace(Ciphertext<Element>& ciphertext1,
                                                 ConstCiphertext<Element> ciphertext2) const {
    std::cout << "In EvalAddCoreInPlace in " << __FILE__ << std::endl;
    std::vector<Element>& cv1       = ciphertext1->GetElements();
    const std::vector<Element>& cv2 = ciphertext2->GetElements();

    size_t c1Size     = cv1.size();
    size_t c2Size     = cv2.size();
    size_t cSmallSize = std::min(c1Size, c2Size);

    for (size_t i = 0; i < cSmallSize; i++) {
        cv1[i] += cv2[i];
    }

    if (c1Size < c2Size) {
        cv1.reserve(c2Size);
        for (size_t i = c1Size; i < c2Size; i++) {
            cv1.emplace_back(cv2[i]);
        }
    }
}
```





