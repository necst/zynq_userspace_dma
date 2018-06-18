#!/bin/bash

usage_str="provide exactly 2 arguments; USAGE: $0 <build|clean> <project>"

if [[ "$#" -ne 2 ]]; then
	echo $usage_str
	exit -1
fi

action=$1

project=$2

if [[ "$action" != "build" ]] && [[ "$action" != "clean" ]]; then
	echo $usage_str
	exit -1
fi

curdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

source ${curdir}/vivado_lib.sh

project_dir=${curdir}/${project}

if [[ ! -d "${project_dir}" ]]; then
	echo "$project_dir does not exist"
	exit -1
fi

hls_dir=${project_dir}/hls
vivado_dir=${project_dir}/vivado

if [[ ! -d "${vivado_dir}" ]]; then
	echo "${vivado_dir} does not exist"
	exit -1	
fi

if [[ "$action" == "build" ]]; then

	if [[ -e "${hls_dir}" ]] && [[ -d "${hls_dir}" ]]; then
		check_command vivado_hls
		run_command ${hls_dir} "vivado_hls -f hls.tcl"
	fi
	
	check_command vivado
	run_command "$vivado_dir" "vivado -mode batch -source bd.tcl"
	mv ${vivado_dir}/pynq_${project}/project_1.runs/impl_1/${project}_wrapper.bit ${curdir}

elif [[ "$action" == "clean" ]]; then

	if [[ -e "${hls_dir}" ]] && [[ -d "${hls_dir}" ]]; then
		clean "${hls_dir}" pynq_${project} *.log
	fi

	clean "${vivado_dir}" pynq_${project} *.log *.jou .Xil

fi

