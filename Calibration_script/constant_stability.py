import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import csv

path = '/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps15/Calibrationconstants_ps15.csv'
path_pdf = '/Users/resi/Desktop/Schreibtisch - MacBook Pro von Theresa/SS2022/BA scibo/Calibrations/ps15/Calibrationskonstanten_pdf_ps15.pdf'

def get_data(channel):
    i = 10*channel + 2*channel
    with open(path, 'r') as csvfile:
        constants = csv.reader(csvfile, delimiter=';')
        rows = list(constants)
        run = rows[1]
        DAC_VOLTAGE_GAIN = rows[i+3]
        DAC_VOLTAGE_OFFSET = rows[i+4]
        ADC_U_LOAD_GAIN = rows[i+5]
        ADC_U_LOAD_OFFSET = rows[i+6]
        ADC_U_REGULATOR_GAIN = rows[i+7]
        ADC_U_REGULATOR_OFFSET = rows[i+8]
        ADC_I_MON_GAIN = rows[i+9]
        ADC_I_MON_OFFSET = rows[i+10]
        DAC_CURRENT_GAIN = rows[i+11]
        DAC_CURRENT_OFFSET = rows[i+12]
        plot(run[1:22],DAC_VOLTAGE_GAIN[1:22], 'mV/mV', 'lightcoral', 'DAC_VOLTAGE_GAIN', 1)
        plot(run[1:22], DAC_VOLTAGE_OFFSET[1:22], 'mV', 'darkgray', 'DAC_VOLTAGE_OFFSET', 2)
        plot(run[1:22], ADC_U_LOAD_GAIN[1:22], 'mV/mV', 'orange', 'ADC_U_LOAD_GAIN', 3)
        plot(run[1:22], ADC_U_LOAD_OFFSET[1:22], 'mV', 'darkseagreen', 'ADC_U_LOAD_OFFSET', 4)
        plot(run[1:22], ADC_U_REGULATOR_GAIN[1:22], 'mV/mV', 'cadetblue', 'ADC_U_REGULATOR_GAIN', 5)
        plot(run[1:22], ADC_U_REGULATOR_OFFSET[1:22], 'mV', 'cornflowerblue', 'ADC_U_REGULATOR_OFFSET', 6)
        plot(run[1:22], ADC_I_MON_GAIN[1:22], 'mA/mV', 'slateblue', 'ADC_I_MON_GAIN', 7)
        plot(run[1:22], ADC_I_MON_OFFSET[1:22], 'mV', 'thistle', 'ADC_I_MON_OFFSET', 8)
        plot(run[1:22], DAC_CURRENT_GAIN[1:22], 'mA/mV', 'plum', 'DAC_CURRENT_GAIN', 9)
        plot(run[1:22], DAC_CURRENT_OFFSET[1:22], 'mA', 'palevioletred', 'DAC_CURRENT_OFFSET', 10)


def plot(x_data, y_data,y_ax, color, title, n):
    plt.subplot(10, 1, n)
    y = [eval(i) for i in y_data]
    plt.scatter(x_data, y, color=color, linewidths=7.0, label=title)
    plt.ylabel(y_ax)
    plt.legend(prop={'size': 15})

def main():
    with PdfPages(path_pdf) as pdf:
        for channel in range(24):
            print('Plotting Channel %d' %channel)
            plt.subplots(figsize=(20, 30))
            get_data(channel)
            plt.suptitle('Channel %d\n'%channel, size=40.0)
            plt.xlabel('\n Date of Calibration Run', size=25.0)
            plt.tight_layout()
            pdf.savefig()
            plt.close()


if __name__ == '__main__':
    main()