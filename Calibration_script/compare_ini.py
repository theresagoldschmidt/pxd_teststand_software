import numpy as np
import matplotlib.pyplot as plt
import configparser
import pandas as pd
import csv

config_old = configparser.ConfigParser()
config_new = configparser.ConfigParser()

x = []

def gain_offset(name_gain, name_offset,channel):
    gain_old = float(config_old[f'{channel}'].get(name_gain))
    gain_new = float(config_new[f'{channel}'].get(name_gain))
    gain_diff = round((np.abs(gain_old - gain_new) / gain_new) * 100, 2)
    writer.writerow({'Channel': channel, 'Constants': name_gain, 'old': gain_old, 'new': gain_new,'difference': gain_diff, 'in': '%'})

    offset_old = float(config_old[f'{channel}'].get(name_offset))
    offset_new = float(config_new[f'{channel}'].get(name_offset))
    offset_diff = offset_old - offset_new
    writer.writerow({'Channel': channel, 'Constants': name_offset, 'old': offset_old,'new': offset_new, 'difference': offset_diff, 'in': 'abs'})


config_old.read("/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/CalibrationData/1_Calibration_ohneOVP_ps87/constants_node_56.ini")
config_new.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/constants.ini")



with open('comparison.csv', 'w',encoding='UTF8') as csvfile:
    fieldnames = ['Channel', 'Constants', 'old', 'new', 'difference','in']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()
    for channel in range(24):

        gain_offset(f'DAC_VOLTAGE_GAIN', f'DAC_VOLTAGE_OFFSET', channel)
        gain_offset(f'ADC_U_LOAD_GAIN', f'ADC_U_LOAD_OFFSET', channel)
        gain_offset(f'ADC_U_REGULATOR_GAIN', f'ADC_U_REGULATOR_OFFSET', channel)
        gain_offset(f'ADC_I_MON_GAIN', f'ADC_I_MON_OFFSET', channel)
        gain_offset(f'DAC_CURRENT_GAIN', f'DAC_CURRENT_OFFSET', channel)

    comp = pd.read_csv('comparison.csv', sep=",", header=0)
    comp.columns = ['Channel', 'Constants', 'old', 'new', 'difference','in']



    difference = comp['difference']
    voltage_gain = difference[::10]
    load_gain = difference[2::10]
    regulator_gain = difference[4::10]
    mon_gain = difference[6::10]
    current_gain = difference[8::10]


    channel_1 = list(comp['Channel']-0.2)
    channel_2 = list(comp['Channel']-0.1)
    channel_3 = list(comp['Channel'])
    channel_4 = list(comp['Channel'] + 0.1)
    channel_5 = list(comp['Channel'] + 0.2)
    channel_short_1 = channel_1[0::10]
    channel_short_2 = channel_2[2::10]
    channel_short_3 = channel_3[4::10]
    channel_short_4 = channel_4[6::10]
    channel_short_5 = channel_5[8::10]


    #for i in range(5):
    #    channel_i = list(comp['Channel']-0.2+(i*0.1))
    #    channel_short_i = channel_i[i*2::10]
    #    plt.bar(channel_short_i, voltage_gain, color='b', width=0.1)

    plt.bar(channel_short_1, voltage_gain, color='b', width=0.1)
    plt.bar(channel_short_2, load_gain, color = 'r', width = 0.1)
    plt.bar(channel_short_3, regulator_gain, color='y', width=0.1)
    plt.bar(channel_short_4, mon_gain, color='g', width=0.1)
    plt.bar(channel_short_5, current_gain, color='pink', width=0.1)
    #plt.grid()
    plt.xticks(np.arange(0,24,1))
    plt.show()
    plt.close()

