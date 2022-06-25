import numpy as np
import configparser
import csv

config_old = configparser.ConfigParser()
config_new = configparser.ConfigParser()

def gain_offset(name_gain, name_offset,channel):
    gain_old = float(config_old[f'{channel}'].get(name_gain))
    gain_new = float(config_new[f'{channel}'].get(name_gain))
    gain_diff = round((np.abs(gain_old - gain_new) / gain_new) * 100,2)
    writer.writerow({'Channel': channel, 'Constants': name_gain, 'old': gain_old, 'new': gain_new,'difference': gain_diff, 'in': '%'})

    offset_old = float(config_old[f'{channel}'].get(name_offset))
    offset_new = float(config_new[f'{channel}'].get(name_offset))
    offset_diff = (np.abs(offset_old - offset_new))
    writer.writerow({'Channel': channel, 'Constants': name_offset, 'old': offset_old,'new': offset_new, 'difference': offset_diff, 'in': 'abs'})


config_old.read("/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/CalibrationData/1_Calibration_ohneOVP_ps87/constants_node_56.ini")
config_new.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/constants.ini")



with open('comparison.csv', 'w',encoding='UTF8') as csvfile:
    fieldnames = ['Channel', 'Constants', 'old', 'new', 'difference','in']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()
    for channel in range(24):

        gain_offset(f'DAC_VOLTAGE_GAIN', f'DAC_VOLTAGE_OFFSET', f'{channel}')
        gain_offset(f'ADC_U_LOAD_GAIN', f'ADC_U_LOAD_OFFSET', f'{channel}')
        gain_offset(f'ADC_U_REGULATOR_GAIN', f'ADC_U_REGULATOR_OFFSET', f'{channel}')
        gain_offset(f'ADC_I_MON_GAIN', f'ADC_I_MON_OFFSET', f'{channel}')
        gain_offset(f'DAC_CURRENT_GAIN', f'DAC_CURRENT_OFFSET', f'{channel}')



