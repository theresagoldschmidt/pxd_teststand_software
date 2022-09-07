import configparser

config = configparser.ConfigParser()
config_ini = configparser.ConfigParser()

config_ini.read("/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps87/1_Calibration_ps87/constants.ini")


def gain_offset(name_gain, name_offset,channel):
    gain = float(config_ini[f'{channel}'].get(name_gain))


def main():
    pass

if __name__ == '__main__':
    main()