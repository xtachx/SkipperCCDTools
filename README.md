# SkipperCCDTools

Tools for fast analysis of the skipper CCD images. The code computes an

1. Average image
2. Standard deviation image

Across a specified number of NDCM (skips).

# Usage

## cenpa-rocks

The default CMake and gcc versions on cenpa-rocks are quite old, and you will need to load newer versions to run this code. You can do this by:

```
module load cmake-3.15.4
module load gcc-9.1.0
```

When you login. You can also add this to your `.bashrc` file.

## Build

Most simply, in the cloned repository run:

```
cmake .
make
```

To build the `DAMICMRecon` executable.

## Execution

The executable takes a number of command line arguments. You can run `./DAMICMRecon --help` for a detailed explanation of the variables at the command prompt. The possible inputs are:

* `-f, --file` - Required. A single, or list, of file(s) that you want to be processed.
* `-r, --range` - Optional. The range of NDCMs you want considered in the computations. The default values are [0, -1] which corresponds to all the skips.
* `-o, --output` - Optional. The output directory to store the processed files. The default is the current directory.

Each file in the list that is passed to the program gets proccessed, creating an output file with the name `<input-filename>_<start_skip>_to_<end_skip>_<operation>.fits` in the ouput directory.