import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import configparser
import os
config = configparser.ConfigParser()
config_ini = configparser.ConfigParser()
config_ini.optionxform = str

config_ini['0'] = {}
"""
The graphs:
0) U Cal: Uset vs. Uout (x_axis: U_dac[mV], y_axis: U_output[mV]) 
1) U Cal: Uout vs. MonUreg (x_axis: U_output[mV], y_axis: U_regulator[mV])
2) U Cal: Uout vs. MonUload (x_axis: U_output[mV], y_axis: U_load[mV])
3) I Cal: Iout vs. IoutMon  (x_axis: I_output[mA], y_axis: I_monitoring[mA])
4) I Cal: DAC limit vs. IMeasured  (x_axis: Limit DAC[mV], y_axis: limit current[mA])
"""

""" 
PLOTS (0)-(2)________________________________________#_______________________________________________________
"""

config.read("/Users/resi/PycharmProjects/pxd_teststand_software/Calibration_script/path.ini")
config["calibration_data"].get("data_path")

data_UvsU = pd.read_csv(config["calibration_data"].get("data_path"),sep=" ",header=None)
data_UvsU.columns=["1","2","3","4","5","6"]# Number of columns you can see in the dat file.




#config = iniFile( 'FILE.INI' )
#print(config.default_path )

"""
0) Plot(U Cal: Uset vs. U out)
"""

x_0 = data_UvsU['1']
y_0 = data_UvsU['2']

# calculating linear regression
(m_0, b_0), (SSE,), *_ = np.polyfit(x_0, y_0, deg=1, full=True)

# plot
plt.subplot(2, 3, 1)
plt.scatter(x_0,y_0, color='grey', marker='x', label='$U_{set} vs. U_{out}$')
plt.plot(x_0, m_0 * x_0 + b_0, color='r', label=f'slope = {round(m_0, 4)}, int= {round(b_0,4)}' )
plt.xlabel('$U_{dac}$ [mV]')
plt.ylabel('$U_{out}$ [mV]')
#plt.title('$U_{set} vs. U_{out}$')
plt.legend()


print(f'DAC_VOLTAGE_GAIN={round(m_0, 4)*10000} {os.linesep}DAC_VOLTAGE_OFFSET={round(b_0,4)*100}  ')
#plt.show()
#config_ini['0'] = {f'DAC_VOLTAGE_GAIN':round(m_0, 4)*10000,
#                   f'DAC_VOLTAGE_OFFSET':round(b_0,4)*100}



"""
1) U Cal: Uout vs. MonUreg
"""

x_1 = data_UvsU['2']
y_1 = data_UvsU['3']

# calculating linear regression
(m_1, b_1),  *_ = np.polyfit(x_1, y_1, deg=1, full=True)

# plot
plt.subplot(2, 3, 2)
plt.scatter(x_1,y_1, color='grey', marker='x', label='$U_{out} vs. U_{regulator}$')
plt.plot(x_1, m_1 * x_1 + b_1, color='r', label= f'slope = {round(m_1, 4)}, int= {round(b_1,4)}')
plt.xlabel('$U_{out}$ [mV]')
plt.ylabel('$U_{regulator}$ [mV]')
#plt.title('$U_{out} vs. U_{regulator}$')
plt.legend()

print(f'ADC_U_LOAD_GAIN={round(m_1, 4)*10000} {os.linesep}ADC_U_LOAD_OFFSET={round(b_1,4)*100} ')
#plt.show()
config_ini['0'] = {f'ADC_U_LOAD_GAIN':round(m_1, 4)*10000,
                   f'ADC_U_LOAD_OFFSET':round(b_1,4)*100}
"""                                                                                               
2) U Cal: Uout vs. MonUload                                                                        
"""

x_2 = data_UvsU['2']
y_2 = data_UvsU['4']

# calculating linear regression
(m_2, b_2),  *_ = np.polyfit(x_2, y_2, deg=1, full=True)

# plot
plt.subplot(2, 3, 3)
plt.scatter(x_2,y_2, color='grey', marker='x', label='$U_{out} vs. U_{load}$' )
plt.plot(x_2, m_2 * x_2 + b_2, color='r', label= f'slope = {round(m_2, 4)}, int= {round(b_2,4)}' )
plt.xlabel('$U_{out}$ [mV]')
plt.ylabel('$U_{load}$ [mV]')
#plt.title('$U_{out} vs. U_{load}$')
plt.legend()

