import warnings
import numpy as np
import scipy.odr as odr
from scipy.optimize import curve_fit


def red_chisquare(meas, model, meas_err, model_popt):
    """
    Implements the reduced chisquare according to https://en.wikipedia.org/wiki/Reduced_chi-squared_statistic
    Parameters
    ----------
    meas: list, np.array
        Measurement data
    model: list, np.array
        Model data which aims to describe measurement data
    meas_err: list, np.array
        Measurement uncertainty
    model_popt: list, np.array
        Optimized model parameters; only needed for getting degree of freedom (a.k.a len(model_popt))

    Returns
    -------
    float: reduced chisquare
    """

    return np.sum(np.square((meas - model) / meas_err)) / (len(meas_err) - len(model_popt) - 1.0)


def fit_basic(fit_func, x, y, p0, y_err=None, return_pcov=False, **fit_kwargs):
    """
    Simple function that takes data as well as error and optimizes *fit_func* to
    it using non-linear least-squares fit provided by scipy.optimize.curve_fit.
    Additional *fit_kwargs* are passed to curve_fit directly
    fit_func: callable
        Function/model whose parameters are to be optimized to describe the *y* data
    x: list, np.array
        Input data x
    y: list, np.array
        Input data y
    y_err: list, np.array
        Uncertainties (1 sigma) on y input data
    p0: list, np.array
        Estimator of starting parameters for fitting routine
    return_pcov: bool
        Whether to append the covariance matrix of the fit parameters to the returned tuple
    Returns
    -------
    tuple: popt, perr, red_chisquare or popt, perr, red_chisquare, pcov
    """

    if p0 is None:
        warnings.warn("The *curve_fit* routine relies on proper starting parameters *p0* to ensure convergance.",
                      Warning)

    # We are using curve_fit; absolute_sigma=True indicates sigma has unit
    popt, pcov = curve_fit(f=fit_func, xdata=x, ydata=y, sigma=y_err, absolute_sigma=True, p0=p0, **fit_kwargs)

    # Calculate fit errors
    perr = np.sqrt(np.diag(pcov))

    # Calculate reduced chisquare
    red_chi_2 = red_chisquare(meas=y, model=fit_func(x, *popt), meas_err=y_err, model_popt=popt)

    return (popt, perr, red_chi_2) if not return_pcov else (popt, perr, red_chi_2, pcov)


def fit_odr(fit_func, x, y, p0, x_err=None, y_err=None, full_output=False):
    """
    Function that takes *x* and *y* as well as errors on __both__ inputs and optimizes
    *fit_func* to it using orthogonal-distance regression fit provided by scipy.odr
    fit_func: callable
        Function/model whose parameters are to be optimized to describe the *y* data
        Expected to be in the form of f(x, a, b, c, ...) -> y
    x: list, np.array
        Input data x
    y: list, np.array
        Input data y
    p0: list, np.array
        Estimator of starting parameters for fitting routine
    x_err: list, np.array
        Uncertainties (1 sigma) on y input data
    y_err: list, np.array
        Uncertainties (1 sigma) on y input data
    full_output: bool
        Whether to force the use of scipy.odr over scipy.optimize.curve_fit
    Returns
    -------
    tuple: popt, perr, red_chisquare or return value of odr.ODR.run()
    """

    # Expect the fit_function in the form of f(x, a, b, c, ...); re-write for ODR as vector
    odr_fit_func = lambda B, x: fit_func(x, *B)

    # Make fit model
    fit_model = odr.Model(odr_fit_func)

    # Make real data with standard deviations
    real_data = odr.RealData(x=x, y=y, sx=x_err, sy=y_err)

    # Make ODR instance
    _odr = odr.ODR(data=real_data, model=fit_model, beta0=p0)

    # Run ODR
    res = _odr.run()

    # Extract values from result of ODR.run()
    popt, perr, red_chi_2 = res.beta, res.sd_beta, res.res_var

    return (popt, perr, red_chi_2) if not full_output else res