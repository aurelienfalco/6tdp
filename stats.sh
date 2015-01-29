#!/bin/bash

v=seq
if [ "$1" != "" ]; then
	v="$1"
fi



i=1024
m=4096
nb_test=5
np=2
max_proc=16
n=6
nr=2
nc=3
# while [ "$i" -le "$m" ]; do
	seq_time=0
	for j in `seq 1 ${nb_test}`
	do
		ret=`make -s seq n=${np} nr=${nr} nc=${nc} s=$i `
		time=`echo "$ret" | grep 'time=' | grep -Eo '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?' `
		seq_time=$(echo "${time} + ${seq_time}" | bc)
	done
	while [ "$nr" -le "$n" ]; do
		# echo "nr = $nr"
		if (( $i % $nr == 0 ))
			then
			m=$(echo ${max_proc}/${nr} | bc)
			# echo "m = $m"
			nc=1
			while [ "$nc" -le "$m" ]; do
				# echo "nc = $nc"
				if (( $i % $nc == 0 ))
					then
					np=$(echo ${nr}*${nc} | bc)
					sum_time=0
					# parallel $np procs
					for j in `seq 1 ${nb_test}`
					do
						ret=`make -s $v n=${np} nr=${nr} nc=${nc} s=$i `
						# echo "$ret"
						time=`echo "$ret" | grep 'time=' | grep -Eo '[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?' `
						sum_time=$(echo "${time} + ${sum_time}" | bc)
					done
					mean=$(echo "scale=6;${seq_time}/${sum_time}" | bc)
					echo "$np $mean"
					echo "$np $mean" >> ${v}.data
				fi
				nc=$(($nc+1))
			done
		fi
		nr=$(($nr+1))
	done
	# echo "$mean_time2"
	# i=$(($i+256))
	np=$(($np+2))
# done
