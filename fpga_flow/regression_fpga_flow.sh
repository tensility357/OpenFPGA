#! /bin/bash
# Exit if error occurs
set -e
# Make sure a clear start
pwd_path="$PWD"
task_name="k6_N10_regression_0"
config_file="$PWD/configs/regression/${task_name}.conf" 
bench_txt="$PWD/benchmarks/List/test_modes.txt"
rpt_file="$PWD/csv_rpts/fpga_spice/${task_name}.csv"
task_file="$PWD/vpr_fpga_spice_task_lists/${task_name}"

verilog_path="${PWD}/regression_MCNC"

# FPGA-SPICE
rm -rf ${pwd_path}/results_regression
cd ${pwd_path}/scripts

perl rewrite_path_in_file.pl -i $config_file

# SRAM FPGA
# TT case 
perl fpga_flow.pl -conf ${config_file} -benchmark ${bench_txt} -rpt ${rpt_file} -N 10 -K 6 -ace_d 0.5 -remove_designs -multi_thread 1 -vpr_fpga_x2p_rename_illegal_port -vpr_fpga_verilog -vpr_fpga_verilog_dir $verilog_path -vpr_fpga_bitstream_generator -vpr_fpga_verilog_print_autocheck_top_testbench -vpr_fpga_verilog_include_timing -vpr_fpga_verilog_include_signal_init -vpr_fpga_verilog_formal_verification_top_netlist -fix_route_chan_width -vpr_fpga_verilog_include_icarus_simulator -end_flow_with_test -power

rm -rf ${pwd_path}/results
rm -rf $verilog_path
cd ${pwd_path}

echo "Netlists successfully generated and simulated"
exit 0
