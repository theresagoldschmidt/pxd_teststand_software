# Calibration script

This repository contains the python code required to analyse the calibration data from the PXD PS calibration set up.
* **
## main.py
### Input:
Needed are the **.dat** files from the calibration run (23 x 3 files). The only thing needed to generate the calibration 
plots and the calibration constants is the path to the folder containing the data files.
This path needs to be given in the **path.ini** file in the following format:
`````python
data_path = "/Users/resi/Desktop/BA_scibo/Calibrations/ps87/1_Calibration_ps87"
`````
NOTE: When working under Windows the slashes need to be backwards.
The **main.py** file does not need any changes.

### Output:
All output files will be saved in the folder of the calibration data.
* Pass or fail of the calibration run 
* calibration constants in **constants.ini**
* Errors of calibration constants in **constants_err.ini**
* Calibration curves in plots folder 
* Histogram visualising deleted points **deleted_points.pdf**

* **
## get_range.py
Script to adjust the constants_range.ini. The constant_range.ini contains an upper and 
lower limit for each of the constants. This script compares calibration
constants from a specific ini to the constants_range.ini. If the constant in the given
ini is above or below the range, the top or bottom limit will be changed
accordingly. This is a useful script to get an accurate range for the pass/fail
evaluation, however one must be sure, that the calibration from which the constants
are taken, was successful.
### Input: 
Change in code. Path to the constants_range.ini:
``````python
config_range.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/constants_range.ini")
``````
Path to the constants.ini file for comparison :
``````python
config_ini.read("/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps26/1_Calibration_ps26/constants.ini")
``````

### Output:
Direct change in the constants_range.ini.

* **
## collecting_constants.py
This script collects the constants from different calibrations and writes them
into a csv file. To properly function the calibration runs have to be named
in the following manner: "%i_Calibration_psxx" where i is the number of
the column that the constants will be in the table and xx the number of the PS.
### Input: 
To collect the constants and the errors of the constants, the script needs
to run twice after performing some changes.
First to run the script for collecting the constants the path to the
folder containing the folders of the different runs needs to be given:
``````python
path = "/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps26/"
``````
Furthermore,to collect eather the constants or the errors these lines need
to be commeted in/out accordingly:
```````python
names = ["'Date'"] #for constants
#names = [] #for err

result = os.path.join(path, "%d_Calibration_ps26" % i + "/constants.ini") #for constants
#result = os.path.join(path, "%d_Calibration_ps26" % i + "/constants_err.ini") #for errors

date = str(config_ini[f'Information'].get(f'date')) #for constants
values = [date] #for constants
#values = [] #for errors

pd.read_csv('constants_collection.csv', header=None).T.to_csv(path + 'output.csv', header=False, index=False)
#pd.read_csv('constants_err_collection.csv', header=None).T.to_csv(path + 'output_err.csv', header=False, index=False)
```````
### Output:
The output are two csv files, one containing a table with the calibration 
constants and one containing a table with the errors of the constants.
* **
## constants_stability.py
This script visualizes the stability of the calibration constants. It was written
to plot data collected via the collecting constants script.
### Input:
### Output:

* **
## compare_ini.py

### Input:
### Output:

