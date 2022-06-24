import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import configparser
import os
config = configparser.ConfigParser()
config_ini = configparser.ConfigParser()
config_ini.optionxform = str

"""
def clean_graph(x, y)
    for i, coord_x in enumerate(x):
        window_x_left = list(x[max(0,i-7):i])
        window_x_right = list(x[i+1:min(i+9,len(x))])
        print(f"point {i}: window {window_x_left+window_x_right})
        if i == 10:
            break
"""
def plot_and_fit(x, y, xlabel, ylabel, label):

    (m, b), (SSE,), *_ = np.polyfit(x, y, deg=1, full=True)

    # plot
    plt.scatter(x, y, color='grey', marker='x', label=label, linewidths=1.0)
    plt.plot(x, m * x + b, color='r', label=f'slope = {round(m, 4)}, int= {round(b, 4)}')
    plt.rcParams["figure.autolayout"] = True
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend(prop={'size':8})

    return (m, b)



for channel in range(24):

    """
    The graphs:
    0) U Cal: Uset vs. Uout (x_axis: U_dac[mV], y_axis: U_output[mV]) 
    1) U Cal: Uout vs. MonUreg (x_axis: U_output[mV], y_axis: U_regulator[mV])
    2) U Cal: Uout vs. MonUload (x_axis: U_output[mV], y_axis: U_load[mV])
    3) I Cal: Iout vs. IoutMon  (x_axis: I_output[mA], y_axis: I_monitoring[mA])
    4) I Cal: DAC limit vs. IMeasured  (x_axis: Limit DAC[mV], y_axis: limit current[mA])
    """
    plt.subplots(figsize=(12, 6))

    #Getting path from .ini file
    config.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/path.ini")


    #Opening the U_vs_U ini file
    path_UvsU = os.path.join(config["calibration_data"].get("data_path"), 'Channel_%d_U_vs_U'%channel + '.dat')
    data_UvsU = pd.read_csv(path_UvsU,sep=" ",header=None)
    data_UvsU.columns=["$U_{dac}$ [mV]","$U_{out}$ [mV]","$U_{regulator}$ [mV]","$U_{load}$ [mV]","unknown 5","unknown 6"]# Number of columns you can see in the dat file.



    # 0) Plot(U Cal: Uset vs. U out)
    x_0 = data_UvsU['$U_{dac}$ [mV]']
    y_0 = data_UvsU['$U_{out}$ [mV]']
    plt.subplot(2, 3, 1)
    m_0, b_0 = plot_and_fit(x_0, y_0, '$U_{dac}$ [mV]','$U_{out}$ [mV]', '$U_{set} vs. U_{out}$')


    # 1) U Cal: Uout vs. MonUreg
    x_1 = data_UvsU['$U_{out}$ [mV]']
    y_1 = data_UvsU['$U_{regulator}$ [mV]']
    plt.subplot(2, 3, 2)
    m_1, b_1 = plot_and_fit(x_1, y_1, '$U_{out}$ [mV]', '$U_{regulator}$ [mV]', '$U_{out} vs. U_{regulator}$')


    # 2) U Cal: Uout vs. MonUload
    x_2 = data_UvsU['$U_{out}$ [mV]']
    y_2 = data_UvsU['$U_{load}$ [mV]']
    plt.subplot(2, 3, 3)
    m_2, b_2 = plot_and_fit(x_2, y_2, '$U_{out}$ [mV]', '$U_{load}$ [mV]', '$U_{out} vs. U_{load}$')



    #changing to I_vs_I.dat file 'Channel_%d_I_vs_I'%channel
    path_IvsI = os.path.join(config["calibration_data"].get("data_path"), 'Channel_%d_I_vs_I'%channel + '.dat')
    data_IvsI = pd.read_csv(path_IvsI,sep=" ",header=None)
    data_IvsI.columns=["unknown 1","$I_{dac}$ [mA]","$I_{outMon}$ [mA]","unknown 4","unknown 5","unknown 6"]# Number of columns you can see in the dat file.


    # (3) I Cal: Iout vs. IoutMon
    x_3 = data_IvsI['$I_{dac}$ [mA]']
    y_3 = data_IvsI['$I_{outMon}$ [mA]']
    plt.subplot(2, 3, 4)
    if (channel == 13):
        m_3, b_3 = plot_and_fit(x_3, y_3, '$I_{dac}$ [mA]', '$I_{outMon}$ [$mu$A]', '$I_{out} vs. I_{outMon}$')
    else:
        m_3, b_3 = plot_and_fit(x_3, y_3, '$I_{dac}$ [mA]', '$I_{outMon}$ [mA]', '$I_{out} vs. I_{outMon}$')



    #changing to Ilimit_vs_I.dat file
    path_Ilimit_vs_I = os.path.join(config["calibration_data"].get("data_path"), 'Channel_%d_Ilimit_vs_I'%channel + '.dat')
    data_IvsI = pd.read_csv(path_Ilimit_vs_I,sep=" ",header=None)
    data_IvsI.columns=["$Limit DAC$ [mV]","$Limit Current$ [mA]","unknown 3","unknown 4","unknown 5"]# Number of columns you can see in the dat file.


    # 4) I Cal: DAC LIMIT vs. I Measured
    x_4 = data_IvsI['$Limit DAC$ [mV]']
    y_4 = data_IvsI['$Limit Current$ [mA]']
    plt.subplot(2, 3, 5)
    m_4, b_4 = plot_and_fit(x_4, y_4, '$Limit DAC$ [mV]', '$Limit Current$ [mA]', '$DAC LIMIT vs. I Measured$')


    #All 5 plots in one figure
    plt.subplots_adjust(left=0.1,
                        bottom=0.1,
                        right=0.9,
                        top=0.9,
                        wspace=0.6,
                        hspace=0.6)

    plt.savefig("plots/Channel_%d.pdf"%channel, format='pdf', bbox_inches='tight')
    plt.close()


    #writing in constants ini file
    config_ini[f'{channel}'] = {'DAC_VOLTAGE_GAIN':round(m_0*10000, 0),
                                'DAC_VOLTAGE_OFFSET':round(b_0*100,0),
                                'ADC_U_LOAD_GAIN':round(m_1*10000, 0),
                                'ADC_U_LOAD_OFFSET':round(b_1*100,0),
                                'ADC_U_REGULATOR_GAIN':round(m_2*10000,0),
                                'ADC_U_REGULATOR_OFFSET':round(b_2*100,0),
                                'ADC_I_MON_GAIN':round(m_3*10000,0),
                                'ADC_I_MON_OFFSET':round(b_3*100, 0),
                                'DAC_CURRENT_GAIN':round(m_4*10000,0),
                                'DAC_CURRENT_OFFSET':round(b_4*100,0)}

    print('Calculations for Channel %d finished' %channel)

with open('constants.ini', 'w') as configfile:
      config_ini.write(configfile)

