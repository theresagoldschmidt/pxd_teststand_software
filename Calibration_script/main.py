import numpy as np
import matplotlib.pyplot as plt
import configparser
import os
from Calibration_script import fit
from matplotlib.backends.backend_pdf import PdfPages
import csv
from matplotlib import cm
from matplotlib.cm import ScalarMappable

config = configparser.ConfigParser()
config_ini = configparser.ConfigParser()
config_range = configparser.ConfigParser()
config_ini.optionxform = str




def prepare_data(x, y):
    """
    Excluding none and inf in data
    :param x: calibration data
    :param y: calibration data
    :return: calibration data without nan and inf
    """
    cut = np.zeros_like(x, dtype=bool)
    cut[np.isinf(x) | np.isnan(x)] = True
    cut[np.isinf(y) | np.isnan(y)] = True
    cut[x == 0] = True
    return x[~cut], y[~cut]


def cut_outliers(x, y, channel):
    """
    Cuts points that are to far away from the fit
    :param x: np array
    :return: cut data
    """
    # Calculating the slope
    slopes = (y - y[0])/x
    #m = np.polyfit(x[:20],y[:20],deg = 1)

    tolerance = abs(y[0] - y[-1])*0.01

    # Making array same size as data with only False in it
    cut = np.zeros_like(slopes, dtype=bool)
    # Set False to zero in parts where data gradient is close to zero
    if channel == 13:
        cut[:][np.isclose(np.gradient(y), 0, atol=tolerance)] = True

    else:

        cut[:][np.isclose(np.gradient(y), 0, atol=tolerance)] = True
        #cut[:][(abs(m[0]*x+m[1]))-y >= tolerance] = True

    if x[~cut].size == 0:
        return x[~cut], y[~cut], x[cut], y[cut]
    else:
        # calculate mean and standard derivation
        mean, std = np.mean(slopes[~cut]), np.std(slopes[~cut])
        # cut to  2 sigma
        cut[np.logical_or((slopes >= 2 * std + mean), (slopes <= mean - 2 * std))] = True



    return x[~cut], y[~cut],x[cut], y[cut]



def linear(m,x,b):
    """
    Defining the form of a linear function
    :param m: slope
    :param x: calibration data
    :param b: offset
    :return: returns form of linear function
    """
    return m*x+b

def get_and_prepare(data,x_data,y_data):
    x_1 = data[x_data]
    y_1 = data[y_data]
    length = len(x_1)
    x_2,y_2 = prepare_data(x_1, y_1)

    return x_2,y_2, length

def plot_and_fit(x, y, dx, dy, x_cut, y_cut, xlabel, ylabel, label,n):
    """
    Creates plots from cleaned calibration data and linear regression
    :param x: Calibration Data
    :param y: Calibration Data
    :param dx: error
    :param dy: error
    :param xlabel: Label of x axis
    :param ylabel:  label of y axis
    :param label: title of plot
    :return: plot, slope and offset
    """
    if x.size == 0:
        plt.subplot(2, 3, n)
        plt.scatter(x_cut, y_cut, color='grey', marker='.', linewidths=1.0, label='Outliers')
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.legend(prop={'size': 8})
        # default values
        m = 10000
        b = 0
        return m, b
    else:
        # p0 estimator
        p0 = np.mean((y-y[0])/x)
        # (m, b), (SSE,), *_ = np.polyfit(x, y, deg=1, full=True)
        popt, perr, red_chi_2 = fit.fit_odr(fit_func=linear,
                                            x=x,
                                            y=y,
                                            x_err=dx,
                                            y_err=dy,
                                            p0=[p0,y[0]])

        # Make result string
        res = "Fit results of {}:\n".format(fit.fit_odr.__name__)
        res += "m = ({:.3E} {} {:.3E})".format(popt[0], u'\u00B1', perr[0]) + " \n"
        res += "b = ({:.3E} {} {:.3E})".format(popt[1], u'\u00B1', perr[1]) + " \n"
        res += "red. Chi^2 = {:.3f}".format(red_chi_2) + "\n"
        m = popt[0]
        b = popt[1]

        # plot each of 5 subplots
        plt.subplot(2, 3, n)
        plt.scatter(x, y, color='k', marker='.', linewidths=1.0 )
        plt.scatter(x_cut, y_cut, color='grey',marker='.', linewidths=1.0, label = 'Outliers')
        plt.errorbar(x, y, yerr=dy, xerr=dx, fmt='none', ecolor='k', label = label)
        plt.plot(x, m * x + b, color='r', label=f'slope = {round(m, 4)}''\n' f'int= {round(b, 4)}''\n'r' $\chi^2_{\mathrm{red}}$ =' f'{round(red_chi_2, 4)}')
        plt.rcParams["figure.autolayout"] = True
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.legend(prop={'size':8})
        return m, b

