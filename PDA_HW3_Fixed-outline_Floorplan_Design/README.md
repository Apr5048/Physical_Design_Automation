# PDA_HW2_Fixed-outline_Floorplan_Design
--How to Compile
In this directory, enter the following command:
$ make
It will generate the exceutable file "hw3" in "HW3/bin/".

If you want to remove it, please enter the following command:
$make clean

--How to Run
In this directory, enter the following command:
Usage: make test <testcase>
e.g.:
$ make test 1

In "HW3/bin/", enter the following command:
Usage: ./<exe> <hardblocks file> <nets file> <pl file> <floorplan file> <dead spaace ratio>
e.g.:
./hw3 ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100.floorplan 0.1