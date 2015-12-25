#!/usr/bin/env python

from setuptools import setup

setup(name="romeo",
      version="0.1",
      description=("Add a description"),
      author="erik",
      author_email='simba@simba.com',
      packages=["romeo"],
      package_data={"": ["doc/*.rst",
                          "doc/conf.py",
                          "doc/Makefile",
                          "doc/doxygen.cfg",
                          "doc/romeo/*.rst",
                          "src/romeo.h",
                          "src/romeo.mk", 
                          "src/romeo/*.h",
                          "src/*.c",
                          "tst/*/*"]},
      license='MIT',
      classifiers=[
          'License :: OSI Approved :: MIT License',
      ],
      url='https://github.com/eerimoq/simba')
