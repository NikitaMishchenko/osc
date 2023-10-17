#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "core/function.h"

class  GnuslSplineWrapper
{
public:
    GnuslSplineWrapper(const Function &input)
    {
        m_x = new double[input.size()];
        m_y = new double[input.size()];

        for (int i = 0; i < input.size(); i++)
        {
            m_x[i] = input.getDomain(i);
            m_y[i] = input.getCodomain(i);
        }

        m_acc = gsl_interp_accel_alloc();
        m_spline = gsl_spline_alloc(gsl_interp_cspline, input.size());

        gsl_spline_init(m_spline, m_x, m_y, input.size());
    }

    ~GnuslSplineWrapper()
    {
        gsl_spline_free(m_spline);
        gsl_interp_accel_free(m_acc);

        delete [] m_x;
        delete [] m_y;
    }

    double getEvaluation(const double val) const
    {
        return gsl_spline_eval(m_spline, val, m_acc);
    }


private:
    double *m_y;
    double *m_x;

    gsl_interp_accel *m_acc;
    gsl_spline *m_spline;
};

// https://www.gnu.org/software/gsl/doc/html/interp.html
