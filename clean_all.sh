#!/bin/bash

# Clean script for BGU System Programming Labs
# This script removes all object files, executables, and temporary files

echo "🧹 Cleaning BGU System Programming Labs..."

# Count files before cleaning
echo "📊 Counting files to be cleaned..."
OBJECT_FILES=$(find . -name "*.o" -type f | wc -l)
ZONE_FILES=$(find . -name "*:Zone.Identifier" -type f | wc -l)
A_OUT_FILES=$(find . -name "a.out" -type f | wc -l)

echo "Found:"
echo "  - Object files (.o): $OBJECT_FILES"
echo "  - Zone Identifier files: $ZONE_FILES"
echo "  - a.out files: $A_OUT_FILES"

# Function to safely remove files
safe_remove() {
    local pattern="$1"
    local description="$2"
    
    echo "🗑️  Removing $description..."
    find . -name "$pattern" -type f -exec rm -f {} \;
}

# Remove object files
safe_remove "*.o" "object files (.o)"

# Remove Zone.Identifier files (Windows metadata)
safe_remove "*:Zone.Identifier" "Zone Identifier files"

# Remove a.out files
safe_remove "a.out" "a.out files"

# Remove specific executables (preserve source files)
echo "🗑️  Removing executables..."

# Lab-specific executables
rm -f lab0/bin/main
rm -f lab1/bin/cw
rm -f lab2/myshell lab2/looper
rm -f lab3/start lab3/task1
rm -f lab4/task4 lab4/deep_thought lab4/offensive lab4/ntsc lab4/abc
rm -f lab4/Task4/task4
rm -f lab4/units/units
rm -f lab5/loader lab5/task1 lab5/workingLoader
rm -f LabA/encoder
rm -f LabB/AntiVirus lab B/part0 LabB/infected
rm -f labE/myELF labE/task0
rm -f task3/menu
rm -f labc/MYPIPELINE/myshell/myshell labc/MYPIPELINE/myshell/looper
rm -f labc/MYPIPELINE/mypipeline/mypipeline

# Remove backup files
safe_remove "*~" "backup files (~)"
safe_remove "*.swp" "Vim swap files"
safe_remove "*.swo" "Vim swap files"

# Remove core dumps
safe_remove "core" "core dump files"
safe_remove "core.*" "core dump files"

# Remove temporary files
safe_remove ".DS_Store" "macOS metadata files"
safe_remove "Thumbs.db" "Windows thumbnail cache"

echo "✅ Cleanup completed!"
echo ""
echo "📁 To rebuild any lab, navigate to its directory and run:"
echo "   make"
echo ""
echo "🔄 To clean individual labs, use 'make clean' in each lab directory"
