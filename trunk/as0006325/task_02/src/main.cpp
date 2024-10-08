#include <iostream>
#include <cmath>

/**
 * \brief An abstract model's class
 *
 * Model is used to represent other models.
 */
class Model {
public:
    /**
     * @brief Virtual function for temperature determination
     *
     * @param Yt: input temperature
     * @param Uw: input warm
     * @return double output temperature
     *
     */
    virtual double simulate_temperature(double Yt, double Uw) = 0;

    /**
     * @brief Destroy the model object
     *
     */
    virtual ~Model() = default;
};

/**
 * @brief Linear model of temperature control
 *
 */
class LinearModel : public Model {
private:
    double a; ///< Parametr a
    double b; ///< Parametr b
public:
    /**
     * @brief Construct a new LinearModel object
     *
     * @param a,b: some constants
     *
     */
    LinearModel(double a, double b) : a(a), b(b) {}

    /**
     * @brief This is a redefined method for calculating the output temperature of a linear model object
     */
    double simulate_temperature(double Yt, double Uw) final {
        return a * Yt + b * Uw;
    }

    /**
     * @brief Destroy the LinearModel object
     */
    ~LinearModel() override = default;
};

/**
 * @brief Nonlinear model of temperature control
 *
 */
class NonlinearModel : public Model {
private:
    double a; ///< Parametr a
    double b; ///< Parametr b
    double c; ///< Parametr c
    double d; ///< Parametr d
    //! Previous temperature value
    double PreYt = 0;
    //! Previous warm value
    double PreUw = 0;
public:

    /**
     * @brief Construct a new NonlinearModel object
     * @param a,b,c,d: some constants
     */
    NonlinearModel(double a, double b, double c, double d) :
        a(a),
        b(b),
        c(c),
        d(d) {}

    /**
     * @brief This is a redefined method for calculating the output temperature of a nonlinear model object
     */
    double simulate_temperature(double Yt, double Uw) final {
        double calc = a * Yt - b * pow(PreYt, 2) + c * Uw + d * sin(PreUw);
        PreYt = Yt;
        PreUw = Uw;
        return calc;
    }

    /**
     * @brief Destroy the NonlinearModel object
     *
     */
    ~NonlinearModel() override = default;
};

/**
 * @brief Proportional–integral–derivative regulator
 */
class PIDregulator
{
private:
    //! Transfer coefficient
    const double K = 0.1;
    //! Integration constant
    const double T = 10;
    //! Differentiation constant
    const double TD = 80;
    //! Quantization step
    const double T0 = 50;
    //! Simulation time
    const double numOfTimeModeling = 30;
    //! Control variable value
    double Uk = 0;

    /**
     * @brief Calculate current control value
     *
     * @param e deviation from the desired value
     * @param e1 previous deviation from the desired value
     * @param e2 previous deviation from e1
     * @return double updated control variable value
     */
    double calculate_Uk(double e, double e1, double e2)
    {
        double q0 = K * (1 + TD / T0);
        double q1 = -K * (1 + 2 * TD / T0 - T0 / T);
        double q2 = K * TD / T0;
        Uk += q0 * e + q1 * e1 + q2 * e2;
        return Uk;
    }

public:

    /**
     * @brief Modeling regulator
     *
     * @param w desired value
     * @param y0 initial temperature
     * @param model linear or nonlinear model
     */
    void Regulate(double w, double y0, Model& model)
    {
        double e1 = 0;
        double e2 = 0;
        double y = y0;
        for (int i = 1; i <= numOfTimeModeling; i++) {
            double e;
            e = w - y;
            Uk = calculate_Uk(e, e1, e2);
            y = model.simulate_temperature(y0, Uk);
            std::cout << "E = " << e << ", Yt = " << y << ", Uk = " << Uk << std::endl;
            e2 = e1;
            e1 = e;
        }
        Uk = 0;
    }
};

void input_parametrs(double& a, double& b, double& c, double& d, bool is_nonlinearModel) {
    std::cout << "Input a-parameter: "; std::cin >> a;
    std::cout << "Input b-parameter: "; std::cin >> b;

    if (is_nonlinearModel) {
        std::cout << "Input c-parameter: "; std::cin >> c;
        std::cout << "Input d-parameter: "; std::cin >> d;
    }
}
int main() {
    const double w = 8;
    const double y0 = 3;
    double a;
    double b;
    double c;
    double d;

    std::cout << "---Please input LinearModel's constant parameters---" << std::endl;
    input_parametrs(a, b, c, d, false);
    LinearModel linear_model{ a,b };

    std::cout << "---Please input NonlinearModel's constant parameters---" << std::endl;
    input_parametrs(a, b, c, d, true);
    NonlinearModel nonlinear_model{ a,b,c,d };

    PIDregulator pid_regulator;

    std::cout << "\t---LinearModel---" << std::endl;
    pid_regulator.Regulate(w, y0, linear_model);
    std::cout << std::endl;

    std::cout << "\t---NonlinearModel---" << std::endl;
    pid_regulator.Regulate(w, y0, nonlinear_model);
    system("Pause");
}