# Calibration script

This repository contains the python code required to analyse the calibration data from the PXD PS calibration set up.

### Input:
Needed are the **.dat** files from the calibration run (23 x 3 files). The only thing needed to generate the calibration 
plots and the calibration constants is the path to the folder containing the data files.
This path needs to be given in the **path.ini** file in the following format:
`````python
data_path = /Users/resi/Desktop/BA_scibo/Calibrations/ps87/1_Calibration_ps87
`````
NOTE: When working under Windows the slashes need to be backwards
The **main.py** file does not need any changes.