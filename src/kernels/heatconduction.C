/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "heatconduction.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<heatconduction>()
{
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription("Compute thermal conductivity");
  params.addParam<MaterialPropertyName>("diffusion_coefficient_name",
                                        "thermal_conductivity",
                                        "Property name of the diffusivity (Default: thermal_conductivity)");
  params.addParam<MaterialPropertyName>("diffusion_coefficient_dT_name",
                                        "thermal_conductivity_dT",
                                        "Property name of the derivative of the diffusivity with respect "
                                        "to the variable (Default: thermal_conductivity_dT)");
  params.set<bool>("use_displaced_mesh") = true;
//  params.addRequiredCoupledVar("coupled", "Coupled variable (reaction status)");
  return params;
}

heatconduction::heatconduction(const InputParameters & parameters) :
    Diffusion(parameters),
    _dim(_subproblem.mesh().dimension()),
    _diffusion_coefficient(getMaterialProperty<Real>("diffusion_coefficient_name")),
    _diffusion_coefficient_dT(hasMaterialProperty<Real>("diffusion_coefficient_dT_name") ?
                              &getMaterialProperty<Real>("diffusion_coefficient_dT_name") : NULL)
//    _coupled_val(coupledValue("coupled"))
{
}

Real
heatconduction::computeQpResidual()
{  //std::cout << _coupled_val[_qp]<<std::endl;
  //return _diffusion_coefficient[_qp]*Diffusion::computeQpResidual() - _coupled_val[_qp];
  return _diffusion_coefficient[_qp]*Diffusion::computeQpResidual();
}

Real
heatconduction::computeQpJacobian()
{
  Real jac = _diffusion_coefficient[_qp] * Diffusion::computeQpJacobian();
  if (_diffusion_coefficient_dT)
    jac += (*_diffusion_coefficient_dT)[_qp] * _phi[_j][_qp] * Diffusion::computeQpResidual();
  return jac;
}
