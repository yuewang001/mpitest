#!/bin/bash

#echo $1 $2 
taskset -c $1 ./emt1 $1 40000000 600000
