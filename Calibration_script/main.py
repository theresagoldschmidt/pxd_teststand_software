import numpy as np
import matplotlib.pyplot as plt
import configparser
import os

config = configparser.ConfigParser()
config_ini = configparser.ConfigParser()
config_ini.optionxform = str
from Calibration_script import fit
np.seterr(divide='ignore', invalid='ignore')




def cut_outliers(x, y):
    """
    :param slopes: np array
    :return: cut data
    """

    slopes = y / x
    cut = np.zeros_like(slopes, dtype=bool)
    cut[np.isinf(slopes) | np.isnan(slopes)] = True
    cut[1:][np.isclose(np.diff(y), 0, atol = 1E-02)] = True
    mean, std = np.mean(slopes[~cut]), np.std(slopes[~cut])
    cut[np.logical_or((slopes > 2 * std + mean), (slopes < mean - 2 * std))] = True

    #print(np.count_nonzero(~cut) / len(slopes) * 100.)
    return x[~cut], y[~cut]



def linear(m,x,b):
    return m*x+b

def plot_and_fit(x, y,dx, dy, xlabel, ylabel, label):
    """
    :param x: Calibration data
    :param y: Calibration Data
    :return: slope and offset
    """

    #(m, b), (SSE,), *_ = np.polyfit(x, y, deg=1, full=True)
    popt, perr, red_chi_2 = fit.fit_odr(fit_func=linear,
                                        x=x,
                                        y=y,
                                        x_err=dx,
                                        y_err=dy,
                                        p0=[1.,2.])

    # Make result string
    res = "Fit results of {}:\n\t".format(fit.fit_odr.__name__)
    res += "m = ({:.3E} {} {:.3E})".format(popt[0], u'\u00B1', perr[0]) + " keV\n\t"
    res += "b = ({:.3E} {} {:.3E})".format(popt[1], u'\u00B1', perr[1]) + " keV\n\t"
    res += "red. Chi^2 = {:.3f}".format(red_chi_2) + "\n"
    m = popt[0]
    b = popt[1]


    # plot
    plt.scatter(x, y, color='grey', marker='.', label=label, linewidths=1.0)
    plt.errorbar(x, y, yerr=dy, xerr=dx, fmt='none', ecolor='grey', ms=1)
    plt.plot(x, m * x + b, color='r', label=f'slope = {round(m, 4)}, int= {round(b, 4)}''\n'f' $\chi^2$ = {round(red_chi_2, 4)}')
    plt.rcParams["figure.autolayout"] = True
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend(prop={'size':8})

    return (m, b)

def read_data(path, columns):
    dtype = [(col, '<f8') for col in columns]
    data = np.loadtxt(fname=path, delimiter=" ", skiprows=0, dtype=dtype)
    return data



