#!/bin/bash

cpu_num=${1}

if [[ -z ${cpu_num} ]]
then
    echo "ERROR: No CPU (index) was specified." >&2
    exit 1
fi

if [[ ${cpu_num} != +([0-9]) ]]
then
    echo "ERROR: The supplied CPU index was not numerical." >&2
    exit 1
fi

printf "scaling_cur_freq = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/scaling_cur_freq)"\n"
if [[ -r /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/cpuinfo_cur_freq ]]
then
    printf "cpuinfo_cur_freq = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/cpuinfo_cur_freq)"\n"
else
    printf "cpuinfo_cur_freq = ACCESS_DENIED\n"
fi
# Hardware limits
printf "cpuinfo_min_freq = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/cpuinfo_min_freq)"\n"
printf "cpuinfo_max_freq = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/cpuinfo_max_freq)"\n"
# Driver limits
printf "scaling_min_freq = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/scaling_min_freq)"\n"
printf "scaling_max_freq = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/scaling_max_freq)"\n"

printf "scaling_driver = "$(cat /sys/devices/system/cpu/cpu${cpu_num}/cpufreq/scaling_driver)"\n"
exit


