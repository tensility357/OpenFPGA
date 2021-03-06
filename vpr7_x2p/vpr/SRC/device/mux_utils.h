/**************************************************
 * This file includes only declaration for the 
 * functions in mux_utils.c
 * Please refer to the source file for more details
 *************************************************/
#ifndef MUX_UTILS_H
#define MUX_UTILS_H

#include <vector>

#include "linkedlist.h"
#include "circuit_library.h"
#include "mux_library.h"

bool valid_mux_implementation_num_inputs(const size_t& mux_size);

size_t find_mux_num_datapath_inputs(const CircuitLibrary& circuit_lib,
                                    const CircuitModelId& circuit_model,
                                    const size_t& mux_size);

size_t find_mux_implementation_num_inputs(const CircuitLibrary& circuit_lib,
                                          const CircuitModelId& circuit_model,
                                          const size_t& mux_size);

enum e_spice_model_structure find_mux_implementation_structure(const CircuitLibrary& circuit_lib,
					                                           const CircuitModelId& circuit_model,
						                                       const size_t& mux_size);

size_t find_treelike_mux_num_levels(const size_t& mux_size);

size_t find_multilevel_mux_branch_num_inputs(const size_t& mux_size,
                                             const size_t& mux_level);

std::vector<bool> build_mux_intermediate_buffer_location_map(const CircuitLibrary& circuit_lib, 
                                                             const CircuitModelId& circuit_model,
                                                             const size_t& num_mux_levels);

MuxLibrary convert_mux_arch_to_library(const CircuitLibrary& circuit_lib, t_llist* muxes_head);

size_t find_mux_num_reserved_config_bits(const CircuitLibrary& circuit_lib,
                                         const CircuitModelId& mux_model,
                                         const MuxGraph& mux_graph);

size_t find_mux_num_config_bits(const CircuitLibrary& circuit_lib,
                                const CircuitModelId& mux_model,
                                const MuxGraph& mux_graph, 
                                const e_sram_orgz& sram_orgz_type);

size_t find_mux_num_shared_config_bits(const CircuitLibrary& circuit_lib,
                                       const CircuitModelId& mux_model,
                                       const MuxGraph& mux_graph, 
                                       const e_sram_orgz& sram_orgz_type);

#endif
