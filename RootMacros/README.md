## The ROOT script

### This folder contains all root files needed to generate the calibration constants and curves from the calibration data

> This scipts was written by Stephan Rummel fot the calibration of the PXD Powersupply. 



- After the calibration is done, there will be measurement files for all the 24 channels, which are located here:

```
path="C:/Users/LMUPS/Documents/GitHub/pxd_teststand_software/LightCal_build/target/Debug/";
```
> To generate the calibration constants and curves the entire RootMacros folder hast to be downloaded and root needs to be installed.
     
- To analyse the given data with the currently used root script, one has to open the "eval_wite_ini.C" and adjust the path, to where the files from the calibration run lay. The paths in line 59, 404 and 437 have to be set manually.

- Then open root and execute the script by giving the following command:

```
  .x eval_white_ini.C 
```
- This script will generate a file "constants_node_" where the generation constants are saved. Also the curves for each of the 24 channels will be opened and automaticly saved to the folder set in the "eval_wite_ini.C" file.

- I found it useful to generate a folder with the following naming convention: "x_Calibration_psX", wherex stands for the number of the run, and X fot the number of the PS.
In this folder I included a sub folder for the plots and used the following path in the .C file mentioned bevor to save the plots:

```
pathname = Form("C:/Users/LMUPS/Documents/GitHub/pxd_teststand_software/LightCal_build/target/Debug/4_Calibration_ps15/plots/Channel%d.pdf", i);
```  
  
  
  
  
  
