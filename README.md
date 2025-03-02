# UtilityLib

Writing some utility stuff so that I can use them on my other C++ projects in the future.

Provides following utilities:
1. String utility functions
2. File IO utility functions
3. Winsock2 wrapper classes for multiple use cases
4. Hopefully more things to add as I find new ideas

- All of the following utilities above uses the Error enumerations provided by this library itself "ErrorPkg.h/.cpp" , "UtilityLib::Error"

- Currently I directly use Visual Studio makefiles (.sln .vcxproj) because I don't like to write CMake. Writing that is awful but eventually I will move to CMake so that someone can select which utilites to use which utilities not to use etc. This is necessary since Winsock2 is Windows only or someone might not want some of the utilities the library provides or someone might hate Visual Studio (which I don't get at all, it is pretty good imo but whatever)

- Also, I have plans to add tests for the code i wrote. Currently i randomly try to see if any error exist by changing main and trying new implementations, which is a pretty awful method and I'm pretty sure my code contains a lot of bugs right now. I will add tests when I feel like I can drag myself to that grind (gaming joke).

- Eventually I will add a license to be cool, I don't think this project will go anywhere but if any soul out ther finds this repository randomly, do whatever you want with this code until I add a license

- Also, you need C++20 because while doing this project I try to learn new features of the standards as I come across for their use cases. There is no planned learning strategy, just vibing around. Currently I used some concepts in there. Yayyy
