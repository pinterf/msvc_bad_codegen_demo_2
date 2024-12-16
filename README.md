# MSVC 2022 Compiler Bad Code Generation Issue

## Bug Demonstration

1. Open the solution with Visual Studio 2022.
2. Choose the x64 Release configuration.
3. Run the project.

### Possible Console Printouts

- Compiler bad code generation. Wrong: -9223372036854775808 Good: 0
- Compiler O.K.

The project optimization settings are configured to demonstrate the bug.

### Steps to have the bug disappear:

1. Set the Optimization section Inline function expansion from "Any Suitable (/Ob2)" to "Only __inline (/Ob1)".

This issue has been demonstrated on MSVC version 17.12.3, but it has been present for several months.

For more information, see the comments in the main.cpp file.
