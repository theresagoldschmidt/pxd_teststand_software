import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import configparser
import os
config_old = configparser.ConfigParser()
config_new = configparser.ConfigParser()

for channel in range(1):
    config_old.read("/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/CalibrationData/1_Calibration_ohneOVP_ps87/constants_node_56.ini")
    config_new.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/constants.ini")

    print(config_old[f'{channel}'].get('DAC_VOLTAGE_GAIN'))
    print(config_new[f'{channel}'].get('DAC_VOLTAGE_GAIN'))