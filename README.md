OpenFHE - Open-Source Fully Homomorphic Encryption Library\
Copy for ICSG testing purposes
=====================================

## To build project in SCC using Singularity Container

Follow the following steps:
  * Go to [SCC](https://scc-ondemand1.bu.edu/pun/sys/dashboard) and go to a login node
  * ssh into scc-i02 using ```ssh scc-i02``` (As of 2024, scc-i01 does not seem to work)
  * Go to scratch folder using ```cd /scratch``` and make a folder in it
  * Create a singularity .def file (say openfhe-sandbox.def) in that folder and copy into it the following
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
	  git clone -b eval-add-pke-tests <Repo URL>
  %runscript
          /bin/bash
  ```
  * Run ```singularity build --fakeroot --sandbox openfhe-sandbox-container/ openfhe-sandbox.def ``` to build container
  * Move/Copy the sandbox container to another folder you want to use it in (Say projectnb/he/<your name>) and delete the folder you made in scratch
  * Run ```singularity shell --writable openfhe-sandbox-container/``` to open container shell

## To build project and run code

Follow the [docs](https://openfhe-development.readthedocs.io/en/latest/sphinx_rsts/intro/installation/installation.html)

To get the ciphertext limbs in files, run the command ```./getFiles.sh```. The ciphertext input and output code is in [simple-real-numbers-evalout.cpp](https://github.com/bu-icsg/OpenFHE-ICSG/blob/eval-add-pke-tests/src/pke/examples/simple-real-numbers-evalout.cpp). The python script is located in the same directory as ```getFiles.sh``` and is called ```numtofile.py```.

## Misc. Docs

Official discussion group for OpenFHE - [OpenFHE Discourse](https://openfhe.discourse.group/)

### Params Information

Located in [cryptocontextparams-base.h](https://github.com/bu-icsg/OpenFHE-ICSG/blob/eval-add-pke-tests/src/pke/include/scheme/cryptocontextparams-base.h). Use with a CCParams<> variable to get and set params.

After creating CryptoContext, can view param info by using the getters in [elemparams.h](https://github.com/bu-icsg/OpenFHE-ICSG/blob/eval-add-pke-tests/src/core/include/lattice/elemparams.h)

### Getting ciphertext limbs as text files (in hex)

Run ```getFiles.sh``` (located in the main directory). This will create a folder called ```temp``` which will have the required files. Use ```./getFiles.sh remove``` to clean up.

### EvalAdd in [simple-real-numbers-ICSG-test.cpp](https://github.com/bu-icsg/OpenFHE-ICSG/blob/eval-add-pke-tests/src/pke/examples/simple-real-numbers-ICSG-test.cpp)


Stack trace goes to [EvalAddCoreInPlace](https://github.com/bu-icsg/OpenFHE-ICSG/blob/e780c5b9d408daddce1bdd9cfcfb9468760d4f33/src/pke/lib/schemebase/base-leveledshe.cpp#L603)

Can print out most of stack trace by running ```/<name of repo in container>/build/bin/examples/pke/simple-real-numbers-ICSG-test``` in container shell.

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
![EvalAdd](https://github.com/bu-icsg/OpenFHE-ICSG/assets/84148847/452309a1-a340-41cc-80b1-8e9e4366e114)

### EvalMult for vector x vector in [simple-real-numbers-ICSG-test.cpp](https://github.com/bu-icsg/OpenFHE-ICSG/blob/eval-add-pke-tests/src/pke/examples/simple-real-numbers-ICSG-test.cpp)

Stack trace goes to [EvalMultCore](https://github.com/bu-icsg/OpenFHE-ICSG/blob/e780c5b9d408daddce1bdd9cfcfb9468760d4f33/src/pke/lib/schemebase/base-leveledshe.cpp#L656)

```
Ciphertext<Element> LeveledSHEBase<Element>::EvalMultCore(ConstCiphertext<Element> ciphertext1,
                                                          ConstCiphertext<Element> ciphertext2) const {
    Ciphertext<Element> result = ciphertext1->CloneZero();

    std::vector<Element> cv1        = ciphertext1->GetElements();
    const std::vector<Element>& cv2 = ciphertext2->GetElements();

    size_t cResultSize = cv1.size() + cv2.size() - 1;
    std::vector<Element> cvMult(cResultSize);

    if (cv1.size() == 2 && cv2.size() == 2) {
        cvMult[2] = (cv1[1] * cv2[1]);
        cvMult[1] = (cv1[1] *= cv2[0]);
        cvMult[0] = (cv2[0] * cv1[0]);
        cvMult[1] += (cv1[0] *= cv2[1]);
    }
    else {
        std::vector<bool> isFirstAdd(cResultSize, true);

        for (size_t i = 0; i < cv1.size(); i++) {
            for (size_t j = 0; j < cv2.size(); j++) {
                if (isFirstAdd[i + j] == true) {
                    cvMult[i + j]     = cv1[i] * cv2[j];
                    isFirstAdd[i + j] = false;
                }
                else {
                    cvMult[i + j] += cv1[i] * cv2[j];
                }
            }
        }
    }

    result->SetElements(std::move(cvMult));
    result->SetNoiseScaleDeg(ciphertext1->GetNoiseScaleDeg() + ciphertext2->GetNoiseScaleDeg());
    result->SetScalingFactor(ciphertext1->GetScalingFactor() * ciphertext2->GetScalingFactor());
    const auto plainMod = ciphertext1->GetCryptoParameters()->GetPlaintextModulus();
    result->SetScalingFactorInt(
        ciphertext1->GetScalingFactorInt().ModMul(ciphertext2->GetScalingFactorInt(), plainMod));
    return result;
}
```
![EvalMult](https://github.com/bu-icsg/OpenFHE-ICSG/assets/84148847/1933beb4-48d6-4fcb-99f9-6fae9d193b43)