print(f'ADC_U_REGULATOR_GAIN={round(m_2, 4)*10000} {os.linesep}ADC_U_REGULATOR_={round(b_2,4)*100} ')
#plt.show()
config_ini['0'] = {f'ADC_U_REGULATOR_GAIN':round(m_2, 4)*10000,
                   f'ADC_U_REGULATOR':round(b_2,4)*100}
"""                                                                                                                 
PLOTS (3)___________________________________________________________________________________________________        
"""
#still need a dynamic solution for the path stuff

data_IvsI = pd.read_csv(config["calibration_data"].get("data_path_2"),sep=" ",header=None)
data_IvsI.columns=["1","2","3","4","5","6"]# Number of columns you can see in the dat file.

"""
(3) I Cal: Iout vs. IoutMon
"""

x_3 = data_IvsI['2']
y_3 = data_IvsI['3']

# calculating linear regression
(m_3, b_3), *_ = np.polyfit(x_3, y_3, deg=1, full=True)

# plot
plt.subplot(2, 3, 4)
plt.scatter(x_3,y_3, color='grey', marker='x', label='$I_{out} vs. I_{outMon}$' )
plt.plot(x_3, m_3 * x_3 + b_3, color='r', label= f'slope = {round(m_3, 4)}, int= {round(b_3,4)}' )
plt.xlabel('$I_{dac}$ [mA]')
plt.ylabel('$I_{outMon}$ [mA]')
plt.legend()
#plt.title('$I_{out} vs. I_{outMon}$')
#plt.text(-1000,200, f'slope = {round(m_3, 4)}, int= {round(b_3,4)}')

print(f'ADC_I_MON_GAIN={round(m_3, 4)*10000} {os.linesep}ADC_I_MON_OFFSET={round(b_3,2)*100} ')
#plt.show()
config_ini['0'] = {f'ADC_I_MON_GAIN':round(m_3, 4)*10000,
                   f'ADC_I_MON_OFFSET':round(b_3,4)*100}


""" 
PLOTS (4)_______________________________________________________________________________________________data_IvsI = pd.read_csv(config["calibration_data"].get("data_path"),sep=" ",header=None)
"""

data_IvsI = pd.read_csv(config["calibration_data"].get("data_path_3"),sep=" ",header=None)
data_IvsI.columns=["1","2","3","4","5"]# Number of columns you can see in the dat file.

"""
4) I Cal: DAC LIMIT vs. I Measured
"""

x_4 = data_IvsI['1']
y_4 = data_IvsI['2']

# calculating linear regression
(m_4, b_4),  *_ = np.polyfit(x_4, y_4, deg=1, full=True)

# plot
plt.subplot(2, 3, 5)
plt.scatter(x_4,y_4, color='grey', marker='x', label='$DAC LIMIT vs. I Measured$')
plt.plot(x_4, m_4 * x_4 + b_4, color='r', label=f'slope = {round(m_4, 4)}, int= {round(b_4,4)}' )
plt.xlabel('$Limit DAC$ [mV]')
plt.ylabel('$Limit Current$ [mA]')
#plt.title('$DAC LIMIT vs. I Measured$')
plt.legend()


print(f'DAC_CURRENT_GAIN={round(m_4, 4)*10000} {os.linesep}DAC_CURRENT_OFFSET={round(b_4,4)*100}  ')
plt.show()

"""
writing in constants ini file
guess there would be a nicer solution, but hevent found it yet
wanted to do it after each plotting but only shoed the last constants 
"""
config_ini['0'] = {f'DAC_VOLTAGE_GAIN':round(m_0, 4)*10000,
                   f'DAC_VOLTAGE_OFFSET':round(b_0,2)*100,
                   f'ADC_U_LOAD_GAIN':round(m_1, 4)*10000,
                   f'ADC_U_LOAD_OFFSET':round(b_1,2)*100,
                   f'ADC_U_REGULATOR_GAIN':round(m_2, 4)*10000,
                   f'ADC_U_REGULATOR':round(b_2,2)*100,
                   f'ADC_I_MON_GAIN':round(m_3, 4)*10000,
                   f'ADC_I_MON_OFFSET':round(b_3, 2)*100,
                   f'DAC_CURRENT_GAIN':round(m_4, 4)*10000,
                   f'DAC_CURRENT_OFFSET':round(b_4,2)*100}


with open('constants.ini', 'w') as configfile:
  config_ini.write(configfile)
