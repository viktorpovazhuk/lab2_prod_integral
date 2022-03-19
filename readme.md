# Lab work 2: Paraller integral calculation
Authors (team): [Viktor Povazhuk](https://github.com/viktorpovazhuk), [Bohdan Mykhailiv](https://github.com/bmykhaylivvv)    
Variant: 1
## Prerequisites

To compile .cpp file you should have compiler (e.g. g++) and cmake for compiling project.

### Compilation

Project can be compiled just by running fr ```./compile.sh``` or ```./compile.sh -O``` for compilation with optimization from root directory of project.

### Installation

There is no need to install any additional dependencies as all modules, used in python script, is installed in Python standart library.

### Usage

**Step 1:** Run ```./compile.sh -O``` in your ROOT directory  
**Step 2:** Execute compile file in /bin directory by running ```./integrate_task_1 arg```  
- *arg* - path to configurations file  

Output:
e.g.<br />
```
1
4545447.66
0.023079142
5.07741893e-09
100878
```
where
```
1 -- variant
4545447.66 -- calculation result
0.023079142 -- absolute error
5.07741893e-09 -- relative error
100878 -- calculation time (microseconds)
```

OR

You can run python script which run calculations multiple number of times.  
**Step 1:** Run ```./compile.sh -O``` in your ROOT directory  
**Step 2:** Move to ```/scripts``` directory  
**Step 3:** ```python measure_script.py arg1 arg2```  
- *arg1* - number of times to repeat experiments  
- *arg2* - path to configurations file  

Output: the smallest time among experiment which were run.
e.g.<br />
```
53336340
```
### Important!

Note that scripts are path-sensetive.  
Configurations file you can find in ```/configs``` folder.

### Results
On the chart below you can see relation between number of threads and time of calculation.  
TO DO: Change this chart to real one
<img src="./assets/chart.png" alt="Chart image of relation between threads number and time of calculation" width="500"/>

We can see that see that calculation time decreases when we increase number of threads. It means that we implemented efficient method for parallel integral calculation.  
It's important to notice, that, for example, for 3 threads time of calculation is larger than for 2 threads, its due the fact that we get remainder when divide by 3 while calculation integral and it`s more diffilult to reach the desirable error.
