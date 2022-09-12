import configparser
import csv
import os
import pandas as pd

config = configparser.ConfigParser()
config_ini = configparser.ConfigParser()

#config_ini.read("/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps87/1_Calibration_ps87/constants.ini")


def gain_offset(name_gain, name_offset,channel):
    gain = float(config_ini[f'{channel}'].get(name_gain))
    offset = float(config_ini[f'{channel}'].get(name_offset))
    return gain,offset

def main():
    path = "/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps87/"
    with open('constants_collection.csv', 'w+', encoding='UTF8') as csvfile:

            names = ["'Date'"]
            for i in range(24):
                names.append(i)
                names.append("'DAC_VOLTAGE_GAIN_%i'" % i)
                names.append("'DAC_VOLTAGE_OFFSET_%i'" % i)
                names.append("'ADC_U_LOAD_GAIN_%i'" % i)
                names.append("'ADC_U_LOAD_OFFSET_%i'" % i)
                names.append("'ADC_U_REGULATOR_GAIN_%i'" % i)
                names.append("'ADC_U_REGULATOR_OFFSET_%i'" % i)
                names.append("'ADC_I_MON_GAIN_%i'" % i)
                names.append("'ADC_I_MON_OFFSET_%i'" % i)
                names.append("'DAC_CURRENT_GAIN_%i'" % i)
                names.append("'DAC_CURRENT_OFFSET_%i'" % i)

            writer = csv.DictWriter(csvfile, fieldnames=names)
            writer.writeheader()

            for i in range(1, 21):
                if i == 10:
                    pass
                else:
                    result = os.path.join(path, "%d_Calibration_ps87" % i + "/constants.ini")
                    config_ini.read(result)
                    date = str(config_ini[f'Information'].get(f'date'))
                    values = [date]
                    for channel in range(24):
                        (g1, o1) = gain_offset(f'DAC_VOLTAGE_GAIN', f'DAC_VOLTAGE_OFFSET', channel)
                        (g2, o2) = gain_offset(f'ADC_U_LOAD_GAIN', f'ADC_U_LOAD_OFFSET', channel)
                        (g3, o3) = gain_offset(f'ADC_U_REGULATOR_GAIN', f'ADC_U_REGULATOR_OFFSET', channel)
                        (g4, o4) = gain_offset(f'ADC_I_MON_GAIN', f'ADC_I_MON_OFFSET', channel)
                        (g5, o5) = gain_offset(f'DAC_CURRENT_GAIN', f'DAC_CURRENT_OFFSET', channel)
                        for i in [channel, g1,o1,g2,o2,g3,o3,g4,o4,g5,o5]:
                            values.append(i)


                    dictonary ={names[i]: values[i] for i in range(len(names))}
                    writer.writerow(dictonary)

    csvfile.close()
    pd.read_csv('constants_collection.csv', header=None).T.to_csv(path + 'output.csv', header=False, index=False)


if __name__ == '__main__':
    main()