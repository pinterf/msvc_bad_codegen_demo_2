#include <iostream>
#include <stdint.h>

/*
  This project was created for reporting an issue on a VS2022 Microsoft C++ compiler code generation bug.

  Microsoft Visual Studio Community 2022 (64 - bit)
  Version 17.12.3

  Platform toolset v143
  x64 - Release

  Optimizations: Maximum Optimization (Favor Speed) (/O2)
  Inline function expansion: Any Suitable (/Ob2)  -- *** if this is changed to "Only __inline (/Ob1)", then the bug does not occur ***
  Enable intrinsic functions: Yes (/Oi)
  Favor size of speed: Favor fast code (/Ot)
  Omit frame pointers: nothing
  Enable Fiber safe optimizations: No
  Whole program optimization: No

  When the compiler encounters with
  
  if ((x < 0) == (y > comp))
  
  instead of
  
  if ((x < 0 && y > comp) || (x >= 0 && y <= comp))
  
  then it genarates bad code if Inline function expansion is set to "Any Suitable".
  When "signed_saturated_add64" is not inlined then the bug does not occur.

*/

// msvc 17.12.3 generates bad code on this
static int64_t signed_saturated_add64_bad_msvc(int64_t x, int64_t y) {
  // determine the lower or upper bound of the result
  int64_t ret = (x < 0) ? INT64_MIN : INT64_MAX;
  // this is always well defined:
  // if x < 0 this adds a positive value to INT64_MIN
  // if x > 0 this subtracts a positive value from INT64_MAX
  int64_t comp = ret - x;

  // the condition below is equivalent to
  // ((x < 0) && (y > comp)) || ((x >=0) && (y <= comp)) ret = x + y;
  if ((x < 0) == (y > comp)) ret = x + y; // for speed reasons we use this one instead
  return ret;
}

static int64_t signed_saturated_add64(int64_t x, int64_t y) {
  // determine the lower or upper bound of the result
  int64_t ret = (x < 0) ? INT64_MIN : INT64_MAX;
  // this is always well defined:
  // if x < 0 this adds a positive value to INT64_MIN
  // if x > 0 this subtracts a positive value from INT64_MAX
  int64_t comp = ret - x;

  // the condition below is equivalent to
  // ((x < 0) && (y > comp)) || ((x >=0) && (y <= comp)) ret = x + y;
  if ((x < 0 && y > comp) || (x >= 0 && y <= comp)) ret = x + y; // use slower version which does not have compilation bug
  // if ((x < 0) == (y > comp)) ret = x + y; // for speed reasons we use this one instead

  return ret;
}

int main()
{
    // some magic, I have the original code super-minimized until the bug exists.
    const int BUFSIZE = 1;
    int16_t buf[BUFSIZE];
    int16_t buf2[BUFSIZE];

    for (auto i = 0; i < BUFSIZE; i++) {
      buf[i] = -1; // compiler bug occurs only when buffer content is negative!
      buf2[i] = 1;
    }

    int i = 0;

    // expected result is -1 + 1 = 0
    int64_t a = signed_saturated_add64_bad_msvc((int64_t)buf[i] * buf2[i], 1);
    int64_t b = signed_saturated_add64((int64_t)buf[i] * buf2[i], 1);

    if (a != b)
      std::cout << "Compiler bad code generation. wrong: " << a << " good: " << b << std::endl;
    else
      std::cout << "Compiler O.K." << std::endl;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