def residual_plots(data_x, data_y,x,y,cut_x, cut_y, m, b, n):
    """
    Plots the rediuals for the data points
    :param data_x: Name of the x data
    :param data_y: Name of the y data
    :param x: the x data without the outliers
    :param y: the y data without the outliers
    :param cut_x: the x outliers
    :param cut_y: the y outliers
    :param m: the slope
    :param b: the offset
    :param n: which subplot
    :return: residual plots
    """
    plt.subplot(2,3,n)
    plt.scatter(x, y-(m * x + b) , color='g', marker='.', linewidths=1.0, label="Residual:\n"+data_x+" vs. "+data_y)
    plt.scatter(cut_x, cut_y-(m * cut_x + b) , color='grey', marker='.', linewidths=1.0)
    plt.xlabel(data_x)
    plt.ylabel(data_y)
    plt.legend(prop={'size': 8})

def help_plots(data, data_x, data_y, title,n):
    x_0 = data[data_x]
    y_0 = data[data_y]
    plt.subplot(2, 3, n)
    plt.scatter(x_0, y_0, color='b', marker='.', linewidths=1.0, label=title)
    plt.xlabel(data_x)
    plt.ylabel(data_y)
    plt.legend(prop={'size': 8})

def read_data(path, columns):
    """
    Gets data from path
    :param path: path to .dat diles
    :param columns: multiple columns in each file
    :return: data
    """
    dtype = [(col, '<f8') for col in columns]
    data = np.loadtxt(fname=path, delimiter=" ", skiprows=0, dtype=dtype)
    return data

def SMU_V_error(data):
    error=np.zeros_like(data)

    for i in range(len(error)):
        if (abs(data[i])<200):
            error[i] =  data[i]*0.00015+0.225
        elif (abs(data[i])<2000):
            error[i] = data[i]*0.00015+0.325
        elif (abs(data[i])< 20000):
            error[i] = data[i]*0.00015+5
        elif (abs(data[i])< 200000):
            error[i] = data[i]*0.00015+50
    return error

def SMU_I_error(data, channel):
    error=np.zeros_like(data)
    if channel == 13:
        np.multiply(data, 0.0001)
    for i in range(len(error)):
        if (abs(data[i])<0.1):
            error[i] =  data[i]*0.0002+0.000025
        elif (abs(data[i])<1):
            error[i] = data[i]*0.0002+0.0002
        elif (abs(data[i])< 10):
            error[i] = data[i]*0.0002+0.0025
        elif (abs(data[i])< 100):
            error[i] = data[i]*0.0002+0.02
        elif (abs(data[i]) < 1000):
            error[i] = data[i] * 0.0003 + 1.5
        elif (abs(data[i]) < 1500):
            error[i] = data[i] * 0.0005 + 3.5
        elif (abs(data[i]) < 3000):
            error[i] = data[i] * 0.04 + 7
        elif (abs(data[i]) < 10000):
            error[i] = data[i] * 0.04 + 25
    return error


