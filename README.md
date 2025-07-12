# System Programming Labs - BGU Course

This repository contains a collection of system programming labs completed as part of a Computer Science course at Ben-Gurion University (BGU). The labs cover fundamental concepts in C programming, system calls, memory management, process control, and low-level system interactions.

## Course Overview

These labs progressively build knowledge in:
- C programming fundamentals
- Assembly language integration
- Process management and IPC
- Shell implementation
- Virus/malware analysis and detection
- ELF file format analysis
- Memory management and loaders
- System call implementation

## Lab Structure

### Lab 0 - Basic C Programming & Assembly Integration
**Directory:** `lab0/`
- **Focus:** Introduction to C programming with assembly language integration
- **Key Files:**
  - `src/main.c` - Main program with command-line argument parsing
  - `src/add.s` - Assembly implementation of addition function
  - `src/numbers.c` - Number input handling
- **Features:** Command-line options (`-o` for output file, `-v` for verbose mode)

### Lab 1 - String Manipulation & Menu Systems
**Directory:** `lab1/`
- **Focus:** String processing, pointer manipulation, and menu-driven programs
- **Key Files:**
  - `src/count-words.c` - Word counting functionality
  - `src/menu.c` - Interactive menu system
  - `src/addresses.c` - Memory address demonstrations
  - `src/pointers.c` - Pointer manipulation exercises
- **Features:** Dynamic string handling, pointer arithmetic

### Lab 2 - Shell Implementation & Process Management
**Directory:** `lab2/`
- **Focus:** Custom shell implementation with process control and piping
- **Key Files:**
  - `myshell.c` - Complete shell implementation
  - `LineParser.c/h` - Command line parsing utilities
  - `myPipe.c` - Pipe implementation
  - `myPipeLine.c` - Pipeline management
- **Features:**
  - Process management (background/foreground)
  - Command history (20 commands)
  - Signal handling
  - Process status tracking (RUNNING/SUSPENDED/TERMINATED)
  - Input/output redirection

### Lab 3 - System Calls & Virus Simulation
**Directory:** `lab3/`
- **Focus:** Low-level system programming and virus-like behavior simulation
- **Key Files:**
  - `main.c` - Main virus simulation program
  - `util.c/h` - System call utilities
  - `start.s` - Assembly startup code
- **Features:**
  - Direct system call implementation
  - File infection simulation
  - Assembly integration
  - Educational virus behavior (non-malicious)

### Lab 4 - Reverse Engineering & Binary Analysis
**Directory:** `lab4/`
- **Focus:** Binary analysis, reverse engineering, and exploit development
- **Key Files:**
  - `Task4/task4.c` - String analysis program
  - `hexeditplus.c` - Hex editor implementation
  - Various binary files for analysis
- **Features:**
  - Digit counting in strings
  - Binary file manipulation
  - Reverse engineering exercises

### Lab 5 - ELF Loader Implementation
**Directory:** `lab5/`
- **Focus:** ELF file format understanding and custom loader implementation
- **Key Files:**
  - `loader.c` - ELF file loader implementation
  - `start.s` - Custom startup assembly code
  - Various ELF object files for testing
- **Features:**
  - ELF header parsing
  - Program header analysis
  - Memory mapping implementation
  - Dynamic loading capabilities

### Lab A - Encoding/Decoding Systems
**Directory:** `LabA/`
- **Focus:** Cryptographic encoding and file processing
- **Key Files:**
  - `encoder.c` - Encoding/decoding implementation
- **Features:**
  - Character encoding algorithms
  - File I/O operations
  - Debug mode support

### Lab B - Antivirus Implementation
**Directory:** `LabB/`
- **Focus:** Virus detection and signature-based scanning
- **Key Files:**
  - `AntiVirus.c` - Antivirus engine implementation
  - Signature files for virus detection
- **Features:**
  - Virus signature loading
  - File scanning capabilities
  - Big-endian/little-endian conversion
  - Linked list management for virus database

### Lab C & E - Advanced ELF Analysis
**Directories:** `labC/`, `labE/`
- **Focus:** Advanced ELF file manipulation and analysis
- **Key Files:**
  - `labE/myELF.c` - Comprehensive ELF analyzer
- **Features:**
  - Section header analysis
  - Symbol table parsing
  - Multi-file ELF comparison
  - Debug mode functionality

## Building and Running

Most labs include Makefiles for easy compilation:

```bash
# Navigate to any lab directory
cd lab[X]/

# Compile the project
make

# Run the executable (varies by lab)
./[executable_name]
```

## Cleaning Build Artifacts

### Clean All Labs at Once
To remove all object files, executables, and temporary files from all labs:

```bash
# From the root labs directory
./clean_all.sh
```

This script removes:
- Object files (`.o`)
- Executables (lab-specific binaries)
- Zone Identifier files (Windows metadata)
- Backup files (`*~`, `.swp`, `.swo`)
- Core dumps
- Temporary system files

### Clean Individual Labs
Most labs support individual cleaning:

```bash
# Navigate to specific lab directory
cd lab[X]/

# Clean build artifacts
make clean
```

### Manual Cleanup
For manual cleanup of specific file types:

```bash
# Remove all object files
find . -name "*.o" -type f -delete

# Remove Zone Identifier files (Windows metadata)
find . -name "*:Zone.Identifier" -type f -delete

# Remove backup files
find . -name "*~" -type f -delete
```

## Prerequisites

- GCC compiler with support for:
  - C99 standard
  - Assembly language (NASM/GAS)
  - 32-bit compilation capabilities
- Linux environment (tested on standard distributions)
- Standard development tools (make, ld, etc.)

## Educational Value

These labs provide hands-on experience with:
- **System Programming:** Direct interaction with OS primitives
- **Memory Management:** Manual memory allocation and mapping
- **Process Control:** Creating and managing processes
- **File Systems:** Low-level file operations and format parsing
- **Security Concepts:** Understanding malware and protection mechanisms
- **Assembly Integration:** Mixing C and assembly code
- **Debugging Skills:** Using various debugging and analysis tools

## Important Notes

- **Educational Purpose:** These labs are designed for learning system programming concepts
- **Simulated Threats:** Any "virus" or "malware" components are educational simulations only
- **Platform Specific:** Code is primarily designed for Linux/Unix systems
- **Academic Context:** Part of BGU Computer Science curriculum

## Repository Structure

```
labs/
├── README.md                 # This file
├── lab0/                    # Basic C & Assembly
├── lab1/                    # String Manipulation
├── lab2/                    # Shell Implementation
├── lab3/                    # System Calls & Virus Sim
├── lab4/                    # Reverse Engineering
├── lab5/                    # ELF Loader
├── LabA/                    # Encoding Systems
├── LabB/                    # Antivirus Implementation
├── labC/                    # Advanced Topics
├── labE/                    # ELF Analysis
└── task3/                   # Additional Exercises
```

## Contact

This repository represents coursework completed at Ben-Gurion University. For academic inquiries, please refer to the course materials and instructors.

---

*Note: This code is provided for educational purposes and reference. Please respect academic integrity policies when using this material.*