def main():
    # Getting path from .ini file
    config.read("path.ini")
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



        # Opening the U_vs_U ini file
        path_UvsU = os.path.join(config["calibration_data"].get("data_path"), 'Channel_%d_U_vs_U' % channel + '.dat')

        columns_UvsU = ["$U_{dac}$ [mV]", "$U_{out}$ [mV]", "$U_{regulator}$ [mV]", "$U_{load}$ [mV]", "unknown 5",
                             "unknown 6"]

        data_UvsU = read_data(path_UvsU, columns_UvsU)




        # 0) Plot(U Cal: Uset vs. U out)
        x_0 = data_UvsU['$U_{dac}$ [mV]']
        y_0 = data_UvsU['$U_{out}$ [mV]']
        x_0, y_0 = cut_outliers(x_0, y_0)
        plt.subplot(2, 3, 1)
        m_0, b_0 = plot_and_fit(x_0, y_0, 10, 10, '$U_{dac}$ [mV]', '$U_{out}$ [mV]', '$U_{set} vs. U_{out}$')

        # 1) U Cal: Uout vs. MonUreg
        x_1 = data_UvsU['$U_{out}$ [mV]']
        y_1 = data_UvsU['$U_{regulator}$ [mV]']
        x_1, y_1 = cut_outliers(x_1, y_1)
        plt.subplot(2, 3, 2)
        m_1, b_1 = plot_and_fit(x_1, y_1, 10, 10, '$U_{out}$ [mV]', '$U_{regulator}$ [mV]', '$U_{out} vs. U_{regulator}$')

        # 2) U Cal: Uout vs. MonUload
        x_2 = data_UvsU['$U_{out}$ [mV]']
        y_2 = data_UvsU['$U_{load}$ [mV]']
        x_2, y_2 = cut_outliers(x_2, y_2)
        plt.subplot(2, 3, 3)
        m_2, b_2 = plot_and_fit(x_2, y_2, 10, 10, '$U_{out}$ [mV]', '$U_{load}$ [mV]', '$U_{out} vs. U_{load}$')

        # changing to I_vs_I.dat file 'Channel_%d_I_vs_I'%channel

        path_IvsI = os.path.join(config["calibration_data"].get("data_path"), 'Channel_%d_I_vs_I' % channel + '.dat')
        columns_IvsI = ["unknown 1", "$I_{dac}$ [mA]", "$I_{outMon}$ [mA]", "unknown 4", "unknown 5", "unknown 6"]

        data_IvsI = read_data(path_IvsI, columns_IvsI)


        # (3) I Cal: Iout vs. IoutMon
        x_3 = data_IvsI['$I_{dac}$ [mA]']
        y_3 = data_IvsI['$I_{outMon}$ [mA]']
        x_3, y_3 = cut_outliers(x_3, y_3)
        plt.subplot(2, 3, 4)
        if (channel == 13):
            m_3, b_3 = plot_and_fit(x_3, y_3, 0.001, 0.001,'$I_{dac}$ [mA]', '$I_{outMon}$ [$mu$A]', '$I_{out} vs. I_{outMon}$')
        else:
            m_3, b_3 = plot_and_fit(x_3, y_3, 1, 1, '$I_{dac}$ [mA]', '$I_{outMon}$ [mA]', '$I_{out} vs. I_{outMon}$')

        # changing to Ilimit_vs_I.dat file
        path_IlimitvsI = os.path.join(config["calibration_data"].get("data_path"),'Channel_%d_Ilimit_vs_I' % channel + '.dat')
        columns_IlimitvsI = ["$Limit DAC$ [mV]", "$Limit Current$ [mA]", "unknown 3", "unknown 4", "unknown 5"]

        data_IlimitvsI = read_data(path_IlimitvsI, columns_IlimitvsI)



        # 4) I Cal: DAC LIMIT vs. I Measured
        x_4 = data_IlimitvsI['$Limit DAC$ [mV]']
        y_4 = data_IlimitvsI['$Limit Current$ [mA]']
        x_4, y_4 = cut_outliers(x_4, y_4)
        plt.subplot(2, 3, 5)
        m_4, b_4 = plot_and_fit(x_4, y_4, 10, 1, '$Limit DAC$ [mV]', '$Limit Current$ [mA]', '$DAC LIMIT vs. I Measured$')
        if (channel == 13):
            m_4 = m_4 *1000
            b_4 = b_4 *1000



        # All 5 plots in one figure
        plt.subplots_adjust(left=0.1,
                            bottom=0.1,
                            right=0.9,
                            top=0.9,
                            wspace=0.6,
                            hspace=0.6)

        plt.savefig("plots/Channel_%d.pdf" % channel, format='pdf', bbox_inches='tight')
        plt.close()

        # writing in constants ini file
        config_ini[f'{channel}'] = {'DAC_VOLTAGE_GAIN': round(m_0 * 10000, 0),
                                    'DAC_VOLTAGE_OFFSET': round(b_0 * 100, 0),
                                    'ADC_U_LOAD_GAIN': round(m_1 * 10000, 0),
                                    'ADC_U_LOAD_OFFSET': round(b_1 * 100, 0),
                                    'ADC_U_REGULATOR_GAIN': round(m_2 * 10000, 0),
                                    'ADC_U_REGULATOR_OFFSET': round(b_2 * 100, 0),
                                    'ADC_I_MON_GAIN': round(m_3 * 10000, 0),
                                    'ADC_I_MON_OFFSET': round(b_3 * 100, 0),
                                    'DAC_CURRENT_GAIN': round(m_4 * 10000, 0),
                                    'DAC_CURRENT_OFFSET': round(b_4 * 100, 0)}

        print('Calculations for Channel %d finished' % channel)

    with open('constants.ini', 'w') as configfile:
        config_ini.write(configfile)



if __name__ == '__main__':
    main()