def histo_deleted_points(length):
    Channel = []
    plot_0 = []
    plot_1 = []
    plot_2 = []
    plot_3 = []
    plot_4 = []
    with open('deleted_points.csv', 'r') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        next(plots)
        for row in plots:
            Channel.append(row[0])
            plot_0.append(int(row[1]))
            print(plot_0)
            plot_1.append(int(row[2]))
            plot_2.append(int(row[3]))
            plot_3.append(int(row[4]))
            plot_4.append(int(row[5]))
    plt.subplots(figsize=(12, 6))
    plot_histo(Channel, plot_0, '$U_{out} vs. U_{dac}$',1, length )
    plot_histo(Channel, plot_1, '$U_{regulator} vs. U_{out}$', 2, length)
    plot_histo(Channel, plot_2, '$U_{load} vs. U_{out}$', 3, length)
    plot_histo(Channel, plot_3,'$I_{outMON} vs. I_{SMU}$', 4, length)
    plot_histo(Channel, plot_4,'$Limit Current vs. I Limit_{DAC}$', 5, length)
    plt.subplots_adjust(left=0.1,
                        bottom=0.1,
                        right=0.9,
                        top=0.9,
                        wspace=0.6,
                        hspace=0.6)

    plt.savefig(os.path.join(config["calibration_data"].get("data_path"),'deleted_points.pdf'))
    plt.close()


def plot_histo(x,y,title,n, length):

    color_map = cm.get_cmap('RdYlGn_r')

    data_hight_normalized = [z / length for z in y]
    colors = color_map(data_hight_normalized)


    plt.subplot(2, 3, n)
    plt.bar(x, y, color=colors, width=0.72, label=title)
    plt.xlabel('Channel')
    plt.xticks(fontsize=6)
    plt.ylabel('Deleted Points')
    plt.legend(prop={'size': 8})
    plt.rcParams["figure.autolayout"] = True

    sm = ScalarMappable(cmap=color_map, norm=plt.Normalize(0, length))
    sm.set_array([])
    plt.colorbar(sm)
    #cbar.set_label('Color', rotation=270, labelpad=25)

def pass_fail():
    config_range.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/constants_range.ini")
    Channel = []
    plot_0 = []
    plot_1 = []
    plot_2 = []
    plot_3 = []
    plot_4 = []
    success = False
    with open('deleted_points.csv', 'r') as csvfile:
        plots = csv.reader(csvfile, delimiter=',')
        next(plots)
        for row in plots:
            Channel.append(int(row[0]))
            plot_0.append(int(row[1]))
            plot_1.append(int(row[2]))
            plot_2.append(int(row[3]))
            plot_3.append(int(row[4]))
            plot_4.append(int(row[5]))

            if int(row[1]) > 50 or int(row[2]) > 50 or int(row[3]) > 50 or int(row[4]) > 50 or int(row[5]) > 50:
                print('Warning! Please check Channel %d.'%(int(row[0])))
                success = True
            else:
                pass


        in_range = 0

        for channel in range(24):
            if get_range(f'DAC_VOLTAGE_GAIN', f'DAC_VOLTAGE_OFFSET', channel) == True:
                print('Warning! Please check Channel %d. DAC_VOLTAGE out of usual range.' % channel)
                in_range+= 1
            if get_range(f'ADC_U_LOAD_GAIN', f'ADC_U_LOAD_OFFSET', channel) == True:
                print('Warning! Please check Channel %d. ADC_U_LOAD out of usual range.' % channel)
                in_range += 1
            if get_range(f'ADC_U_REGULATOR_GAIN', f'ADC_U_REGULATOR_OFFSET', channel) == True:
                print('Warning! Please check Channel %d. ADC_U_REGULATOR out of usual range.' % channel)
                in_range += 1
            if get_range(f'ADC_I_MON_GAIN', f'ADC_I_MON_OFFSET', channel) == True:
                print('Warning! Please check Channel %d. ADC_I_MON out of usual range.' % channel)
                in_range += 1
            if get_range(f'DAC_CURRENT_GAIN', f'DAC_CURRENT_OFFSET', channel) == True:
                print('Warning! Please check Channel %d. DAC_CURRENT out of usual range.' % channel)
                in_range += 1
            else:
                pass
        if success == False and in_range == 0:
            print('Calibration was successful!')
        elif success == False and in_range != 0:
            print('Calibration was NOT successful! Please check warnings!')
        elif success == True and in_range == 0:
            print('Calibration was NOT successful! To many points were deleted!')


