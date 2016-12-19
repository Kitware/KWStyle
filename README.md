KWStyle
=======

[![Circle CI]( https://circleci.com/gh/Kitware/KWStyle.svg?style=svg)](https://circleci.com/gh/Kitware/KWStyle )

Overview
--------

KWStyle is a style checker for source code which is integrated in the software process
to ensure that the code written by several users is consistent and can be
viewed/printed as it was written by one person.

Features
--------

Among the fearures provided by KWStyle:

- Several Indentation checking
- Copyright Header correctness
- Maximum line length
- Encapsulation preservation
- Internal variable checking via regular expressions
- New line at the end of file

A complete list of the features is available [here][fl].

Build
-----

```shell
mkdir build
cd build
cmake ..
make
```

Install
-------

- Build and `make install` or `checkinstall`
- On at least Debian and Trisquel, `apt-get install kwstyle`


[fl]: https://kitware.github.io/KWStyle/resources/features.html
