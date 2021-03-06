/***********************************/
/*      SPICE Modeling for VPR     */
/*       Xifan TANG, EPFL/LSI      */
/***********************************/
#include <string.h>
#include <assert.h>

/* Include vpr structs*/
#include "util.h"
#include "arch_types.h"

/* SPICE Support Headers */
#include "read_xml_spice_util.h"

/* Subroutines to Initial and Free SPICE Structs */
void my_free(void* ptr) {
  if (NULL != ptr) {
    free(ptr);	
  }
  return;
}

void InitSpiceMeasParams(t_spice_meas_params* meas_params) {
  /* MeasParams*/
  meas_params->sim_num_clock_cycle = 2;
  meas_params->accuracy_type = SPICE_FRAC;
  meas_params->accuracy = 1e4;  

  meas_params->slew_upper_thres_pct_rise = 0.9;
  meas_params->slew_lower_thres_pct_rise = 0.1;

  meas_params->slew_upper_thres_pct_fall = 0.1;
  meas_params->slew_lower_thres_pct_fall = 0.9;

  meas_params->input_thres_pct_rise = 0.5;
  meas_params->output_thres_pct_rise = 0.5;

  meas_params->input_thres_pct_fall = 0.5;
  meas_params->output_thres_pct_fall = 0.5;

  return;
}

void FreeSpiceMeasParams() {
  return;
}

void InitSpiceStimulateParams(t_spice_stimulate_params* stimulate_params) {
  /* Clocks */ 
  stimulate_params->clock_slew_rise_time = 0.05;
  stimulate_params->clock_slew_rise_type = SPICE_FRAC;
  stimulate_params->clock_slew_fall_time = 0.05;
  stimulate_params->clock_slew_fall_type = SPICE_FRAC;

  stimulate_params->op_clock_freq = 0.;
  stimulate_params->prog_clock_freq = 0.;
  stimulate_params->sim_clock_freq_slack = 0.2;
  /* inputs */
  stimulate_params->input_slew_rise_time = 0.05;
  stimulate_params->input_slew_rise_type = SPICE_FRAC;
  stimulate_params->input_slew_fall_time = 0.05;
  stimulate_params->input_slew_fall_type = SPICE_FRAC;

  return;
}

void FreeSpiceStimulateParams() {
  return;
}

void InitSpiceVariationParams(t_spice_mc_variation_params* mc_variation_params) {
  mc_variation_params->variation_on = FALSE; 
  mc_variation_params->abs_variation = 0.; 
  mc_variation_params->num_sigma = 1; 
}

void FreeSpiceVariationParams() {
  return; 
}

void InitSpiceMonteCarloParams(t_spice_mc_params* mc_params) {
  /* Initialize the CMOS and RRAM variations */
  InitSpiceVariationParams(&(mc_params->cmos_variation));
  InitSpiceVariationParams(&(mc_params->rram_variation));
  
  return;
}

void FreeSpiceMonteCarloParams() {
  return;
}

void InitSpiceParams(t_spice_params* params) {
  /* Default Simulation Temperature */
  params->sim_temp = 25;

  params->post = FALSE;
  params->captab = TRUE;
  params->fast = TRUE;

  /* measure params */
  InitSpiceMeasParams(&(params->meas_params));

  /* stimulate params*/
  InitSpiceStimulateParams(&(params->stimulate_params));

  /* stimulate params*/
  InitSpiceMonteCarloParams(&(params->mc_params));

  return;
}

void FreeSpiceParams() {

  return;
}

void FreeSpiceModelNetlist(t_spice_model_netlist* spice_model_netlist) {
  my_free(spice_model_netlist->path);
  
  return;	
}

void FreeSpiceModelBuffer() {
    
  return;
}

void FreeSpiceModelPassGateLogic() {

  return;
}

void FreeSpiceModelPort(t_spice_model_port* spice_model_port) {
  my_free(spice_model_port->prefix);  

  return;
}

void FreeSpiceModelWireParam() {
  return;
}

void FreeSpiceModel(t_spice_model* spice_model) {
  int i;

  my_free(spice_model->name);
  my_free(spice_model->prefix);
  my_free(spice_model->model_netlist);
  
  spice_model->include_netlist = NULL;
  
  /* Free the buffers */
  FreeSpiceModelBuffer();
  FreeSpiceModelBuffer();
  my_free(spice_model->input_buffer);
  my_free(spice_model->output_buffer);
  spice_model->input_buffer = NULL;
  spice_model->output_buffer = NULL;

  FreeSpiceModelPassGateLogic();
  my_free(spice_model->pass_gate_logic);
  spice_model->pass_gate_logic = NULL;
  
  /* Free the ports */
  for (i = 0; i < spice_model->num_port; i++) {
    FreeSpiceModelPort(&(spice_model->ports[i]));
  }
  my_free(spice_model->ports);
  spice_model->ports = NULL;
  spice_model->num_port = 0;
	
  /* Free wire parameters */
  if (NULL != spice_model->wire_param) {
    FreeSpiceModelWireParam();
    my_free(spice_model->wire_param);
    spice_model->wire_param = NULL;
  }
	
  return;
}

void InitSpice(t_spice* spice) {
  spice->num_include_netlist = 0;
  spice->include_netlists = NULL;
  spice->num_spice_model = 0;
  spice->spice_models = NULL;
 
  return;
}

void FreeSpice(t_spice* spice) {
  int i;
  
  /* Free SPICE included netlists */
  for (i = 0; i < spice->num_include_netlist; i++) {
    FreeSpiceModelNetlist(&(spice->include_netlists[i]));
  }
  my_free(spice->include_netlists);
  spice->include_netlists = NULL;
  spice->num_include_netlist = 0;

  /* Free SPICE Models */
  for (i = 0; i < spice->num_spice_model; i++) {
    FreeSpiceModel(&(spice->spice_models[i]));
  }
  my_free(spice->spice_models);
  spice->spice_models = NULL;
  spice->num_spice_model = 0; 

  return; 
}

void FreeSpiceMuxArch(t_spice_mux_arch* spice_mux_arch) {
  my_free(spice_mux_arch->num_input_per_level);  
  my_free(spice_mux_arch->input_level);  
  my_free(spice_mux_arch->input_offset);  

  return;
}

static 
void FreeSramInfOrgz(t_sram_inf_orgz* sram_inf_orgz) {
  my_free(sram_inf_orgz->spice_model_name);

  return;
}

void FreeSramInf(t_sram_inf* sram_inf) {
  if (NULL != sram_inf->spice_sram_inf_orgz) {
    FreeSramInfOrgz(sram_inf->spice_sram_inf_orgz);
  }

  if (NULL != sram_inf->verilog_sram_inf_orgz) {
    FreeSramInfOrgz(sram_inf->verilog_sram_inf_orgz);
  }

  return;
}
