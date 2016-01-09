# irc-boost
  
## Requirement  
  
You will need [boost-1.60](http://www.boost.org/users/history/version_1_60_0.html) and C++11 to compile it.

## Usage

To build it, you can type in :
```sh
make
```
You may change the compiler on the makefile (using the CXX var)
```sh
make CXX=g++ 		        # to use the GCC compiler
make CXX=clang++ 	      # default compiler
```

The executable name is ``` pudding ```, without arguments
```sh
./pudding
```

You also can clear the executable and the build folder.
```sh
make clean
```