def get_range(name_gain, name_offset,channel):
    in_range = False
    gain_upper = float(config_range[f'{channel}'].get(name_gain + '_UPPER'))
    gain_lower = float(config_range[f'{channel}'].get(name_gain + '_LOWER'))
    gain = float(config_ini[f'{channel}'].get(name_gain))

    offset_upper = float(config_range[f'{channel}'].get(name_offset + '_UPPER'))
    offset_lower = float(config_range[f'{channel}'].get(name_offset + '_LOWER'))
    offset = float(config_ini[f'{channel}'].get(name_offset))

    if gain > gain_upper:
        in_range = True
    elif gain < gain_lower:
        in_range = True
    elif offset > offset_upper:
        in_range = True
    elif offset < offset_lower:
        in_range = True
    else:
        pass

    return in_range

def main():
    # Getting path from .ini file
    config.read("path.ini")
    # Making the folders for the  plots
    if os.path.exists(os.path.join(config["calibration_data"].get("data_path"),"plots")) == True:
        pass
    else:
        os.mkdir(os.path.join(config["calibration_data"].get("data_path"),"plots"))

    with open('deleted_points.csv', 'w', encoding='UTF8') as csvfile:
        fieldnames = ['Channel', 'plot_0', 'plot_1', 'plot_2', 'plot_3', 'plot_4']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        for channel in range(24):
            path = os.path.join(config["calibration_data"].get("data_path"),"plots/Channel_%d.pdf" % channel )

            with PdfPages(path) as pdf:
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
                columns_UvsU = ["$U_{dac}$ [mV]", "$U_{out}$ [mV]", "$U_{regulator}$ [mV]", "$U_{load}$ [mV]", "unknown 5","unknown 6"]
                data_UvsU = read_data(path_UvsU, columns_UvsU)

                # Opening I_vs_I.dat file
                path_IvsI = os.path.join(config["calibration_data"].get("data_path"),'Channel_%d_I_vs_I' % channel + '.dat')
                columns_IvsI = ["unknown 1", "$I_{out(SMU)}$ [mA]", "$I_{outMon}$ [mV]", "$U_{outMon}$", "StatBit","$U_{SMU}$"]
                data_IvsI = read_data(path_IvsI, columns_IvsI)

                # Opening Ilimit_vs_I.dat file
                path_IlimitvsI = os.path.join(config["calibration_data"].get("data_path"),'Channel_%d_Ilimit_vs_I' % channel + '.dat')
                columns_IlimitvsI = ["$I Limit DAC$ [mV]", "$Limit Current$ [mA]", "unknown 3", "unknown 4", "StatBit"]
                data_IlimitvsI = read_data(path_IlimitvsI, columns_IlimitvsI)



                # 0) Plot(U Cal: Uset vs. U out)
                x_0,y_0,l_0= get_and_prepare(data_UvsU, '$U_{dac}$ [mV]', '$U_{out}$ [mV]')
                x_0, y_0,x_cut_0,y_cut_0 = cut_outliers(x_0, y_0, channel)
                y_err_0 = SMU_V_error(y_0)
                m_0, b_0 = plot_and_fit(x_0, y_0, 4, y_err_0, x_cut_0,y_cut_0,  '$U_{dac}$ [mV]', '$U_{out}$ [mV]', '$U_{out} vs. U_{dac}$',1)

                # 1) U Cal: Uout vs. MonUreg
                x_1,y_1, l_1= get_and_prepare(data_UvsU, '$U_{out}$ [mV]', '$U_{regulator}$ [mV]')
                x_1, y_1,x_cut_1,y_cut_1 = cut_outliers(x_1, y_1, channel)
                x_err_1 = SMU_V_error(x_1)
                m_1, b_1 = plot_and_fit(x_1, y_1, x_err_1, 30 ,x_cut_1,y_cut_1, '$U_{out}$ [mV]', '$U_{regulator}$ [mV]', '$U_{regulator} vs. U_{out}$',2)

                # 2) U Cal: Uout vs. MonUload
                x_2,y_2, l_2= get_and_prepare(data_UvsU, '$U_{out}$ [mV]', '$U_{load}$ [mV]')
                x_2, y_2, x_cut_2,y_cut_2 = cut_outliers(x_2, y_2, channel)
                x_err_2 = SMU_V_error(x_2)
                m_2, b_2 = plot_and_fit(x_2, y_2, x_err_2, 30, x_cut_2,y_cut_2, '$U_{out}$ [mV]', '$U_{load}$ [mV]', '$U_{load} vs. U_{out}$',3)

                # 3) I Cal: Iout vs. IoutMon
                x_3,y_3, l_3 = get_and_prepare(data_IvsI, '$I_{out(SMU)}$ [mA]', '$I_{outMon}$ [mV]')
                x_3, y_3, x_cut_3,y_cut_3= cut_outliers(x_3, y_3, channel)
                x_err_3 = SMU_I_error(x_3, channel)
                if channel == 13:
                    m_3, b_3 = plot_and_fit(x_3, y_3, x_err_3, 0.03,x_cut_3,y_cut_3,'$I_{SMU}$ [mA]', '$I_{outMon}$ [$mu$V] (V because ADC)', '$I_{outMON} vs. I_{SMU}$',4)
                    #m_3 = m_3/1000
                else:
                    m_3, b_3 = plot_and_fit(x_3, y_3, x_err_3, 30, x_cut_3,y_cut_3, '$I_{SMU}$ [mA]', '$I_{outMon}$ [mV] (V because ADC)', '$I_{outMOn} vs. I_{SMU}$',4)

                # 4) I Cal: DAC LIMIT vs. I Measured
                x_4,y_4,l_4 = get_and_prepare(data_IlimitvsI, '$I Limit DAC$ [mV]', '$Limit Current$ [mA]')
                x_4, y_4, x_cut_4,y_cut_4 = cut_outliers(x_4, y_4, channel)
                y_err_4= SMU_I_error(y_4, channel)
                m_4, b_4 = plot_and_fit(x_4, y_4, 4, y_err_4, x_cut_4,y_cut_4, '$I Limit_{DAC}$ [mV]', '$Limit Current$ [mA]', '$Limit Current vs. I Limit_{DAC}$',5)
                #if channel == 13:
                #    m_4 = m_4 * 1000
                #    b_4 = b_4 * 1000

                # Calculating
                title = "$\\bf{Number\:of\:deleted\:points:}$\n\n"
                plot_0 = '0) U Cal: Uset vs. U out: $\\bf{%d}$\n'%(l_0-len(x_0))
                plot_1 = '1) U Cal: Uout vs. MonUreg: $\\bf{%d}$\n'%(l_1-len(x_1))
                plot_2 = '2) U Cal: Uout vs. MonUload: $\\bf{%d}$\n'%(l_2-len(x_2))
                plot_3 = '3) I Cal: Iout vs. IoutMon: $\\bf{%d}$\n'%(l_3-len(x_3))
                plot_4 = '4) I Cal: DAC LIMIT vs. I Measured:$\\bf{%d}$ \n'%(l_4-len(x_4))

                plt.figtext(0.75, 0.18,title+plot_0+plot_1+plot_2+plot_3+plot_4,bbox=dict(facecolor='lightgrey', edgecolor='red'), fontdict=None)


                writer.writerow({'Channel': channel, 'plot_0': '%d'%(l_0-len(x_0)), 'plot_1': '%d'%(l_1-len(x_1)), 'plot_2': '%d'%(l_2-len(x_2)), 'plot_3': '%d'%(l_3-len(x_3)), 'plot_4': '%d'%(l_4-len(x_4))})


                # All 5 plots in one figure
                plt.subplots_adjust(left=0.1,
                                    bottom=0.1,
                                    right=0.9,
                                    top=0.9,
                                    wspace=0.6,
                                    hspace=0.6)
                plt.tight_layout()
                pdf.savefig()
                plt.close()

                # Now plotting the residual PLots
                plt.subplots(figsize=(12, 6))

                # 0) Plot(U Cal: Uset vs. U out)
                residual_plots('$U_{dac}$ [mV]', '$U_{out}$ [mV]',x_0, y_0, x_cut_0, y_cut_0, m_0, b_0,1)
                # 1) U Cal: Uout vs. MonUreg
                residual_plots('$U_{out}$ [mV]', '$U_{regulator}$ [mV]',x_1, y_1, x_cut_1, y_cut_1, m_1, b_1,2)
                # 2) U Cal: Uout vs. MonUload
                residual_plots('$U_{out}$ [mV]', '$U_{load}$ [mV]',x_2, y_2, x_cut_2, y_cut_2, m_2, b_2,3)
                # (3) I Cal: Iout vs. IoutMon
                if (channel == 13):
                    residual_plots('$I_{out(dac)}$ [mA]', '$I_{outMon}$ [mV]',x_3, y_3, x_cut_3, y_cut_3, m_3, b_3,4)
                else:
                    residual_plots('$I_{out(dac)}$ [mA]', '$I_{outMon}$ [mV]',x_3, y_3, x_cut_3, y_cut_3, m_3, b_3,4)
                # 4) I Cal: DAC LIMIT vs. I Measured
                residual_plots('$Limit DAC$ [mV]', '$Limit Current$ [mA]',x_4, y_4, x_cut_4, y_cut_4, m_4, b_4, 5)

                # All 5 residual plots in one figure
                plt.subplots_adjust(left=0.1,
                                    bottom=0.1,
                                    right=0.9,
                                    top=0.9,
                                    wspace=0.6,
                                    hspace=0.6)

                pdf.savefig()
                plt.close()

                # Now plotting the help PLots
                plt.subplots(figsize=(12, 6))
                # 1) I Cal: Iout vs. UoutMon- horizontal line
                help_plots(data_IvsI, '$I_{out(SMU)}$ [mA]', '$U_{outMon}$', 'I Cal: Iout vs. UoutMon - horizontal line', 1)
                # 2) I Cal: Iout vs. U-SMU - horizontal line
                help_plots(data_IvsI, '$I_{out(SMU)}$ [mA]', '$U_{SMU}$', 'I Cal: Iout vs. U-SMU - horizontal line', 2)
                # 3) I Cal: Iout vs. StatBit - should be high
                help_plots(data_IvsI, '$I_{out(SMU)}$ [mA]', 'StatBit', 'I Cal: Iout vs. StatBit - should be high', 3)
                # 4) Limit Cal: I Limit out vs. StatBit - should be low
                help_plots(data_IlimitvsI, '$Limit Current$ [mA]', 'StatBit', 'LimitCal: I Limit vs. StatBit - should be low',4)

                # All 5 residual plots in one figure
                plt.subplots_adjust(left=0.1,
                                    bottom=0.1,
                                    right=0.9,
                                    top=0.9,
                                    wspace=0.6,
                                    hspace=0.6)

                pdf.savefig()
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

            #with open(os.path.join(config["calibration_data"].get("data_path"),'constants.ini'), 'w') as configfile:

            with open(os.path.join(config["calibration_data"].get("data_path"),'constants.ini'), 'w') as configfile:
                config_ini.write(configfile)

        csvfile.close()
        print('Plotting Histogram with Number of deleted points...')
        histo_deleted_points(l_1)
        print('Checking if Calibration was successful...\n')
        pass_fail()



if __name__ == '__main__':
    main()

