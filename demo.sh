#!/bin/bash

# Parse Universal Binary
echo "Parsing Universal Binary."
echo "Listing Universal Binary architectures using './bin/poc ~/jtool/jtool arches'"
./bin/poc ~/jtool/jtool arches
echo "-----------------------------------------------------------------------------"

# Split Universal Binary. This creates files split0 and split1
echo "Splitting Universal Binary using './bin/poc ~/jtool/jtool split'"
./bin/poc ~/jtool/jtool split
echo "-----------------------------------------------------------------------------"

# Signing simple MachO
echo "Signing a simple hello world C program and storing the output in out.bin"
# a. First create the MachO
echo -e '#include <stdio.h>\nint main(){printf("Hello World!\\n");return 0;}' > test.c
# b. Then compile it
gcc test.c -o test
# c. Actually sign it. This creates file out.bin
./bin/poc test sign
echo "-----------------------------------------------------------------------------"

# Validate the previously generated signature
echo "Validating signature attached to out.bin"
./bin/poc out.bin validate
echo "-----------------------------------------------------------------------------"

# Extract de previously generated signature
echo "Extracting the signature attached to out.bin and storing it in sigdump0"
./bin/poc out.bin dumpsig
echo "-----------------------------------------------------------------------------"

# Cleanup with: rm test test.c split1 split0 sigdump0 out.bin
